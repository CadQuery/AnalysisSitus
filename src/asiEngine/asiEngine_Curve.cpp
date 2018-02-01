//-----------------------------------------------------------------------------
// Created on: 29 January 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2017-2018, Sergey Slyadnev
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
//    * Neither the name of Sergey Slyadnev nor the
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
#include <asiEngine_Curve.h>

//-----------------------------------------------------------------------------

Handle(asiData_CurveNode)
  asiEngine_Curve::Create(const TCollection_ExtendedString& name,
                          const Handle(ActAPI_INode)&       parent)
{
  Handle(ActAPI_INode) node = asiData_CurveNode::Instance();
  m_model->GetCurvePartition()->AddNode(node);

  // Initialize
  Handle(asiData_CurveNode) curve = Handle(asiData_CurveNode)::DownCast(node);
  curve->Init();
  curve->SetName(name);

  // Set as child
  parent->AddChildNode(curve);

  return curve;
}

//-----------------------------------------------------------------------------

Handle(asiData_CurvatureCombsNode)
  asiEngine_Curve::CreateCurvatureCombs(const Handle(asiData_CurveNode)& parent,
                                        const int                        numPts,
                                        const double                     scaleFactor)
{
  Handle(ActAPI_INode) node = asiData_CurvatureCombsNode::Instance();
  m_model->GetCurvatureCombsPartition()->AddNode(node);

  // Initialize
  Handle(asiData_CurvatureCombsNode) combs = Handle(asiData_CurvatureCombsNode)::DownCast(node);
  combs->Init();
  combs->SetName("Curvature combs");
  combs->SetNumPoints(numPts);
  combs->SetScaleFactor(scaleFactor);
  //
  combs->ConnectReference(asiData_CurvatureCombsNode::PID_RefCurve, parent);

  // Set as child
  parent->AddChildNode(combs);

  return combs;
}

//-----------------------------------------------------------------------------

Handle(asiData_CurvatureCombsNode)
  asiEngine_Curve::CreateOrUpdateCurvatureCombs(const Handle(asiData_CurveNode)& parent,
                                                const int                        numPts,
                                                const double                     scaleFactor)
{
  Handle(ActAPI_IPartition)
    combsPartition = m_model->GetCurvatureCombsPartition();

  Handle(ActAPI_INode) node = combsPartition->GetNode(1);
  //
  if ( node.IsNull() )
  {
    node = this->CreateCurvatureCombs(parent, numPts, scaleFactor);
  }
  else
  {
    Handle(asiData_CurvatureCombsNode)
      combsNode = Handle(asiData_CurvatureCombsNode)::DownCast(node);

    // Update
    combsNode->SetNumPoints(numPts);
    combsNode->SetScaleFactor(scaleFactor);
  }

  return Handle(asiData_CurvatureCombsNode)::DownCast(node);
}

//-----------------------------------------------------------------------------

void asiEngine_Curve::Clean_All(const Handle(asiData_CurveNode)& parent)
{
  this->_cleanChildren(parent);
}

//-----------------------------------------------------------------------------

//! Removes all child Nodes for the given parent.
//! \param[in] parent parent Node to clean up children for.
void asiEngine_Curve::_cleanChildren(const Handle(ActAPI_INode)& parent)
{
  Handle(ActAPI_HNodeList) nodesToDelete = new ActAPI_HNodeList;

  // Loop over direct children of a Surfaces Node
  for ( Handle(ActAPI_IChildIterator) cit = parent->GetChildIterator(); cit->More(); cit->Next() )
  {
    Handle(ActAPI_INode) child_n = cit->Value();

    // Check if the given Node is consistent
    if ( child_n.IsNull() || !child_n->IsWellFormed() )
      continue;

    // Set Node for deletion
    nodesToDelete->Append(child_n);
  }

  // Delete all Nodes queued for removal
  for ( ActAPI_NodeList::Iterator nit( *nodesToDelete.operator->() ); nit.More(); nit.Next() )
    m_model->DeleteNode( nit.Value()->GetId() );
}
