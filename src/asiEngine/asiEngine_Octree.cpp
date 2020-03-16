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

// asiEngine includes
#include <asiEngine_IV.h>
#include <asiEngine_Part.h>
#include <asiEngine_Triangulation.h>

// asiData includes
#include <asiEngine_BuildOctreeFunc.h>

//-----------------------------------------------------------------------------

Handle(asiData_OctreeNode)
  asiEngine_Octree::CreateOctree(const Handle(ActAPI_INode)&       owner,
                                 const asiAlgo_CSG                 operation,
                                 const Handle(asiData_OctreeNode)& left,
                                 const Handle(asiData_OctreeNode)& right)
{
  // Build BVH to be used in the distance function. The BVH is constructed
  // by the corresponding service class (i.e., asiEngine_XXX), though
  // we do not store the constructed BVH in the owner Node (to avoid
  // side effects).
  Handle(asiAlgo_BVHFacets) bvh;
  //
  if ( operation == CSG_Primitive )
  {
    if ( owner->IsKind( STANDARD_TYPE(asiData_PartNode) ) )
    {
      bvh = asiEngine_Part(m_model).BuildBVH(false);
    }
    else if ( owner->IsKind( STANDARD_TYPE(asiData_TriangulationNode) ) )
    {
      bvh = asiEngine_Triangulation(m_model).BuildBVH(false);
    }
    else if ( owner->IsKind( STANDARD_TYPE(asiData_IVTopoItemNode) ) )
    {
      Handle(asiData_IVTopoItemNode)
        topoItemNode = Handle(asiData_IVTopoItemNode)::DownCast(owner);

      bvh = asiEngine_IV(m_model).BuildBVH(topoItemNode, false);
    }
    else
    {
      return nullptr; // Unexpected type of owner.
    }
  }

  // Create a new peristent Node.
  Handle(asiData_OctreeNode)
    node = Handle(asiData_OctreeNode)::DownCast( asiData_OctreeNode::Instance() );
  //
  m_model->GetOctreePartition()->AddNode(node);

  // Prepare name.
  TCollection_AsciiString nodeName("SVO ");
  nodeName += asiAlgo_CSGUtils::GetOperationName(operation);

  // Initialize.
  node->Init();
  node->SetUserFlags(NodeFlag_IsPresentedInPartView | NodeFlag_IsPresentationVisible);
  node->SetName(nodeName);
  node->SetOperation(operation);
  node->SetBVH(bvh);

  // Set references to the operand SVOs.
  if ( !left.IsNull() && !right.IsNull() )
  {
    node->ConnectReference(asiData_OctreeNode::PID_OpLeft,  left);
    node->ConnectReference(asiData_OctreeNode::PID_OpRight, right);
  }

  // Attach tree function.
  node->ConnectTreeFunction( asiData_OctreeNode::PID_BuildFunc,
                             asiEngine_BuildOctreeFunc::GUID(),
                             ActParamStream() << node->Parameter(asiData_OctreeNode::PID_BVH)
                                              << node->Parameter(asiData_OctreeNode::PID_DomainMinX)
                                              << node->Parameter(asiData_OctreeNode::PID_DomainMinY)
                                              << node->Parameter(asiData_OctreeNode::PID_DomainMinZ)
                                              << node->Parameter(asiData_OctreeNode::PID_DomainMaxX)
                                              << node->Parameter(asiData_OctreeNode::PID_DomainMaxY)
                                              << node->Parameter(asiData_OctreeNode::PID_DomainMaxZ)
                                              << node->Parameter(asiData_OctreeNode::PID_DomainIsCube)
                                              << node->Parameter(asiData_OctreeNode::PID_MinCellSize)
                                              << node->Parameter(asiData_OctreeNode::PID_MaxCellSize)
                                              << node->Parameter(asiData_OctreeNode::PID_Precision)
                                              << node->Parameter(asiData_OctreeNode::PID_Operation)
                                              << node->Parameter(asiData_OctreeNode::PID_OpLeft)
                                              << node->Parameter(asiData_OctreeNode::PID_OpRight),
                             ActParamStream() << node->Parameter(asiData_OctreeNode::PID_Octree)
                                              << node->Parameter(asiData_OctreeNode::PID_NumElements) );

  // Set as child for the owner Node.
  owner->AddChildNode(node);

  return node;
}
