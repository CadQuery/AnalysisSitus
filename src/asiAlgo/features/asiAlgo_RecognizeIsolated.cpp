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
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Wire.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #pragma message("===== warning: DRAW_DEBUG is enabled")
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
: asiAlgo_Recognizer(shape,
                     aag,
                     progress,
                     plotter)
{}

//-----------------------------------------------------------------------------

asiAlgo_RecognizeIsolated::asiAlgo_RecognizeIsolated(const Handle(asiAlgo_AAG)& aag,
                                                     ActAPI_ProgressEntry       progress,
                                                     ActAPI_PlotterEntry        plotter)
: asiAlgo_Recognizer(aag->GetMasterShape(),
                     aag,
                     progress,
                     plotter)
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

  /* ========================
   *  Check for separability.
   * ======================== */

  const int c0 = m_aag->GetConnectedComponentsNb();
  int       c1 = 0;

  // Check the separability property.
  m_aag->PushSubgraphX(seeds);
  {
    c1 = m_aag->GetConnectedComponentsNb();
  }
  m_aag->PopSubgraph();

  if ( c1 >= c0 + 1 )
    m_progress.SendLogMessage(LogInfo(Normal) << "Inner feature looks separable.");
  else
    m_progress.SendLogMessage(LogWarn(Normal) << "Inner feature is not separable.");

  /* ==========================
   *  Get inner/outer contours.
   * ========================== */

  std::vector< TopoDS_Edge >              outerEdges;
  std::vector< std::vector<TopoDS_Edge> > innerEdges;
  //
  this->extractContours(seeds, outerEdges, innerEdges);

  /* ===========================
   *  Check for suppressibility.
   * =========================== */

  // Get all faces which should not pass to the isolated features.
  asiAlgo_Feature neighborsOverOuterEdges;
  for ( asiAlgo_Feature::Iterator fit(seeds); fit.More(); fit.Next() )
  {
    const int baseFaceId = fit.Key();
    //
    for ( auto eit = outerEdges.cbegin(); eit != outerEdges.cend(); ++eit )
    {
      neighborsOverOuterEdges.Unite( m_aag->GetNeighborsThru(baseFaceId, *eit) );
    }
  }

  // Eliminate base faces from AAG to check connected components.
  m_aag->PushSubgraphX(seeds);
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

        if ( !this->checkInnerBoundaryInclusion(innerEdges, ccomp) )
          continue;

        m_result.ids.Unite(ccomp);
      }
    }
  }
  m_aag->PopSubgraph();

  /* ==========
   *  Finalize.
   * ========== */

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

#if defined DRAW_DEBUG
    m_plotter.DRAW_SHAPE(expWE.Current(), Color_Blue, 1., true, "outerEdges");
#endif
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

#if defined DRAW_DEBUG
      m_plotter.DRAW_SHAPE(expWE.Current(), Color_Red, 1., true, "innerEdges");
#endif
    }

    innerEdges.push_back(edgesOnWire);
  }
}

//-----------------------------------------------------------------------------

bool asiAlgo_RecognizeIsolated::checkInnerBoundaryInclusion(const std::vector< std::vector<TopoDS_Edge> >& innerEdges,
                                                            const asiAlgo_Feature&                         groupToCheck) const
{
  // Collect all edges of the group in question.
  NCollection_IndexedMap<TopoDS_Edge> allEdgesInGroup;
  //
  for ( asiAlgo_Feature::Iterator fit(groupToCheck); fit.More() ; fit.Next() )
  {
    TopTools_IndexedMapOfShape faceEdges;
    TopExp::MapShapes(m_aag->GetFace( fit.Key() ), TopAbs_EDGE, faceEdges);

    // Insert both orientations since hasher uses "IsEqual".
    for ( TopTools_IndexedMapOfShape::Iterator eit(faceEdges); eit.More(); eit.Next() )
    {
      const TopoDS_Edge& edge = TopoDS::Edge( eit.Value() );
      allEdgesInGroup.Add( TopoDS::Edge( edge.Oriented(TopAbs_FORWARD) ) );
      allEdgesInGroup.Add( TopoDS::Edge( edge.Oriented(TopAbs_REVERSED) ) );
    }
  }

#if defined DRAW_DEBUG
  m_plotter.DRAW_SHAPE(comp, Color_Yellow, 1., true, "allEdgesInGroup");
#endif

  // Check if the group in question contains all the inner edges.
  bool isPresent = false;
  for ( auto innerIt = innerEdges.cbegin(); innerIt != innerEdges.cend(); ++innerIt )
  {
    const std::vector<TopoDS_Edge>& innerContour = *innerIt;

    // At least one inner contour should be present in the edge set for the
    // connected component `C`. For a base face with multiple holes, it is
    // invalid to expect that all inner edges are a subset for the connected
    // component being tested.
    isPresent = true;
    for ( auto eit = innerContour.cbegin(); eit != innerContour.cend(); ++eit )
    {
      const TopoDS_Edge& innerEdge = *eit;

      if ( !allEdgesInGroup.Contains(innerEdge) )
      {
        isPresent = false;

#if defined DRAW_DEBUG
        m_plotter.DRAW_SHAPE(innerEdge, Color_Blue, 1., true, "nonPresent");
#endif
        break;
      }
    }
    //
    if ( isPresent )
      break;
  }

  return isPresent;
}

