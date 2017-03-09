//-----------------------------------------------------------------------------
// Created on: 09 March 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_CompleteEdgeLoop.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param aag      [in] Attributed Adjacency Graph (AAG).
//! \param progress [in] Progress Notifier.
//! \param plotter  [in] Imperative Plotter.
asiAlgo_CompleteEdgeLoop::asiAlgo_CompleteEdgeLoop(const Handle(asiAlgo_AAG)& aag,
                                                   ActAPI_ProgressEntry       progress,
                                                   ActAPI_PlotterEntry        plotter)
: ActAPI_IAlgorithm(progress, plotter), m_aag(aag)
{}

//-----------------------------------------------------------------------------

//! Attempts to construct a loop of edges starting from the given seed edge.
//! \param seedEdgeIndex [in]  1-based index of a seed edge.
//! \param loopIndices   [out] all edges forming a loop.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_CompleteEdgeLoop::operator()(const int                   seedEdgeIndex,
                                          TColStd_PackedMapOfInteger& loopIndices)
{
  const TopoDS_Shape& model = m_aag->GetMasterCAD();

  // Build auxiliary maps
  TopExp::MapShapesAndAncestors(model, TopAbs_VERTEX, TopAbs_EDGE, m_vertexEdgeMap);
  TopExp::MapShapesAndAncestors(model, TopAbs_EDGE,   TopAbs_FACE, m_edgeFaceMap);

  // Traverse topology graph to accumulate all next/previous edges
  this->traverse(seedEdgeIndex, loopIndices);

  return true;
}

//-----------------------------------------------------------------------------

//! Traverses next/previous edges for the given seed.
//! \param seedEdgeIndex [in]  1-based index of the seed edge.
//! \param loopIndices   [out] accumulated indices of loop edges.
void asiAlgo_CompleteEdgeLoop::traverse(const int                   seedEdgeIndex,
                                        TColStd_PackedMapOfInteger& loopIndices) const
{
  const TopTools_IndexedMapOfShape& edgeMap = m_aag->GetMapOfEdges();

  // Get edge by its 1-based index
  const TopoDS_Edge& edge = TopoDS::Edge( edgeMap.FindKey(seedEdgeIndex) );
  //
  loopIndices.Add(seedEdgeIndex);

  // Get vertices for the seed edge
  TopoDS_Vertex Vf, Vl;
  TopExp::Vertices(edge, Vf, Vl);

  // Get edges sharing the first and last vertices
  TColStd_PackedMapOfInteger neighbors;
  //
  const TopTools_ListOfShape& prevEdges = m_vertexEdgeMap.FindFromKey(Vf);
  const TopTools_ListOfShape& nextEdges = m_vertexEdgeMap.FindFromKey(Vl);
  //
  this->addEdges(prevEdges, neighbors);
  this->addEdges(nextEdges, neighbors);
  //
  neighbors.Subtract(loopIndices); // Keep only non-traversed edges
  //
  if ( neighbors.Extent() )
    loopIndices.Unite(neighbors);

  // Traverse recursively
  for ( TColStd_MapIteratorOfPackedMapOfInteger nit(neighbors); nit.More(); nit.Next() )
  {
    const int eidx = nit.Key();
    //
    this->traverse(eidx, loopIndices);
  }
}

//-----------------------------------------------------------------------------

//! From the given collection of neighbor edges, this method selects those
//! which were not iterated, and adds them to the output collection.
//! \param neighborEdges [in]  edges to traverse.
//! \param result        [out] accumulated indices of loop edges.
void asiAlgo_CompleteEdgeLoop::addEdges(const TopTools_ListOfShape& neighborEdges,
                                        TColStd_PackedMapOfInteger& result) const
{
  const TopTools_IndexedMapOfShape& edgeMap = m_aag->GetMapOfEdges();

  for ( TopTools_ListIteratorOfListOfShape lit(neighborEdges); lit.More(); lit.Next() )
  {
    const TopoDS_Edge& nextEdge = TopoDS::Edge( lit.Value() );
    const int          eidx     = edgeMap.FindIndex(nextEdge);
    //
    if ( eidx && !result.Contains(eidx) )
    {
      // Check if this edge is a free edge
      const TopTools_ListOfShape& faces = m_edgeFaceMap.FindFromKey(nextEdge);
      //
      if ( faces.Extent() == 1 )
        result.Add(eidx);
    }
  }
}
