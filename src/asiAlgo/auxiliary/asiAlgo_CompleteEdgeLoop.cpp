//-----------------------------------------------------------------------------
// Created on: 09 March 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
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
#include <asiAlgo_CompleteEdgeLoop.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>

//-----------------------------------------------------------------------------

asiAlgo_CompleteEdgeLoop::asiAlgo_CompleteEdgeLoop(const Handle(asiAlgo_AAG)& aag,
                                                   ActAPI_ProgressEntry       progress,
                                                   ActAPI_PlotterEntry        plotter)
: ActAPI_IAlgorithm(progress, plotter), m_aag(aag)
{}

//-----------------------------------------------------------------------------

bool asiAlgo_CompleteEdgeLoop::operator()(const int                   seedEdgeIndex,
                                          TColStd_PackedMapOfInteger& loopIndices)
{
  const TopoDS_Shape& model = m_aag->GetMasterCAD();

  // Build auxiliary maps.
  if ( m_vertexEdgeMap.IsEmpty() )
    TopExp::MapShapesAndAncestors(model, TopAbs_VERTEX, TopAbs_EDGE, m_vertexEdgeMap);
  //
  if ( m_edgeFaceMap.IsEmpty() )
    TopExp::MapShapesAndAncestors(model, TopAbs_EDGE,   TopAbs_FACE, m_edgeFaceMap);

  // Traverse topology graph to accumulate all next/previous edges.
  this->traverse(seedEdgeIndex, loopIndices);

  return true;
}

//-----------------------------------------------------------------------------

void asiAlgo_CompleteEdgeLoop::traverse(const int                   seedEdgeIndex,
                                        TColStd_PackedMapOfInteger& loopIndices) const
{
  const TopTools_IndexedMapOfShape& edgeMap = m_aag->RequestMapOfEdges();

  // Get edge by its 1-based index.
  const TopoDS_Edge& edge = TopoDS::Edge( edgeMap.FindKey(seedEdgeIndex) );
  //
  loopIndices.Add(seedEdgeIndex);

  // Get vertices for the seed edge.
  TopoDS_Vertex Vf, Vl;
  TopExp::Vertices(edge, Vf, Vl);

  // Get edges sharing the first and last vertices.
  TColStd_PackedMapOfInteger neighbors;
  //
  const TopTools_ListOfShape& prevEdges = m_vertexEdgeMap.FindFromKey(Vf);
  const TopTools_ListOfShape& nextEdges = m_vertexEdgeMap.FindFromKey(Vl);
  //
  this->addEdges(prevEdges, neighbors);
  this->addEdges(nextEdges, neighbors);
  //
  neighbors.Subtract(loopIndices); // Keep only non-traversed edges.
  //
  if ( neighbors.Extent() )
    loopIndices.Unite(neighbors);

  // Traverse recursively.
  for ( TColStd_MapIteratorOfPackedMapOfInteger nit(neighbors); nit.More(); nit.Next() )
  {
    const int eidx = nit.Key();
    //
    this->traverse(eidx, loopIndices);
  }
}

//-----------------------------------------------------------------------------

void asiAlgo_CompleteEdgeLoop::addEdges(const TopTools_ListOfShape& neighborEdges,
                                        TColStd_PackedMapOfInteger& result) const
{
  const TopTools_IndexedMapOfShape& edgeMap = m_aag->RequestMapOfEdges();

  for ( TopTools_ListIteratorOfListOfShape lit(neighborEdges); lit.More(); lit.Next() )
  {
    const TopoDS_Edge& nextEdge = TopoDS::Edge( lit.Value() );
    const int          eidx     = edgeMap.FindIndex(nextEdge);
    //
    if ( eidx && !result.Contains(eidx) )
    {
      // Check if this edge is a free edge.
      const TopTools_ListOfShape& faces = m_edgeFaceMap.FindFromKey(nextEdge);
      //
      if ( faces.Extent() == 1 )
        result.Add(eidx);
    }
  }
}
