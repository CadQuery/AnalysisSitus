//-----------------------------------------------------------------------------
// Created on: 27 February 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiEngine_Octree.h>

// asiData includes
#include <asiEngine_BuildOctreeFunc.h>

//-----------------------------------------------------------------------------

Handle(asiData_OctreeNode)
  asiEngine_Octree::CreateOctree(const Handle(ActAPI_INode)& owner,
                                 const int                   ownerBVH)
{
  Handle(asiData_OctreeNode)
    node = Handle(asiData_OctreeNode)::DownCast( asiData_OctreeNode::Instance() );
  //
  m_model->GetOctreePartition()->AddNode(node);

  // Initialize.
  node->Init();
  node->SetUserFlags(NodeFlag_IsPresentedInPartView | NodeFlag_IsPresentationVisible);
  node->SetName("SVO");

  // Attach tree function.
  node->ConnectTreeFunction( asiData_OctreeNode::PID_BuildFunc,
                             asiEngine_BuildOctreeFunc::GUID(),
                             ActParamStream() << owner->Parameter(ownerBVH)
                                              << node->Parameter(asiData_OctreeNode::PID_DomainMinX)
                                              << node->Parameter(asiData_OctreeNode::PID_DomainMinY)
                                              << node->Parameter(asiData_OctreeNode::PID_DomainMinZ)
                                              << node->Parameter(asiData_OctreeNode::PID_DomainMaxX)
                                              << node->Parameter(asiData_OctreeNode::PID_DomainMaxY)
                                              << node->Parameter(asiData_OctreeNode::PID_DomainMaxZ)
                                              << node->Parameter(asiData_OctreeNode::PID_DomainIsCube)
                                              << node->Parameter(asiData_OctreeNode::PID_MinCellSize)
                                              << node->Parameter(asiData_OctreeNode::PID_MaxCellSize)
                                              << node->Parameter(asiData_OctreeNode::PID_Precision),
                             ActParamStream() << node->Parameter(asiData_OctreeNode::PID_Octree)
                                              << node->Parameter(asiData_OctreeNode::PID_NumElements) );

  // Set as child for the owner Node.
  owner->AddChildNode(node);

  return node;
}

//-----------------------------------------------------------------------------

Handle(asiData_OctreeNode)
  asiEngine_Octree::FindOctree(const Handle(ActAPI_INode)& owner,
                               const int                   ownerBVH,
                               const bool                  create)
{
  if ( owner.IsNull() || !owner->IsWellFormed() )
    return nullptr;

  Handle(asiData_OctreeNode) octreeNode;

  /* Regardless of the owner type, octree is stored as a child Node */
  for ( Handle(ActAPI_IChildIterator) cit = owner->GetChildIterator(); cit->More(); cit->Next() )
  {
    octreeNode = Handle(asiData_OctreeNode)::DownCast( cit->Value() );

    if ( octreeNode.IsNull() || !octreeNode->IsWellFormed() )
      continue;
  }

  // Create if requested.
  if ( octreeNode.IsNull() && create )
  {
    // Create Octree Node.
    octreeNode = this->CreateOctree(owner, ownerBVH);
  }

  return octreeNode;
}
