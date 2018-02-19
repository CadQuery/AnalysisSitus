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
#include <asiAlgo_ModConstructEdge.h>

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

Handle(Geom2d_Curve) pcu(const TopoDS_Edge& E, const TopoDS_Face& F)
{
  Handle(ShapeConstruct_ProjectCurveOnSurface) myProjector = new ShapeConstruct_ProjectCurveOnSurface;

  double f, l;
  Handle(Geom_Curve) c3d = BRep_Tool::Curve(E, f, l);

  Handle(Geom2d_Curve) c2d;
  Standard_Real a1, b1;

  Standard_Real TolFirst = -1, TolLast = -1;
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
    // Let's convert our curve to b-curve.
    c3d = GeomConvert::CurveToBSplineCurve(c3d, Convert_QuasiAngular);

    myProjector->Perform (c3d,f,l,c2d,TolFirst,TolLast);
  }

  //ShapeFix_Edge sfe;
  //sfe.FixAddPCurve(E, F, false);
  //double f,l;
  //c2d = BRep_Tool::CurveOnSurface(E, F, f, l);

  return c2d;
}

//-----------------------------------------------------------------------------

asiAlgo_ModConstructEdge::asiAlgo_ModConstructEdge(const Handle(asiAlgo_AAG)& aag,
                                                   ActAPI_ProgressEntry       notifier,
                                                   ActAPI_PlotterEntry        plotter)
: asiAlgo_ModBase(notifier, plotter), m_aag(aag)
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

bool asiAlgo_ModConstructEdge::NewSurface(const TopoDS_Face&    F,
                                          Handle(Geom_Surface)& S,
                                          TopLoc_Location&      L,
                                          double&               tol,
                                          bool&                 revWires,
                                          bool&                 revFace)
{
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
  if ( !E.IsPartner(m_edgeInfo.situation.e) )
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
  if ( !V.IsPartner(m_edgeInfo.situation.v_first) &&
       !V.IsPartner(m_edgeInfo.situation.v_last) )
    return false;

  // First curve is taken from the neighbor edge.
  Handle(Geom_Curve) C1, C1_alt;
  double f1 = 0.0, l1 = 0.0, f1_alt = 0.0, l1_alt = 0.0;

  // Second curve is the intersection curve.
  Handle(Geom_Curve) C2 = m_edgeInfo.resolution.icurve->C;
  double f2 = -100; // TODO: rework
  double l2 = +100; // TODO: rework

  // Result.
  Handle(asiAlgo_IntersectionPointCC)* ipoint = NULL;

  // First vertex.
  if ( V.IsPartner(m_edgeInfo.situation.v_first) )
  {
    // Get curve from the neighbor edge.
    C1     = BRep_Tool::Curve(m_edgeInfo.situation.e_1_prev, f1, l1);
    C1_alt = BRep_Tool::Curve(m_edgeInfo.situation.e_2_next, f1_alt, l1_alt);

    // Choose where to store the intersection point.
    ipoint = &m_edgeInfo.resolution.ivf;
  }
  // Last vertex.
  else if ( V.IsPartner(m_edgeInfo.situation.v_last) )
  {
    // Get curve from the neighbor edge.
    C1     = BRep_Tool::Curve(m_edgeInfo.situation.e_1_next, f1, l1);
    C1_alt = BRep_Tool::Curve(m_edgeInfo.situation.e_2_prev, f1_alt, l1_alt);

    // Choose where to store the intersection point.
    ipoint = &m_edgeInfo.resolution.ivl;
  }

  // Choose parametric ranges for intersection.
  f1     -= 100;
  l1     += 100;
  f1_alt -= 100;
  l1_alt += 100;
  f2     -= 100;
  l2     += 100;

  // Intersect curves.
  if ( !this->intersectCurves(C1, f1, l1,
                              C2, f2, l2,
                              *ipoint) &&
                              //
       !this->intersectCurves(C1_alt, f1_alt, l1_alt,
                              C2, f2, l2,
                              *ipoint) )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot intersect curves.");
    return false;
  }

