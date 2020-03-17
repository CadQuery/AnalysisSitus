//-----------------------------------------------------------------------------
// Created on: 14 August 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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
#include <asiEngine_Triangulation.h>

// asiVisu includes
#include <asiVisu_TriangulationNodeInfo.h>

// asiAlgo includes
#include <asiAlgo_CheckDeviations.h>
#include <asiAlgo_Timer.h>

//-----------------------------------------------------------------------------

Handle(asiData_TriangulationNode) asiEngine_Triangulation::CreateTriangulation()
{
  // Add Triangulation Node to Partition
  Handle(asiData_TriangulationNode)
    triangulation_n = Handle(asiData_TriangulationNode)::DownCast( asiData_TriangulationNode::Instance() );
  //
  m_model->GetTriangulationPartition()->AddNode(triangulation_n);

  // Initialize
  triangulation_n->Init();
  triangulation_n->SetUserFlags(NodeFlag_IsPresentedInPartView | NodeFlag_IsPresentationVisible);
  triangulation_n->SetName("Triangulation");

  // Return the just created Node
  return triangulation_n;
}

//-----------------------------------------------------------------------------

Handle(asiAlgo_BVHFacets) asiEngine_Triangulation::BuildBVH(const bool store)
{
  // Get Triangulation Node
  Handle(asiData_TriangulationNode) tris_n = m_model->GetTriangulationNode();

  // Build BVH for facets
  Handle(asiAlgo_BVHFacets)
    bvh = new asiAlgo_BVHFacets(tris_n->GetTriangulation(),
                                asiAlgo_BVHFacets::Builder_Binned,
                                m_progress,
                                m_plotter);

  if ( store) // Store in OCAF
    tris_n->SetBVH(bvh);

  return bvh;
}

//-----------------------------------------------------------------------------

bool
  asiEngine_Triangulation::CheckDeviation(const Handle(asiData_IVPointSetNode)& pcNode)
{
  Handle(asiData_DeviationNode) devNode;
  return this->CheckDeviation(pcNode, devNode);
}

//-----------------------------------------------------------------------------

bool
  asiEngine_Triangulation::CheckDeviation(const Handle(asiData_IVPointSetNode)& pcNode,
                                          Handle(asiData_DeviationNode)&        devNode)
{
  // Get Triangulation Node.
  Handle(asiData_TriangulationNode) trisNode = m_model->GetTriangulationNode();

  // Check deviations.
  asiAlgo_CheckDeviations checkDeviations( pcNode->GetPoints(),
                                           m_progress,
                                           m_plotter );
  //
  if ( !checkDeviations.Perform( trisNode->GetTriangulation() ) )
    return false;

  // Create Deviation Node.
  Handle(ActAPI_INode) devNodeBase = asiData_DeviationNode::Instance();
  m_model->GetDeviationPartition()->AddNode(devNodeBase);

  // Initialize.
  devNode = Handle(asiData_DeviationNode)::DownCast(devNodeBase);
  //
  devNode->Init();
  devNode->SetName("Deviation");

  // Store deviations.
  devNode->SetMeshWithScalars( checkDeviations.GetResult() );

  // Add Deviation Node as a child of the Triangulation Node.
  trisNode->AddChildNode(devNode);

  return true;
}

//-----------------------------------------------------------------------------

void asiEngine_Triangulation::GetHighlightedFacets(TColStd_PackedMapOfInteger& facetIndices)
{
  // Get actual selection
  const asiVisu_ActualSelection&          sel     = m_prsMgr->GetCurrentSelection();
  const Handle(asiVisu_CellPickerResult)& pickRes = sel.GetCellPickerResult(SelectionNature_Persistent);
  //
  asiVisu_TriangulationNodeInfo*
    nodeInfo = asiVisu_TriangulationNodeInfo::Retrieve( pickRes->GetPickedActor() );
  //
  if ( !nodeInfo )
    return;

  facetIndices = pickRes->GetPickedElementIds();
}
