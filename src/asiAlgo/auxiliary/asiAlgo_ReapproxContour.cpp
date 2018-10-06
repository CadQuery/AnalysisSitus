//-----------------------------------------------------------------------------
// Created on: 06 October 2018
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
#include <asiAlgo_ReapproxContour.h>

// CC includes
#include <asiAlgo_CheckContour.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <Geom_Line.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HSequenceOfReal.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopTools_HSequenceOfShape.hxx>

#undef DRAW_DEBUG
#undef DRAW_DEBUG_SEGMENTATION
#undef DRAW_DEBUG_APPROX
#undef DRAW_DEBUG_REFINE

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

#define ToDeg(Rad) Rad*180.0/M_PI
#define ToRad(Rad) Rad*M_PI/180.0

//-----------------------------------------------------------------------------

int ranged(const int I, const int NUM)
{
  return (I - 1) % NUM + 1;
}

//-----------------------------------------------------------------------------

asiAlgo_ReapproxContour::asiAlgo_ReapproxContour(const TopoDS_Shape&  contour,
                                                 const double         precision,
                                                 const double         barrierAngleDeg,
                                                 ActAPI_ProgressEntry progress,
                                                 ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm  (progress, plotter),
  m_contour          (contour),
  m_fPrec            (precision),
  m_fBarrierAngleDeg (barrierAngleDeg)
{
#if defined COUT_DEBUG
  std::cout << "asiAlgo_ReapproxContour precision: " << precision << std::endl;
#endif
  m_bClosed = asiAlgo_CheckContour(progress, plotter).Check_connectedWire(contour, false, m_fPrec);
}

//-----------------------------------------------------------------------------

bool asiAlgo_ReapproxContour::operator()(const bool   useSegments,
                                         const bool   useAccumulatedAngle,
                                         TopoDS_Wire& wire)
{
  // Perform segmentation.
  std::vector<t_segment> segments;
  if ( !this->doSegmentation(useSegments, useAccumulatedAngle, segments) )
    return false;

  // Perform approximation.
  std::vector<Handle(Geom_Curve)> curves;
  if ( !this->doApprox(segments, curves) )
    return false;

  // Make wire.
  TopoDS_Wire W;
  if ( !this->doWire(curves, W) )
    return false;

  // Set result.
  wire = W;
  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_ReapproxContour::doSegmentation(const bool              useSegments,
                                             const bool              useAccumulatedAngle,
                                             std::vector<t_segment>& segments)
{
  /* ========================
   *  Prepare discretization
   * ======================== */

  gp_XYZ Pm;
  int nContourVerts = 0;
  TColgp_SequenceOfPnt pts;

  TopAbs_Orientation aContourOri = m_contour.Orientation();

  for ( TopExp_Explorer exp(m_contour, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    const TopoDS_Edge& E = TopoDS::Edge( exp.Current() );

    double f, l;
    BRep_Tool::Range(E, f, l);
    BRepAdaptor_Curve BAC(E);
    GCPnts_TangentialDeflection Defl(BAC, f, l, M_PI/18.0, m_fPrec);

    TopAbs_Orientation anOri = E.Orientation();
    if (aContourOri == TopAbs_REVERSED)
    {
      if (anOri == TopAbs_REVERSED)
        anOri = TopAbs_FORWARD;
      else if (anOri == TopAbs_FORWARD)
        anOri = TopAbs_REVERSED;
    }

    if ( anOri == TopAbs_REVERSED )
    {
      gp_Pnt P_prev;
      for ( int pt_idx = Defl.NbPoints(); pt_idx >= 1; --pt_idx )
      {
        gp_Pnt P = Defl.Value(pt_idx);

        if ( pt_idx < Defl.NbPoints() )
        {
          const gp_Dir vecTo     = P.XYZ() - P_prev.XYZ();
          const double sparsity  = P.Distance(P_prev);
          const double step      = 50; // [mm]
          const int    numNewPts = (int) (sparsity / step) - 1;

          for ( int kk = 1; kk <= numNewPts; ++kk )
          {
            gp_Pnt P_enriching = P_prev.XYZ() + vecTo.XYZ()*step*kk;
            pts.Append(P_enriching);
          }
        }

        pts.Append(P);
        ++nContourVerts;
        Pm += P.XYZ();

#if defined DRAW_DEBUG && defined DRAW_DEBUG_SEGMENTATION
        DRAW_INITGROUP(CC_ReapproxContour_Raw)
        DRAW_POINT(P, CC_ReapproxContour_Raw, Draw_bleu, Draw_Plus)
#endif

        P_prev = P;
      }
    }
    else
    {
      gp_Pnt P_prev;
      for ( int pt_idx = 1; pt_idx <= Defl.NbPoints(); ++pt_idx )
      {
        gp_Pnt P = Defl.Value(pt_idx);

        if ( pt_idx > 1 )
        {
          const gp_Dir vecTo     = P.XYZ() - P_prev.XYZ();
          const double sparsity  = P.Distance(P_prev);
          const double step      = 50; // [mm]
          const int    numNewPts = (int) (sparsity / step) - 1;

          for ( int kk = 1; kk <= numNewPts; ++kk )
          {
            gp_Pnt P_enriching = P_prev.XYZ() + vecTo.XYZ()*step*kk;
            pts.Append(P_enriching);
          }
        }

        pts.Append(P);
        ++nContourVerts;
        Pm += P.XYZ();

#if defined DRAW_DEBUG && defined DRAW_DEBUG_SEGMENTATION
        DRAW_INITGROUP(CC_ReapproxContour_Raw)
        DRAW_POINT(P, CC_ReapproxContour_Raw, Draw_rouge, Draw_Plus)
#endif

        P_prev = P;
      }
    }
  }
  if ( nContourVerts )
    Pm /= nContourVerts;

  // Set center point.
  m_center = Pm;

  /* ==================================
   *  Eliminate near-coincident points
   * ================================== */

  TColgp_SequenceOfPnt refined_pts;
  this->makeCoarser(pts, m_fPrec, refined_pts);

  /* ===========================================================
   *  Make segmentation with extraction of principal directions
   * =========================================================== */

  bool failed = false;
  int pt_idx_origin = 1;
  const int num_refined_pts = refined_pts.Length();
  while ( pt_idx_origin <= num_refined_pts )
  {
    gp_Pnt P_origin = refined_pts(pt_idx_origin);
    TColgp_SequenceOfPnt segment;
    segment.Append(P_origin);

    int next_idx = pt_idx_origin + 1;
    int last_idx = next_idx;

    gp_Pnt P_next = refined_pts( ranged(next_idx, num_refined_pts) );
    gp_Pnt P_last;
    gp_Pnt P_prev = P_next;

    gp_Vec Principal_prev = P_next.XYZ() - P_origin.XYZ();
    gp_Vec Principal;

    // Assess fracture angle only for non-boundary points.
    if ( pt_idx_origin < num_refined_pts )
    {
      double cumul_ang = 0.0;
      double ang;
      bool stop = false;
      do
      {
        next_idx++;

        const int next_idx_ranged = ranged(next_idx, num_refined_pts);
        if ( next_idx_ranged == pt_idx_origin )
        {
          // We seem to return back to seed. That's very bad.
          stop   = true;
          failed = true;
          break;
        }

        P_next = refined_pts(next_idx_ranged);

        gp_Dir Principal_next = P_next.XYZ() - P_prev.XYZ();
        ang = Abs( Principal_prev.Angle(Principal_next) ) + cumul_ang;

        if ( ang > ToRad(m_fBarrierAngleDeg) || next_idx_ranged == 1 )
        {
          cumul_ang = 0.0;
          stop      = true;
          Principal = Principal_prev;
          P_last    = P_prev;
          last_idx  = next_idx - 1;

#if defined DRAW_DEBUG && defined DRAW_DEBUG_SEGMENTATION
          DRAW_INITGROUP(CC_ReapproxContour)
          DRAW_POINT(P_last, CC_ReapproxContour, Draw_vert, Draw_Square)
#endif
        }
        else
        {
          cumul_ang      = useAccumulatedAngle ? ang : 0.0;
          Principal_prev = Principal_next;
          P_prev         = P_next;
          Principal      = Principal_next;
          P_last         = P_next;
        }
      }
      while ( !stop );

      if ( failed || !useSegments )
        Principal = gp::Origin().XYZ();
      else
        Principal = P_last.XYZ() - P_origin.XYZ();
    }
    else
    {
      Principal = Principal_prev;
    }

    // ...
    // Prepare segment
    // ...

    t_segment seg;

    // Populate points.
    seg.Pts = new TColgp_HSequenceOfPnt;
    for ( int k = pt_idx_origin; k <= last_idx; ++k )
      seg.Pts->Append( refined_pts( ranged(k, num_refined_pts) ) );

    // Set principal if detected.
    if ( Principal.Magnitude() > Precision::Confusion() )
    {
      gp_Lin Principal_lin(P_origin, Principal);
      seg.Principal    = Principal_lin;
      seg.HasPrincipal = true;
    }
    else
      seg.HasPrincipal = false;

    // Populate result.
    segments.push_back(seg);

    // Continue.
    pt_idx_origin = last_idx;
  }

#if defined DRAW_DEBUG && defined DRAW_DEBUG_SEGMENTATION
  // Dump to Draw
  DRAW_INITGROUP(CC_ReapproxContour_Seg)
  DRAW_INITGROUP(CC_ReapproxContour_Principal)
  DRAW_INITGROUP(CC_ReapproxContour_Pt)

  std::cout << "CC_ReapproxContour: num. derived segments is " << Segments.Length() << std::endl;

  for ( int seg_idx = 1; seg_idx <= Segments.Length(); ++seg_idx )
  {
    const Segment& seg = Segments(seg_idx);

    DRAW_LINE3D(seg.Pts->Value(1), seg.Pts->Value( seg.Pts->Length() ), CC_ReapproxContour_Principal, Draw_bleu)
    DRAW_POINT(seg.Pts->Value(1), CC_ReapproxContour_Pt, Draw_rouge, Draw_Circle)

    for ( int pt_idx = 1; pt_idx <= seg.Pts->Length(); ++pt_idx )
    {
      DRAW_POINT(seg.Pts->Value(pt_idx), CC_ReapproxContour_Seg, Draw_vert, Draw_Square)
    }
  }
#endif

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_ReapproxContour::doApprox(const std::vector<t_segment>&    segments,
                                       std::vector<Handle(Geom_Curve)>& curves)
{
  // Check if segmentation was done successfully. Otherwise we cannot
  // re-approximate segment-by-segment.
  bool isSegmented = true;
  for ( int seg_idx = 0; seg_idx < int( segments.size() ); ++seg_idx )
  {
    if ( !segments[seg_idx].HasPrincipal )
    {
      isSegmented = false;
      break;
    }
  }

  Handle(TColgp_HSequenceOfPnt) all_points;
  if ( !isSegmented )
    all_points = new TColgp_HSequenceOfPnt;

  gp_XYZ ori;
  int nTerms = 0;
  for ( int seg_idx = 0; seg_idx < int( segments.size() ); ++seg_idx )
  {
    const t_segment& seg = segments[seg_idx];

    Handle(TColStd_HSequenceOfReal) params_seq = (isSegmented ? new TColStd_HSequenceOfReal : NULL);
    Handle(TColgp_HSequenceOfPnt)   points_seq = (isSegmented ? new TColgp_HSequenceOfPnt   : all_points);

    // Evaluate parameters filtering out those conflicting against the
    // principal. Thus we ensure that the resulting curve does not contain
    // reverse points.
    for ( int pt_idx = 1; pt_idx <= seg.Pts->Length(); ++pt_idx )
    {
      const gp_Pnt& P = seg.Pts->Value(pt_idx);

      bool isOk = true;
      if ( isSegmented )
      {
        // Project point on principal.
        GeomAPI_ProjectPointOnCurve Proj( P, new Geom_Line(seg.Principal) );
        const double param = Proj.Parameter(1);

        // Check that the coming parameter is greater than existing ones.
        for ( int k = 1; k <= params_seq->Length(); ++k )
        {
          if ( ( param - params_seq->Value(k) ) < RealEpsilon() )
          {
            isOk = false;
            break;
          }
        }

        if ( isOk )
          params_seq->Append(param);
      }

      if ( isOk )
        points_seq->Append(P);
    }

    if ( !isSegmented )
      continue;

    // Make proper arrays.
    Handle(TColStd_HArray1OfReal) params = new TColStd_HArray1OfReal( 1, params_seq->Length() );
    Handle(TColgp_HArray1OfPnt)   points = new TColgp_HArray1OfPnt  ( 1, points_seq->Length() );

    // Populate arrays.
    const int nSegPts = points->Length();
    for ( int k = 1; k <= nSegPts; ++k )
    {
      if ( !params.IsNull() )
        params->ChangeValue(k) = params_seq->Value(k);

      points->ChangeValue(k) = points_seq->Value(k);

      const gp_Pnt& P1 = points_seq->Value(k);
      const gp_Pnt& P2 = points_seq->Value(k == nSegPts ? 1 : k + 1);

#if defined DRAW_DEBUG && defined DRAW_DEBUG_APPROX
      DRAW_INITGROUP(pointseq_approx)
      DRAW_POINT(P1, pointseq_approx, Draw_blanc, Draw_Plus)
#endif

      gp_XYZ n = ( P1.XYZ() - m_center.XYZ() ).Crossed( P2.XYZ() - m_center.XYZ() );
      if ( n.SquareModulus() > Square(m_fPrec) )
      {
        ori += n.Normalized();
        ++nTerms;
      }
    }

    // Interpolate.
    GeomAPI_Interpolate Interp( points, params, 0, Precision::Confusion() );
    //
    Interp.Perform();
    curves.push_back( Interp.Curve() );

#if defined DRAW_DEBUG && defined DRAW_DEBUG_APPROX
    DRAW_INITGROUP(CC_ReapproxContour_curve)
    DRAW_CURVE(Interp.Curve(), CC_ReapproxContour_curve, Draw_blanc)
#endif
  }

  // If no segmentation was done, let's interpolate all points.
  if ( !isSegmented )
  {
    // Remove coincident points.
    TColgp_SequenceOfPnt sparsed;
    this->makeCoarser(all_points->Sequence(), m_fPrec, sparsed);

    // Repack to array.
    Handle(TColgp_HArray1OfPnt) all_points_arr = new TColgp_HArray1OfPnt( 1, sparsed.Length() );
    for ( int k = 1; k <= sparsed.Length(); ++k )
    {
      all_points_arr->ChangeValue(k) = sparsed(k);
    }

    // Interpolate.
    GeomAPI_Interpolate Interp( all_points_arr, m_bClosed ? 1 : 0, Precision::Confusion() );
    Interp.Perform();
    curves.push_back( Interp.Curve() );

#if defined DRAW_DEBUG && defined DRAW_DEBUG_APPROX
    DRAW_INITGROUP(CC_ReapproxContour_curve)
    DRAW_CURVE(Interp.Curve(), CC_ReapproxContour_curve, Draw_blanc)
#endif
  }

  if ( nTerms )
    ori /= nTerms;

  // Save orientation.
  m_ori = ori;
  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_ReapproxContour::doWire(const std::vector<Handle(Geom_Curve)>& curves,
                                     TopoDS_Wire&                           wire) const
{
  Handle(TopTools_HSequenceOfShape) edges = new TopTools_HSequenceOfShape;
  for ( int crv_idx = 0; crv_idx < int( curves.size() ); ++crv_idx )
  {
    const Handle(Geom_Curve)& C = curves[crv_idx];
    TopoDS_Edge E = BRepBuilderAPI_MakeEdge(C);
    edges->Append(E);
  }

  Handle(TopTools_HSequenceOfShape) wires;
  ShapeAnalysis_FreeBounds::ConnectEdgesToWires(edges, Precision::Confusion(), 0, wires);

  if ( wires.IsNull() || wires->Length() != 1 )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot assemble a single wire");
    return false;
  }

  // Set result.
  wire = TopoDS::Wire( wires->Value(1) );
  return true;
}

//-----------------------------------------------------------------------------

void asiAlgo_ReapproxContour::makeCoarser(const TColgp_SequenceOfPnt& source,
                                          const double                resolution,
                                          TColgp_SequenceOfPnt&       result) const
{
  int          pt_idx     = 1;
  const int    nSource    = source.Length();
  const double cornerPrec = resolution / 20;

#if defined DRAW_DEBUG && defined DRAW_DEBUG_REFINE
  DRAW_INITGROUP(CC_ReapproxContour_Refine)
  DRAW_POINT(source(1), CC_ReapproxContour_Refine, Draw_blanc, Draw_Circle)
#endif
  result.Append( source(1) ); // First point is always in a result.

  // Add other points.
  while ( pt_idx < nSource )
  {
    const gp_Pnt& P = source(pt_idx);

    // Select next point which is distant enough.
    gp_Pnt P_next;
    int    next_idx        = pt_idx;
    double dist            = 0.0;
    bool   letPoint2Result = false;
    do
    {
      next_idx++;
      if ( next_idx > nSource )
      {
        letPoint2Result = false;
        break;
      }

      P_next = source(next_idx);
      dist   = P.Distance(P_next);

      const bool isTooClose = (dist < resolution);
      letPoint2Result = !isTooClose;

      // Give the next point a last chance to pass to the result. It may
      // happen if its distance from the previous is not appropriate (however,
      // not critically for further interpolation), but there is a significant
      // fracture in the traverse direction.
      if ( isTooClose && dist > cornerPrec )
      {
        // Choose next point
        int next_next_idx = 0;
        int k = next_idx;
        for ( ; k <= nSource; ++k )
        {
          if ( source(k).Distance(P_next) < cornerPrec )
            continue;

          next_next_idx = k;
          break;
        }

        if ( next_next_idx )
        {
          gp_Pnt P_next_next = source(next_next_idx);
          gp_Dir D_next      = P_next_next.XYZ() - P_next.XYZ();
          gp_Dir D           = P_next.XYZ() - P.XYZ();

          const double ang = Abs( D_next.Angle(D) );
          if ( ang > ToRad(m_fBarrierAngleDeg) )
            letPoint2Result = false;
        }
      }
    }
    while ( !letPoint2Result );

    // Add next point to the refined set.
    if ( letPoint2Result )
    {
#if defined DRAW_DEBUG && defined DRAW_DEBUG_REFINE
      DRAW_POINT(P_next, CC_ReapproxContour_Refine, Draw_blanc, Draw_Circle)
#endif

      // Check if such point is not already in the result.
      bool cannotAppend = false;
      for ( int pi = 1; pi <= result.Length(); ++pi )
      {
        if ( result(pi).Distance(P_next) < resolution )
        {
          cannotAppend = true;
          break;
        }
      }
      if ( !cannotAppend )
        result.Append(P_next);
    }

    pt_idx = next_idx;
  }

  if ( result.Length() < 2 )
  {
    result.Clear();
    result.Append( source(1) );
    result.Append( source(nSource) );

    if ( result(1).Distance( result(2) ) < Precision::Confusion() )
      Standard_ProgramError::Raise("Impossible segment");
  }
}
