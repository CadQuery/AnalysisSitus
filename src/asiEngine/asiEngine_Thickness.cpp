//-----------------------------------------------------------------------------
// Created on: 03 April 2020
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
#include <asiEngine_Thickness.h>

// asiEngine includes
#include <asiEngine_CheckThicknessFunc.h>

// asiAlgo includes
#include <asiAlgo_MeshMerge.h>

//-----------------------------------------------------------------------------

Handle(asiData_ThicknessNode)
  asiEngine_Thickness::CreateThickness(const Handle(ActAPI_INode)& owner)
{
  Handle(Poly_Triangulation) mesh;

  // Resolve the owner.
  if ( owner->IsKind( STANDARD_TYPE(asiData_PartNode) ) )
  {
    // Merge facets.
    asiAlgo_MeshMerge meshMerge( Handle(asiData_PartNode)::DownCast(owner)->GetShape() );
    //
    mesh = meshMerge.GetResultPoly()->GetTriangulation();
  }
  else if ( owner->IsKind( STANDARD_TYPE(asiData_IVTopoItemNode) ) )
  {
    // Merge facets.
    asiAlgo_MeshMerge meshMerge( Handle(asiData_IVTopoItemNode)::DownCast(owner)->GetShape() );
    //
    mesh = meshMerge.GetResultPoly()->GetTriangulation();
  }
  else if ( owner->IsKind( STANDARD_TYPE(asiData_TriangulationNode) ) )
  {
    mesh = Handle(asiData_TriangulationNode)::DownCast(owner)->GetTriangulation();
  }
  else
  {
    return nullptr; // Unexpected type of owner.
  }

  // Create a new peristent Node.
  Handle(asiData_ThicknessNode)
    node = Handle(asiData_ThicknessNode)::DownCast( asiData_ThicknessNode::Instance() );
  //
  m_model->GetThicknessPartition()->AddNode(node);

  // Prepare name.
  TCollection_AsciiString nodeName("Thickness");

  // Initialize.
  node->Init();
  node->SetUserFlags(NodeFlag_IsPresentedInPartView | NodeFlag_IsPresentationVisible);
  node->SetName(nodeName);
  node->SetMesh(mesh);

  // Attach tree function.
  node->ConnectTreeFunction( asiData_ThicknessNode::PID_CheckThicknessFunc,
                             asiEngine_CheckThicknessFunc::GUID(),
                             ActParamStream() << node->Parameter(asiData_ThicknessNode::PID_Mesh)
                                              << node->Parameter(asiData_ThicknessNode::PID_IsCustomDir)
                                              << node->Parameter(asiData_ThicknessNode::PID_Dx)
                                              << node->Parameter(asiData_ThicknessNode::PID_Dy)
                                              << node->Parameter(asiData_ThicknessNode::PID_Dz),
                             ActParamStream() << node->Parameter(asiData_ThicknessNode::PID_ScalarMin)
                                              << node->Parameter(asiData_ThicknessNode::PID_ScalarMax) );

  // Set as child for the owner Node.
  owner->AddChildNode(node);

  return node;
}
