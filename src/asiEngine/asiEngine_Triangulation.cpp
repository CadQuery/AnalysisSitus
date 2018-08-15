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
#include <asiEngine_Triangulation.h>

// asiVisu includes
#include <asiVisu_TriangulationNodeInfo.h>

// asiAlgo includes
#include <asiAlgo_Timer.h>

// Active Data includes
#include <ActData_UniqueNodeName.h>

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
  triangulation_n->SetName("Triangulation");

  // Return the just created Node
  return triangulation_n;
}

//-----------------------------------------------------------------------------

void asiEngine_Triangulation::BuildBVH()
{
  // Get Triangulation Node
  Handle(asiData_TriangulationNode) tris_n = m_model->GetTriangulationNode();

  // Build BVH for facets
  Handle(asiAlgo_BVHFacets)
    bvh = new asiAlgo_BVHFacets(tris_n->GetTriangulation(),
                                asiAlgo_BVHFacets::Builder_Binned,
                                m_progress,
                                m_plotter);

  // Store in OCAF
  tris_n->SetBVH(bvh);
}

//-----------------------------------------------------------------------------

void asiEngine_Triangulation::GetHighlightedFacets(TColStd_PackedMapOfInteger& facetIndices)
{
  // Get actual selection
  const asiVisu_ActualSelection& sel     = m_prsMgr->GetCurrentSelection();
  const asiVisu_PickResult&      pickRes = sel.PickResult(SelectionNature_Pick);
  //
  asiVisu_TriangulationNodeInfo*
    nodeInfo = asiVisu_TriangulationNodeInfo::Retrieve( pickRes.GetPickedActor() );
  //
  if ( !nodeInfo )
    return;

  facetIndices = pickRes.GetPickedElementIds();
}