#if defined DRAW_DEBUG
  if ( V.IsPartner(m_edgeInfo.situation.v_first) )
    m_plotter.DRAW_POINT((*ipoint)->P, Color_Red, "NewPoint:P (v first)");
  else
    m_plotter.DRAW_POINT((*ipoint)->P, Color_Blue, "NewPoint:P (v last)");
#endif

  // Set updated data for the caller
  P   = (*ipoint)->P;
  tol = (*ipoint)->Uncertainty;
  //
  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_ModConstructEdge::NewCurve2d(const TopoDS_Edge&    E,
                                          const TopoDS_Face&    F,
                                          const TopoDS_Edge&    NewE,
                                          const TopoDS_Face&    NewF,
                                          Handle(Geom2d_Curve)& C,
                                          double&               tol)
{
  if ( !E.IsPartner(m_edgeInfo.situation.e) )
    return false;

  Handle(Geom2d_Curve) c2d = pcu(NewE, NewF);
  if ( c2d.IsNull() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot fix p-curve");
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
  if ( !V.IsPartner(m_edgeInfo.situation.v_first) &&
       !V.IsPartner(m_edgeInfo.situation.v_last) )
    return false;

  if ( E.IsPartner(m_edgeInfo.situation.e) )
  {
    // V_first
    if ( V.IsPartner(m_edgeInfo.situation.v_first) )
    {
      gp_Pnt proj;
      ShapeAnalysis_Curve sac;
      sac.Project(m_edgeInfo.resolution.icurve->C,
                  m_edgeInfo.resolution.ivf->P,
                  Precision::Confusion(),
                  proj,
                  p);

      m_plotter.DRAW_POINT(proj, Color_Yellow, "P in NewParameter for v_first on target edge");
      m_plotter.DRAW_POINT(proj, Color_Yellow, "P in NewParameter for v_first on target edge");
    }

    // V_last
    else if ( V.IsPartner(m_edgeInfo.situation.v_last) )
    {
      gp_Pnt proj;
      ShapeAnalysis_Curve sac;
      sac.Project(m_edgeInfo.resolution.icurve->C,
                  m_edgeInfo.resolution.ivl->P,
                  Precision::Confusion(),
                  proj,
                  p);

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
  return BRep_Tool::Continuity(E, F1, F2);
}

//-----------------------------------------------------------------------------

bool asiAlgo_ModConstructEdge::initSituation(const TopoDS_Edge& targetEdge)
{
  // Build child-parent map.
  TopTools_IndexedDataMapOfShapeListOfShape edgeFaceMap;
  TopExp::MapShapesAndAncestors(m_aag->GetMasterCAD(), TopAbs_EDGE, TopAbs_FACE, edgeFaceMap);

  // Check if the edge in question is a part of the model.
  if ( !edgeFaceMap.Contains(targetEdge) )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Edge is not a part of the model.");
    return false;
  }

  // Get faces containing the edge in question.
  const TopTools_ListOfShape& edgeFaces = edgeFaceMap.FindFromKey(targetEdge);

  // Check if there are exactly two faces.
  if ( edgeFaces.Extent() != 2 )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Edge is either a border or a non-manifold one.");
    return false;
  }

  // Initialize edge in question.
  m_edgeInfo.situation.e = targetEdge;

  // Initialize vertices.
  ShapeAnalysis_Edge sae;
  m_edgeInfo.situation.v_first = sae.FirstVertex(m_edgeInfo.situation.e);
  m_edgeInfo.situation.v_last  = sae.LastVertex(m_edgeInfo.situation.e);

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
      if ( !currentEdge.IsPartner(m_edgeInfo.situation.e) )
        continue;

      // Face which employs the target edge with its inherent orientation
      // is declared as F1.
      if ( currentEdge.Orientation() == m_edgeInfo.situation.e.Orientation() )
      {
        if ( edgeFace.IsPartner(F1) )
        {
          m_edgeInfo.situation.f_1 = F1;
          m_edgeInfo.situation.f_2 = F2;
        }
        else
        {
          m_edgeInfo.situation.f_1 = F2;
          m_edgeInfo.situation.f_2 = F1;
        }
      }
      else
      {
        if ( edgeFace.IsPartner(F1) )
        {
          m_edgeInfo.situation.f_1 = F2;
          m_edgeInfo.situation.f_2 = F1;
        }
        else
        {
          m_edgeInfo.situation.f_1 = F1;
          m_edgeInfo.situation.f_2 = F2;
        }
      }
    }

    // Check if all required information has been collected. If so, break.
    if ( !m_edgeInfo.situation.f_1.IsNull() &&
         !m_edgeInfo.situation.f_2.IsNull() )
      break;
  }

  // Build maps for child-parent relations between vertices and edges in F1
  // and F2. These maps are used to resolve previous and next edges.
  TopTools_IndexedDataMapOfShapeListOfShape vertexEdgesF1, vertexEdgesF2;
  TopExp::MapShapesAndAncestors(m_edgeInfo.situation.f_1, TopAbs_VERTEX, TopAbs_EDGE, vertexEdgesF1);
  TopExp::MapShapesAndAncestors(m_edgeInfo.situation.f_2, TopAbs_VERTEX, TopAbs_EDGE, vertexEdgesF2);

  // Initialize the previous and the next edges for the target edge on F1.
  for ( int i = 1; i <= vertexEdgesF1.Extent(); ++i )
  {
    const TopoDS_Shape&         vertex = vertexEdgesF1.FindKey(i);
    const TopTools_ListOfShape& edges  = vertexEdgesF1.FindFromIndex(i);

    // Check if the current vertex is the extremity of the target edge. If not,
    // we skip such vertex.
    bool isFirstOnE = false, isLastOnE = false;
    //
    if ( vertex.IsPartner(m_edgeInfo.situation.v_first) )
      isFirstOnE = true;
    else if ( vertex.IsPartner(m_edgeInfo.situation.v_last) )
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
      if ( currentEdge.IsPartner(m_edgeInfo.situation.e) )
        continue; // Skip target edge itself as it is always in the list of parents.

      if ( isFirstOnE )
        m_edgeInfo.situation.e_1_prev = TopoDS::Edge(currentEdge);
      else if ( isLastOnE )
        m_edgeInfo.situation.e_1_next = TopoDS::Edge(currentEdge);
    }

    // Stop iterating when both edges are done.
    if ( !m_edgeInfo.situation.e_1_prev.IsNull() &&
         !m_edgeInfo.situation.e_1_next.IsNull() )
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
    if ( vertex.IsPartner(m_edgeInfo.situation.v_first) )
      isFirstOnE = true;
    else if ( vertex.IsPartner(m_edgeInfo.situation.v_last) )
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
      if ( currentEdge.IsPartner(m_edgeInfo.situation.e) )
        continue; // Skip target edge itself as it is always in the list of parents.

      if ( isFirstOnE )
        m_edgeInfo.situation.e_2_next = TopoDS::Edge(currentEdge);
      else if ( isLastOnE )
        m_edgeInfo.situation.e_2_prev = TopoDS::Edge(currentEdge);
    }

    // Stop iterating when both edges are done.
    if ( !m_edgeInfo.situation.e_2_prev.IsNull() &&
         !m_edgeInfo.situation.e_2_next.IsNull() )
      break;
  }

#if defined DRAW_DEBUG
  m_edgeInfo.DumpSituation(m_plotter);
#endif

  /* ============================================
   *  Re-intersect faces (just once for an edge)
   * ============================================ */

  // Re-intersect faces.
  if ( !this->intersectFaces(m_edgeInfo.situation.f_1,
                             m_edgeInfo.situation.f_2,
                             m_edgeInfo.resolution.icurve) )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot intersect faces.");
    return false;
  }

  // Correct orientation. BRepTools_Modifier framework does not handle
  // changing orientation of curves, so if after re-intersection the
  // geometric orientation is affect, we have to fix it here.
  if ( !this->correctOriC(m_edgeInfo.resolution.icurve->C,
                          m_edgeInfo.situation.e,
                          m_edgeInfo.resolution.icurve->C) )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot correct orientation of the intersection curve.");
    return false;
  }

  return true;
}
