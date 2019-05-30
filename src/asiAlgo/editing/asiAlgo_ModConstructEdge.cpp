//-----------------------------------------------------------------------------
// Created on: 18 February 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018, Sergey Slyadnev
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
#include <asiAlgo_ModConstructEdge.h>

// asiAlgo includes
#include <asiAlgo_IntersectCS.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomConvert.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeConstruct_ProjectCurveOnSurface.hxx>
#include <ShapeFix_Edge.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #pragma message("===== warning: DRAW_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiAlgo_ModConstructEdge::asiAlgo_ModConstructEdge(const TopoDS_Shape&  shape,
                                                   ActAPI_ProgressEntry notifier,
                                                   ActAPI_PlotterEntry  plotter)
: asiAlgo_ModBase(notifier, plotter), m_shape(shape)
{}

//-----------------------------------------------------------------------------

bool asiAlgo_ModConstructEdge::Init(const TopoDS_Shape& target)
{
  // Downcast for subsequent use.
  const TopoDS_Edge& targetEdge = TopoDS::Edge(target);

  // Initialize info.
  return this->initSituation(targetEdge);
}

//-----------------------------------------------------------------------------

void asiAlgo_ModConstructEdge::SetFrozenVertices(const TopTools_IndexedMapOfShape& vertices)
{
  m_frozenVertices = vertices;
}

//-----------------------------------------------------------------------------

void asiAlgo_ModConstructEdge::Dump(ActAPI_PlotterEntry plotter) const
{
  plotter.REDRAW_SHAPE("e_s1_s2", m_edgeInfo.situation.e_s1_s2, Color_Green, 1., true);
  plotter.REDRAW_SHAPE("e_s1_t1", m_edgeInfo.situation.e_s1_t1, Color_Red,   1., true);
  plotter.REDRAW_SHAPE("e_s1_t2", m_edgeInfo.situation.e_s1_t2, Color_Red,   1., true);
  plotter.REDRAW_SHAPE("e_s2_t1", m_edgeInfo.situation.e_s2_t1, Color_Red,   1., true);
  plotter.REDRAW_SHAPE("e_s2_t2", m_edgeInfo.situation.e_s2_t2, Color_Red,   1., true);
  //
  plotter.REDRAW_SHAPE("f_s1",    m_edgeInfo.situation.f_s1);
  plotter.REDRAW_SHAPE("f_s2",    m_edgeInfo.situation.f_s2);
  plotter.REDRAW_SHAPE("f_t1",    m_edgeInfo.situation.f_t1);
  plotter.REDRAW_SHAPE("f_t2",    m_edgeInfo.situation.f_t2);
  //
  plotter.REDRAW_SHAPE("v_s1_s2_t1", m_edgeInfo.situation.v_s1_s2_t1,
                       m_frozenVertices.Contains(m_edgeInfo.situation.v_s1_s2_t1) ? Color_White : Color_Blue,
                       1., true);
  plotter.REDRAW_SHAPE("v_s1_s2_t2", m_edgeInfo.situation.v_s1_s2_t2,
                       m_frozenVertices.Contains(m_edgeInfo.situation.v_s1_s2_t2) ? Color_White : Color_Blue,
                       1., true);
}

//-----------------------------------------------------------------------------

bool asiAlgo_ModConstructEdge::NewSurface(const TopoDS_Face&    F,
                                          Handle(Geom_Surface)& S,
                                          TopLoc_Location&      L,
                                          double&               tol,
                                          bool&                 revWires,
                                          bool&                 revFace)
{
  asiAlgo_NotUsed(F);
  asiAlgo_NotUsed(S);
  asiAlgo_NotUsed(L);
  asiAlgo_NotUsed(tol);
  asiAlgo_NotUsed(revWires);
  asiAlgo_NotUsed(revFace);

  // Surfaces are not changed.
  return false;
}

//-----------------------------------------------------------------------------

