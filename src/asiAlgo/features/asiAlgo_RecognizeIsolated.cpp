//-----------------------------------------------------------------------------
// Created on: 11 June 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2016-present, Sergey Slyadnev
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
#include <asiAlgo_RecognizeIsolated.h>

// asiAlgo includes
#include <asiAlgo_AAG.h>
#include <asiAlgo_AAGIterator.h>

// OpenCascade includes
#include <BRep_Builder.hxx>
#include <ShapeAnalysis.hxx>
#include <TopExp_Explorer.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiAlgo_RecognizeIsolated::asiAlgo_RecognizeIsolated(const TopoDS_Shape&  shape,
                                                     ActAPI_ProgressEntry progress,
                                                     ActAPI_PlotterEntry  plotter)
: asiAlgo_Recognizer(shape, nullptr, progress, plotter)
{}

//-----------------------------------------------------------------------------

asiAlgo_RecognizeIsolated::asiAlgo_RecognizeIsolated(const TopoDS_Shape&        shape,
                                                     const Handle(asiAlgo_AAG)& aag,
                                                     ActAPI_ProgressEntry       progress,
                                                     ActAPI_PlotterEntry        plotter)
: asiAlgo_Recognizer(shape, aag, progress, plotter)
{}

//-----------------------------------------------------------------------------

asiAlgo_RecognizeIsolated::asiAlgo_RecognizeIsolated(const Handle(asiAlgo_AAG)& aag,
                                                     ActAPI_ProgressEntry       progress,
                                                     ActAPI_PlotterEntry        plotter)
: asiAlgo_Recognizer(aag->GetMasterShape(), aag, progress, plotter)
{}

//-----------------------------------------------------------------------------

bool asiAlgo_RecognizeIsolated::Perform(const asiAlgo_Feature& seeds)
{
#if defined COUT_DEBUG
  TIMER_NEW
  TIMER_GO
#endif

  // Clean up the result.
  m_result.Clear();

  if ( m_aag.IsNull() )
  {
#if defined COUT_DEBUG
    TIMER_NEW
    TIMER_GO
#endif

    // Build master AAG. We allow "smooth" qualifier for dihedral edges
    // as generally we do not care of this heuristic when detecting
    // isolated features. Such features can represent both cavities or
    // local extrusions.
    m_aag = new asiAlgo_AAG(m_master, true);

#if defined COUT_DEBUG
    TIMER_FINISH
    TIMER_COUT_RESULT_MSG("Construct AAG")
#endif
  }

  // Initialize iterator to loop over the preselected seed faces.
  Handle(asiAlgo_AAGSetIterator)
    seed_it = new asiAlgo_AAGSetIterator(m_aag, seeds);

  // Iterate over the seeds.
  for ( ; seed_it->More(); seed_it->Next() )
  {
    const t_topoId     baseFaceId = seed_it->GetFaceId();
    const TopoDS_Face& baseFace   = m_aag->GetFace(baseFaceId);

    // Get edges where features grow and the outer edges
    // restricting the finder.
    std::vector< TopoDS_Edge >              outerEdges;
    std::vector< std::vector<TopoDS_Edge> > innerEdges;
    //
    this->getFaceEdges(baseFace, outerEdges, innerEdges);

    // Get all faces which should not pass to the isolated features.
    asiAlgo_Feature neighborsOverOuterEdges;
    for ( auto eit = outerEdges.cbegin(); eit != outerEdges.cend(); ++eit )
      neighborsOverOuterEdges.Unite( m_aag->GetNeighborsThru(baseFaceId, *eit) );

    // Eliminate base face from AAG to check the number of connected components.
    m_aag->PushSubgraphX(baseFaceId);
    {
      // Get all connected components
      std::vector<asiAlgo_Feature> ccomps;
      m_aag->GetConnectedComponents(ccomps);

      if ( !ccomps.empty() )
      {
        // For each connected component, check whether it contains candidate
        // faces. If so, we are done.
        for ( auto cit = ccomps.cbegin(); cit != ccomps.cend(); ++cit )
        {
          const asiAlgo_Feature& ccomp = *cit;

          // Isolated features should be adjacent to the base face via
          // its internal contours. But that's not enough. It may happen
          // that a face set adjacent to the outer contour propagates via
          // adjacency to the internal contours. Therefore, it is necessary
          // to check that the connected component does not contain faces
          // adjacent to the base one via its outer contour. Moreover,
          // full inner contour should be covered by adjacent faces.

          if ( ccomp.HasIntersection(neighborsOverOuterEdges) )
            continue;

          if ( !this->checkPresence(innerEdges, ccomp) )
            continue;

          m_result.ids.Unite(ccomp);
        }
      }
    }
    m_aag->PopSubgraph();

    // Handle cancellation request: exit safely.
    if ( m_progress.IsCancelling() )
      return false;
  }

  // Populate the result with the transient shape pointers.
  for ( asiAlgo_Feature::Iterator mit(m_result.ids); mit.More(); mit.Next() )
    m_result.faces.Add( m_aag->GetFace( mit.Key() ) );

#if defined COUT_DEBUG
  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Recognize isolated features")
#endif

  return true; // Success.
}