//-----------------------------------------------------------------------------

bool
  asiAlgo_RecognizeIsolated::extractContours(const asiAlgo_Feature&                   seeds,
                                             std::vector< TopoDS_Edge >&              outerEdges,
                                             std::vector< std::vector<TopoDS_Edge> >& innerEdges) const
{
  if ( seeds.Extent() == 1 )
  {
    const t_topoId     baseFaceId = seeds.GetMinimalMapped();
    const TopoDS_Face& baseFace   = m_aag->GetFace(baseFaceId);

    // Get edges where features grow and the outer edges
    // restricting the finder.
    this->getFaceEdges(baseFace, outerEdges, innerEdges);
  }
  else /* Multibase case */
  {
    TopoDS_Shell shell;
    BRep_Builder bb;
    bb.MakeShell(shell);

    // Compose a shell out of the seed faces.
    for ( asiAlgo_Feature::Iterator fit(seeds); fit.More(); fit.Next() )
    {
      bb.Add( shell, m_aag->GetFace( fit.Key() ) );
    }

    // Build map of edges to extract open ("naked") ones.
    TopTools_IndexedDataMapOfShapeListOfShape edgesFaces;
    TopExp::MapShapesAndAncestors(shell, TopAbs_EDGE, TopAbs_FACE, edgesFaces);

    // Find open edges.
    Handle(TopTools_HSequenceOfShape) openEdgesSeq = new TopTools_HSequenceOfShape;
    //
    for ( int k = 1; k <= edgesFaces.Extent(); ++k )
    {
      const TopTools_ListOfShape& faces = edgesFaces(k);
      //
      if ( faces.Extent() == 1 )
      {
        const TopoDS_Edge& E = TopoDS::Edge( edgesFaces.FindKey(k) );
        //
        if ( BRep_Tool::Degenerated(E) )
          continue;
    
        openEdgesSeq->Append(E);
      }
    }

    // Compose border wires from the naked edges.
    Handle(TopTools_HSequenceOfShape) borderWires;
    ShapeAnalysis_FreeBounds::ConnectEdgesToWires(openEdgesSeq, 1e-3, false, borderWires);
    //
    if ( borderWires.IsNull() || (borderWires->Length() < 2) )
      return false;

    // Repack to a standard vector and compute lengths.
    std::vector<TopoDS_Wire> wires;
    std::vector<double>      lengths;
    std::vector<int>         nums;
    //
    for ( TopTools_HSequenceOfShape::Iterator wit(*borderWires); wit.More(); wit.Next() )
    {
      TopoDS_Wire wire = TopoDS::Wire( wit.Value() );
      wires.push_back(wire);

      nums.push_back( int( nums.size() ) );

      // Compute wire's length.
      GProp_GProps prop;
      BRepGProp::LinearProperties(wire, prop);
      lengths.push_back( prop.Mass() );
    }

    std::sort( nums.begin(), nums.end(),
               [&](const int a, const int b)
               {
                 return lengths[a] > lengths[b];
               } );

    // Collect outer edges.
    for ( TopExp_Explorer wexp(wires[nums[0]], TopAbs_EDGE); wexp.More(); wexp.Next() )
    {
      outerEdges.push_back( TopoDS::Edge( wexp.Current() ) );
    }

    // Collect inner edges.
    for ( auto wit = nums.cbegin() + 1; wit != nums.cend(); ++wit )
    {
      std::vector<TopoDS_Edge> wireEdges;
      for ( TopExp_Explorer wexp(wires[*wit], TopAbs_EDGE); wexp.More(); wexp.Next() )
      {
        wireEdges.push_back( TopoDS::Edge( wexp.Current() ) );
      }
      innerEdges.push_back(wireEdges);
    }

#if defined DRAW_DEBUG
    m_plotter.REDRAW_SHAPE("outerWire", wires[nums[0]], Color_Blue, 1., true);
    m_plotter.REDRAW_SHAPE("innerWire", wires[nums[1]], Color_Red, 1., true);
#endif
  }

  return true;
}