bool asiAlgo_ModConstructEdge::NewCurve(const TopoDS_Edge&  E,
                                        Handle(Geom_Curve)& C,
                                        TopLoc_Location&    L,
                                        double&             tol)
{
  // Now 3D curve is constructed for the target edge only.
  if ( !E.IsPartner(m_edgeInfo.situation.e_s1_s2) )
    return false;

  // Check and set the result.
  if ( !m_edgeInfo.resolution.icurve.IsNull() )
  {
    // Set updated data for the caller
    C   = m_edgeInfo.resolution.icurve->C;
    L   = TopLoc_Location();
    tol = m_edgeInfo.resolution.icurve->Uncertainty;
    //
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------

bool asiAlgo_ModConstructEdge::NewPoint(const TopoDS_Vertex& V,
                                        gp_Pnt&              P,
                                        double&              tol)
{
  if ( !V.IsPartner(m_edgeInfo.situation.v_s1_s2_t1) &&
       !V.IsPartner(m_edgeInfo.situation.v_s1_s2_t2) )
    return false;

  // Get old point of the vertex.
  gp_Pnt oldPt = BRep_Tool::Pnt(V);

  // Do not evaluate "frozen" vertices. This small `if` is a part of
  // our knowledge-driven computation paradigm. According to the latter,
  // if you know that a vertex is already in its "good" position, then
  // do not try to reevaluate it. Such a paradigm is especially fruitful
  // in cases where intersection is not well-defined, e.g., when tangential
  // situation is realized.
  if ( m_frozenVertices.Contains(V) )
  {
    m_progress.SendLogMessage(LogNotice(Normal) << "Skipping 'frozen' vertex.");
    m_plotter.DRAW_SHAPE(V, Color_Blue, 1.0, true, "frozen_vertex");

    // Use existing geometry.
    P   = oldPt;
    tol = BRep_Tool::Tolerance(V);

    // Set resolution.
    if ( V.IsPartner(m_edgeInfo.situation.v_s1_s2_t1) )
      m_edgeInfo.resolution.ivf.Init(P, true);
    else
      m_edgeInfo.resolution.ivl.Init(P, true);

    return this->checkResolvedVertices(); // Check if the vertices are Ok.
  }

  // Intersection point.
  asiAlgo_ModEdgeInfo::Resolution::t_vertexStatus* ipoint = NULL;

  // Curve and surface to intersect.
  Handle(Geom_Curve)   icurve = m_edgeInfo.resolution.icurve->C;
  Handle(Geom_Surface) isurf;

  TCollection_AsciiString opFaceName;

  // First vertex.
  if ( V.IsPartner(m_edgeInfo.situation.v_s1_s2_t1) )
  {
    isurf  = BRep_Tool::Surface(m_edgeInfo.situation.f_t1);
    ipoint = &m_edgeInfo.resolution.ivf;

    // For diagnostics.
    opFaceName = "f_t1";
  }
  else if ( V.IsPartner(m_edgeInfo.situation.v_s1_s2_t2) ) // Last vertex.
  {
    isurf  = BRep_Tool::Surface(m_edgeInfo.situation.f_t2);
    ipoint = &m_edgeInfo.resolution.ivl;

    // For diagnostics.
    opFaceName = "f_t2";
  }

  // Prepare intersection tool.
  asiAlgo_IntersectCS intCS(m_progress, m_plotter);
  //
  intCS.SetTangencyAngularTol(0.05); // Set tolerance (in degrees) for
                                     // detecting tangential intersections.

  // Intersect.
  bool hasTangencyPoints = false;
  asiAlgo_IntersectionPointsCS ipoints;
  //
  if ( !intCS(isurf, icurve, ipoints, hasTangencyPoints) )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot intersect curve and surface.");

    this->SetErrorStateOn();
    return false;
  }

  // Check the number of computed intersection points.
  double    uncertainty = 0.0;
  const int numInterPts = ipoints.Length();
  //
  if ( !numInterPts )
  {
    m_progress.SendLogMessage(LogWarn(Normal) << "No intersection points found between "
                                                 "intersection curve 'icurve' and host surface of '%1'."
                                              << opFaceName);

    // Reuse the old point.
    ipoint->Init(oldPt, true);
  }
  else if ( numInterPts > 1 )
  {
    m_progress.SendLogMessage(LogWarn(Normal) << "Ambiguous intersection point (%1 points computed) between "
                                                 "intersection curve 'icurve' and host surface of '%2'."
                                              << numInterPts
                                              << opFaceName);

    // In case of ambiguous intersection, we take the closest point. Ambiguous
    // intersection may occur, for example, if the surface and the line to
    // intersect are touching each other, or if a line intersects a cylinder.
    gp_XYZ chosenPt, avrPoint;
    double minDist = DBL_MAX;
    //
    for ( int k = 1; k <= numInterPts; ++k )
    {
#if defined DRAW_DEBUG
      TCollection_AsciiString ipointname;
      ipointname += "[";
      ipointname += k;
      ipointname += "]";
      ipointname += " Ambiguous intersection point";
      //
      m_plotter.DRAW_POINT(ipoints(k)->P, Color_Red, ipointname);
#endif

      // Select min-distance point.
      const double dist = ( ipoints(k)->P.XYZ() - oldPt.XYZ() ).Modulus();
      if ( dist < minDist )
      {
        minDist     = dist;
        chosenPt    = ipoints(k)->P.XYZ();
        uncertainty = ipoints(k)->Uncertainty;
      }

      // Compute average point.
      avrPoint += ipoints(k)->P.XYZ();
    }
    //
    avrPoint /= numInterPts;

    if ( !hasTangencyPoints )
    {
      m_progress.SendLogMessage(LogNotice(Normal) << "Min distance point is chosen.");

#if defined DRAW_DEBUG
      m_plotter.DRAW_POINT(chosenPt, Color_Red, "chosenPt");
#endif
    }

    // Initialize intersection point. In case if we have the situation of
    // tangency, we take average point for better precision as multiple points
    // represent just one (though ill-defined) intersection.
    ipoint->Init(hasTangencyPoints ? avrPoint : chosenPt, true);
  }
  else // One intersection point.
  {
    uncertainty = ipoints(1)->Uncertainty;

    // Initialize intersection point.
    ipoint->Init(ipoints(1)->P, true);
  }

#if defined DRAW_DEBUG
  if ( V.IsPartner(m_edgeInfo.situation.v_s1_s2_t1) )
    m_plotter.DRAW_POINT((*ipoint).point, Color_Red, "NewPoint:P (v first)");
  else
    m_plotter.DRAW_POINT((*ipoint).point, Color_Blue, "NewPoint:P (v last)");
#endif

  // Set updated data for the caller.
  P   = ipoint->point;
  tol = uncertainty;
  //
  return this->checkResolvedVertices(); // Check if the vertices are Ok.
}

//-----------------------------------------------------------------------------

bool asiAlgo_ModConstructEdge::NewCurve2d(const TopoDS_Edge&    E,
                                          const TopoDS_Face&    F,
                                          const TopoDS_Edge&    NewE,
                                          const TopoDS_Face&    NewF,
                                          Handle(Geom2d_Curve)& C,
                                          double&               tol)
{
  asiAlgo_NotUsed(F);

  if ( !E.IsPartner(m_edgeInfo.situation.e_s1_s2) )
    return false;

  Handle(Geom2d_Curve) c2d;

  try
  {
    c2d = this->buildPCurve(NewE, NewF);
  }
  catch ( ... )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Crash on constructing p-curve.");
  }

  if ( c2d.IsNull() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot fix p-curve.");

    this->SetErrorStateOn();
    return false;
  }

  C   = c2d;
  tol = BRep_Tool::Tolerance(E);

#if defined DRAW_DEBUG
  m_plotter.DRAW_CURVE2D(C, Color_Blue, "NewCurve2d:c2d");
#endif

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_ModConstructEdge::NewParameter(const TopoDS_Vertex& V,
                                            const TopoDS_Edge&   E,
                                            double&              p,
                                            double&              tol)
{
  if ( !V.IsPartner(m_edgeInfo.situation.v_s1_s2_t1) &&
       !V.IsPartner(m_edgeInfo.situation.v_s1_s2_t2) )
    return false;

  if ( E.IsPartner(m_edgeInfo.situation.e_s1_s2) )
  {
    // V_first
    if ( V.IsPartner(m_edgeInfo.situation.v_s1_s2_t1) )
    {
      gp_Pnt proj;
      ShapeAnalysis_Curve sac;
      sac.Project(m_edgeInfo.resolution.icurve->C,
                  m_edgeInfo.resolution.ivf.point,
                  Precision::Confusion(),
                  proj,
                  p);

      // Adjust period.
      if ( asiAlgo_Utils::IsBasisCircular(m_edgeInfo.resolution.icurve->C) )
        if ( p - M_PI > 0 )
          p = p - 2*M_PI;

      m_progress.SendLogMessage(LogInfo(Normal) << "P in NewParameter for v_first on target edge: %1." << p);
      m_plotter.DRAW_POINT(proj, Color_Yellow, "P in NewParameter for v_first on target edge");
    }

    // V_last
    else if ( V.IsPartner(m_edgeInfo.situation.v_s1_s2_t2) )
    {
      gp_Pnt proj;
      ShapeAnalysis_Curve sac;
      sac.Project(m_edgeInfo.resolution.icurve->C,
                  m_edgeInfo.resolution.ivl.point,
                  Precision::Confusion(),
                  proj,
                  p);

      // Adjust period.
      if ( asiAlgo_Utils::IsBasisCircular(m_edgeInfo.resolution.icurve->C) )
        if ( p - M_PI > 0 )
          p = p - 2*M_PI;

      m_progress.SendLogMessage(LogInfo(Normal) << "P in NewParameter for v_last on target edge: %1." << p);
      m_plotter.DRAW_POINT(proj, Color_Yellow, "P in NewParameter for v_last on target edge");
    }

    tol = BRep_Tool::Tolerance(V);
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------

GeomAbs_Shape
  asiAlgo_ModConstructEdge::Continuity(const TopoDS_Edge& E,
                                       const TopoDS_Face& F1,
                                       const TopoDS_Face& F2,
                                       const TopoDS_Edge& NewE,
                                       const TopoDS_Face& NewF1,
                                       const TopoDS_Face& NewF2)
{
  asiAlgo_NotUsed(NewE);
  asiAlgo_NotUsed(NewF1);
  asiAlgo_NotUsed(NewF2);

  return BRep_Tool::Continuity(E, F1, F2);
}

//-----------------------------------------------------------------------------

bool asiAlgo_ModConstructEdge::initSituation(const TopoDS_Edge& targetEdge)
{
  // Build child-parent map.
  asiAlgo_IndexedDataMapOfTShapeListOfShape edgeFaceMap;
  asiAlgo_Utils::MapTShapesAndAncestors(m_shape, TopAbs_EDGE, TopAbs_FACE, edgeFaceMap);

  // Check if the edge in question is a part of the model.
  if ( !edgeFaceMap.Contains(targetEdge) )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Edge is not a part of the model.");

    this->SetErrorStateOn();
    return false;
  }

  // Get faces containing the edge in question.
  const TopTools_ListOfShape& edgeFaces = edgeFaceMap.FindFromKey(targetEdge);

  // Check if there are exactly two faces.
  if ( edgeFaces.Extent() != 2 )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Edge is either a border or a non-manifold one.");

    this->SetErrorStateOn();
    return false;
  }

  // Initialize edge in question.
  m_edgeInfo.situation.e_s1_s2 = targetEdge;

  // Initialize vertices.
  ShapeAnalysis_Edge sae;
  m_edgeInfo.situation.v_s1_s2_t1 = sae.FirstVertex(m_edgeInfo.situation.e_s1_s2);
  m_edgeInfo.situation.v_s1_s2_t2 = sae.LastVertex(m_edgeInfo.situation.e_s1_s2);

  // Initialize owner faces.
  const TopoDS_Face& F1 = TopoDS::Face( edgeFaces.First() );
  const TopoDS_Face& F2 = TopoDS::Face( edgeFaces.Last() );
  //
  for ( TopTools_ListIteratorOfListOfShape fit(edgeFaces); fit.More(); fit.Next() )
  {
    const TopoDS_Shape& edgeFace = fit.Value();

    // Find target edge in the face.
    for ( TopExp_Explorer eexp(edgeFace, TopAbs_EDGE); eexp.More(); eexp.Next() )
    {
      const TopoDS_Shape& currentEdge = eexp.Current();
      //
      if ( !currentEdge.IsPartner(m_edgeInfo.situation.e_s1_s2) )
        continue;

      // Face which employs the target edge with its inherent orientation
      // is declared as F1.
      if ( currentEdge.Orientation() == m_edgeInfo.situation.e_s1_s2.Orientation() )
      {
        if ( edgeFace.IsPartner(F1) )
        {
          m_edgeInfo.situation.f_s1 = F1;
          m_edgeInfo.situation.f_s2 = F2;
        }
        else
        {
          m_edgeInfo.situation.f_s1 = F2;
          m_edgeInfo.situation.f_s2 = F1;
        }
      }
      else
      {
        if ( edgeFace.IsPartner(F1) )
        {
          m_edgeInfo.situation.f_s1 = F2;
          m_edgeInfo.situation.f_s2 = F1;
        }
        else
        {
          m_edgeInfo.situation.f_s1 = F1;
          m_edgeInfo.situation.f_s2 = F2;
        }
      }
    }

    // Check if all required information has been collected. If so, break.
    if ( !m_edgeInfo.situation.f_s1.IsNull() &&
         !m_edgeInfo.situation.f_s2.IsNull() )
      break;
  }

  // Build maps for child-parent relations between vertices and edges in F1
  // and F2. These maps are used to resolve previous and next edges.
  TopTools_IndexedDataMapOfShapeListOfShape vertexEdgesF1, vertexEdgesF2;
  TopExp::MapShapesAndAncestors(m_edgeInfo.situation.f_s1, TopAbs_VERTEX, TopAbs_EDGE, vertexEdgesF1);
  TopExp::MapShapesAndAncestors(m_edgeInfo.situation.f_s2, TopAbs_VERTEX, TopAbs_EDGE, vertexEdgesF2);

  // Initialize the previous and the next edges for the target edge on F1.
  for ( int i = 1; i <= vertexEdgesF1.Extent(); ++i )
  {
    const TopoDS_Shape&         vertex = vertexEdgesF1.FindKey(i);
    const TopTools_ListOfShape& edges  = vertexEdgesF1.FindFromIndex(i);

    // Check if the current vertex is the extremity of the target edge. If not,
    // we skip such vertex.
    bool isFirstOnE = false, isLastOnE = false;
    //
    if ( vertex.IsPartner(m_edgeInfo.situation.v_s1_s2_t1) )
      isFirstOnE = true;
    else if ( vertex.IsPartner(m_edgeInfo.situation.v_s1_s2_t2) )
      isLastOnE = true;
    //
    if ( !isFirstOnE && !isLastOnE )
      continue;

    // Now the vertex is known to be the extremity of the target edge. Therefore,
    // we can decide on which edge is the previous/next one.
    for ( TopTools_ListIteratorOfListOfShape eit(edges); eit.More(); eit.Next() )
    {
      const TopoDS_Shape& currentEdge = eit.Value();
      //
      if ( currentEdge.IsPartner(m_edgeInfo.situation.e_s1_s2) )
        continue; // Skip target edge itself as it is always in the list of parents.

      if ( isFirstOnE )
        m_edgeInfo.situation.e_s1_t1 = TopoDS::Edge(currentEdge);
      else if ( isLastOnE )
        m_edgeInfo.situation.e_s1_t2 = TopoDS::Edge(currentEdge);
    }

    // Stop iterating when both edges are done.
    if ( !m_edgeInfo.situation.e_s1_t1.IsNull() &&
         !m_edgeInfo.situation.e_s1_t2.IsNull() )
      break;
  }

  // Initialize the previous and the next edges for the target edge on F2.
  for ( int i = 1; i <= vertexEdgesF2.Extent(); ++i )
  {
    const TopoDS_Shape&         vertex = vertexEdgesF2.FindKey(i);
    const TopTools_ListOfShape& edges  = vertexEdgesF2.FindFromIndex(i);

    // Check if the current vertex is the extremity of the target edge. If not,
    // we skip such vertex.
    bool isFirstOnE = false, isLastOnE = false;
    //
    if ( vertex.IsPartner(m_edgeInfo.situation.v_s1_s2_t1) )
      isFirstOnE = true;
    else if ( vertex.IsPartner(m_edgeInfo.situation.v_s1_s2_t2) )
      isLastOnE = true;
    //
    if ( !isFirstOnE && !isLastOnE )
      continue;

    // Now the vertex is known to be the extremity of the target edge. Therefore,
    // we can decide on which edge is the previous/next one.
    for ( TopTools_ListIteratorOfListOfShape eit(edges); eit.More(); eit.Next() )
    {
      const TopoDS_Shape& currentEdge = eit.Value();
      //
      if ( currentEdge.IsPartner(m_edgeInfo.situation.e_s1_s2) )
        continue; // Skip target edge itself as it is always in the list of parents.

      if ( isFirstOnE )
        m_edgeInfo.situation.e_s2_t1 = TopoDS::Edge(currentEdge);
      else if ( isLastOnE )
        m_edgeInfo.situation.e_s2_t2 = TopoDS::Edge(currentEdge);
    }

    // Stop iterating when both edges are done.
    if ( !m_edgeInfo.situation.e_s2_t1.IsNull() &&
         !m_edgeInfo.situation.e_s2_t2.IsNull() )
      break;
  }
  
  // Initialize t1.
  TopTools_IndexedMapOfShape f1_prev_neighbors;
  asiAlgo_Utils::GetNeighborsThru(m_shape,
                                  m_edgeInfo.situation.f_s1,
                                  m_edgeInfo.situation.e_s1_t1,
                                  f1_prev_neighbors);
  //
  if ( f1_prev_neighbors.Extent() != 1 )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Unexpected face neighborhood for face f1."
                                                "Cannot find t1.");

    this->SetErrorStateOn();
    return false;
  }
  //
  m_edgeInfo.situation.f_t1 = TopoDS::Face( f1_prev_neighbors(1) );

  // Initialize t2.
  TopTools_IndexedMapOfShape f1_next_neighbors;
  asiAlgo_Utils::GetNeighborsThru(m_shape,
                                  m_edgeInfo.situation.f_s1,
                                  m_edgeInfo.situation.e_s1_t2,
                                  f1_next_neighbors);
  //
  if ( f1_next_neighbors.Extent() != 1 )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Unexpected face neighborhood for face f1."
                                                "Cannot find t2.");

    this->SetErrorStateOn();
    return false;
  }
  //
  m_edgeInfo.situation.f_t2 = TopoDS::Face( f1_next_neighbors(1) );