//-----------------------------------------------------------------------------

void asiAlgo_RecognizeIsolated::getFaceEdges(const TopoDS_Face&                       face,
                                             std::vector< TopoDS_Edge >&              outerEdges,
                                             std::vector< std::vector<TopoDS_Edge> >& innerEdges) const
{
  outerEdges.clear();
  innerEdges.clear();

  const TopoDS_Wire& outerWire = ShapeAnalysis::OuterWire(face);

  // Take outer edges.
  for ( TopExp_Explorer expWE(outerWire, TopAbs_EDGE); expWE.More(); expWE.Next() )
  {
    outerEdges.push_back( TopoDS::Edge( expWE.Current() ) );
  }

  // Explore inner wires to collect inner edges.
  for ( TopExp_Explorer expFW(face, TopAbs_WIRE); expFW.More(); expFW.Next() )
  {
    const TopoDS_Shape& currentWire = expFW.Current();
    //
    if ( currentWire.IsSame(outerWire) )
      continue;

    std::vector<TopoDS_Edge> edgesOnWire;

    // For interior wires, take their edges
    for ( TopExp_Explorer expWE(currentWire, TopAbs_EDGE); expWE.More(); expWE.Next() )
    {
      edgesOnWire.push_back( TopoDS::Edge( expWE.Current() ) );
    }

    innerEdges.push_back(edgesOnWire);
  }
}

//-----------------------------------------------------------------------------

bool asiAlgo_RecognizeIsolated::checkPresence(const std::vector< std::vector<TopoDS_Edge> >& innerEdges,
                                              const asiAlgo_Feature&                         groupToCheck) const
{
  NCollection_IndexedMap<TopoDS_Edge> edges;

  TopoDS_Compound comp;
  BRep_Builder builder;
  builder.MakeCompound(comp);

  // Iterate of the faces in the group of interest.
  for ( asiAlgo_Feature::Iterator fit(groupToCheck); fit.More() ; fit.Next() )
  {
    const TopoDS_Face& face = m_aag->GetFace( fit.Key() );

    TopTools_IndexedMapOfShape faceEdges;
    TopExp::MapShapes(face, TopAbs_EDGE, faceEdges);

    // Insert both orientations since hasher uses "IsEqual".
    for ( TopTools_IndexedMapOfShape::Iterator eit(faceEdges); eit.More(); eit.Next() )
    {
      const TopoDS_Edge& edge = TopoDS::Edge( eit.Value() );
      edges.Add( TopoDS::Edge( edge.Oriented(TopAbs_FORWARD) ) );
      edges.Add( TopoDS::Edge( edge.Oriented(TopAbs_REVERSED) ) );

      builder.Add(comp, edge);
    }
  }

  bool isPresent = false;
  for ( auto innerIt = innerEdges.cbegin(); innerIt != innerEdges.cend(); ++innerIt )
  {
    const std::vector<TopoDS_Edge>& group = *innerIt;

    isPresent = true;
    for ( auto eit = group.cbegin(); eit != group.cend(); ++eit )
    {
      const TopoDS_Edge& edge = *eit;

      if ( !edges.Contains(edge) )
      {
        isPresent = false;
        break;
      }
    }

    if ( isPresent )
      break;
  }

  return isPresent;
}