#if defined DRAW_DEBUG
  m_edgeInfo.DumpSituation(m_plotter);
#endif

  /* ============================================
   *  Re-intersect faces (just once for an edge)
   * ============================================ */

  // Re-intersect base faces.
  if ( !this->intersectSurfaces(m_edgeInfo.situation.f_s1,
                                m_edgeInfo.situation.f_s2,
                                m_edgeInfo.resolution.icurve) )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot intersect faces.");

    this->SetErrorStateOn();
    return false;
  }

  // Correct orientation. BRepTools_Modifier framework does not handle
  // changing orientation of curves, so if after re-intersection the
  // geometric orientation is affected, we have to fix it here.
  if ( !this->correctOriC(m_edgeInfo.resolution.icurve->C,
                          m_edgeInfo.situation.e_s1_s2,
                          m_edgeInfo.resolution.icurve->C) )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot correct orientation of the intersection curve.");

    this->SetErrorStateOn();
    return false;
  }

#if defined DRAW_DEBUG
  m_plotter.REDRAW_CURVE("Resolution.icurve", m_edgeInfo.resolution.icurve->C, Color_Green);
#endif

  return true;
}

//-----------------------------------------------------------------------------

Handle(Geom2d_Curve) asiAlgo_ModConstructEdge::buildPCurve(const TopoDS_Edge& E,
                                                           const TopoDS_Face& F)
{
  Handle(ShapeConstruct_ProjectCurveOnSurface)
    myProjector = new ShapeConstruct_ProjectCurveOnSurface;

  double f, l;
  Handle(Geom_Curve) c3d = BRep_Tool::Curve(E, f, l);

#if defined DRAW_DEBUG
  m_plotter.REDRAW_CURVE("buildPCurve:E(curve)", new Geom_TrimmedCurve(c3d, f, l), Color_Red);
  m_plotter.REDRAW_SHAPE("buildPCurve:F", F);
#endif

  Handle(Geom2d_Curve) c2d;
  double TolFirst = -1, TolLast = -1;
  TopoDS_Vertex V1, V2;
  TopExp::Vertices(E, V1, V2);
  if (!V1.IsNull())
    TolFirst = BRep_Tool::Tolerance(V1);
  if (!V2.IsNull())
    TolLast = BRep_Tool::Tolerance(V2);
      
  Handle(ShapeAnalysis_Surface)
    sas = new ShapeAnalysis_Surface( BRep_Tool::Surface(F) );

  try
  {
    myProjector->Init ( sas, Precision::Confusion() );
    myProjector->Perform (c3d,f,l,c2d,TolFirst,TolLast);
  }
  catch ( ... )
  {
    std::cout << "Exception on projecting 3D curve" << std::endl;
    this->SetErrorStateOn();
    //// Let's convert our curve to b-curve.
    //c3d = GeomConvert::CurveToBSplineCurve(c3d, Convert_QuasiAngular);

    //myProjector->Perform (c3d,f,l,c2d,TolFirst,TolLast);
  }

  /*ShapeFix_Edge sfe;
  sfe.FixAddPCurve(E, F, false);
  c2d = BRep_Tool::CurveOnSurface(E, F, f, l);*/

  return c2d;
}

//-----------------------------------------------------------------------------

bool asiAlgo_ModConstructEdge::checkResolvedVertices()
{
  if ( !m_edgeInfo.resolution.ivf.done || !m_edgeInfo.resolution.ivl.done )
    return true; // Check is done only if both vertices are resolved.

  // Check if the vertices are not coincident.
  if ( m_edgeInfo.resolution.ivf.point.Distance(m_edgeInfo.resolution.ivl.point) < Precision::Confusion() )
  {
    this->SetErrorStateOn();
    return false;
  }

  return true;
}
