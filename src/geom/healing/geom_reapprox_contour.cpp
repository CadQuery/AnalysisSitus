//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_reapprox_contour.h>

// Geometry includes
#include <geom_check_contour.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <GCPnts_QuasiUniformAbscissa.hxx>
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
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

#define ToDeg(Rad) Rad*180.0/M_PI
#define ToRad(Rad) Rad*M_PI/180.0

int ranged(const int I, const int NUM)
{
  return (I - 1) % NUM + 1;
}

//! Constructor.
//! \param Contour         [in] input contour to re-approximate.
//! \param precision       [in] precision to use.
//! \param barrierAngleDeg [in] barrier angle for segmentation.
geom_reapprox_contour::geom_reapprox_contour(const TopoDS_Shape& Contour,
                                             const double        precision,
                                             const double        barrierAngleDeg)
: m_contour(Contour), m_fPrec(precision), m_fBarrierAngle(barrierAngleDeg)
{
  m_bClosed = geom_check_contour().Check_connectedWire(Contour, false, m_fPrec, NULL);
}

//! Performs actual re-approximation.
//! \param Wire                [out]    resulting wire (re-approximated contour).
//! \param useSegments         [in]     indicates whether to use segmentation of wire by edges.
//! \param useAccumulatedAngle [in]     indicates whether to use accumulated angle for segmentation.
//! \param Journal             [in/out] journal instance.
//! \return true in case of success, false -- otherwise.
bool geom_reapprox_contour::operator()(TopoDS_Wire&         Wire,
                                       const bool           useSegments,
                                       const bool           useAccumulatedAngle,
                                       ActAPI_ProgressEntry Journal)
{
  // Run segmentation
  NCollection_Sequence<Segment> Segments;
  if ( !this->doSegmentation(Segments, useSegments, useAccumulatedAngle, Journal) )
    return false;

  // Run approximation
  NCollection_Sequence<Handle(Geom_Curve)> Curves;
  if ( !this->doApprox(Segments, Curves, Journal) )
    return false;

  // Make wire
  TopoDS_Wire W;
  if ( !this->doWire(Curves, W, Journal) )
    return false;

  Wire = W;
  return true;
}

//! Performs segmentation of the original contour onto point sets with
//! associated principal directions.
//! \param Segments            [out]    segmentation result.
//! \param useSegments         [in]     indicates whether to use segmentation of wire by edges.
//! \param useAccumulatedAngle [in]     indicates whether to use accumulated angle for segmentation.
//! \param Journal             [in/out] journal instance.
//! \return true in case of success, false -- otherwise.
bool geom_reapprox_contour::doSegmentation(NCollection_Sequence<Segment>& Segments,
                                            const bool                    useSegments,
                                            const bool                    useAccumulatedAngle,
                                            ActAPI_ProgressEntry          Journal)
{
  /* ========================
   *  Prepare discretization
   * ======================== */

  gp_XYZ Pm;
  int nContourVerts = 0;
  TColgp_SequenceOfPnt pts;
  for ( TopExp_Explorer exp(m_contour, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    const TopoDS_Edge& E = TopoDS::Edge( exp.Current() );

    double f, l;
    BRep_Tool::Range(E, f, l);
    BRepAdaptor_Curve BAC(E);
    GCPnts_TangentialDeflection Defl(BAC, f, l, m_fPrec, m_fPrec);

    if ( E.Orientation() == TopAbs_REVERSED )
    {
      for ( int pt_idx = Defl.NbPoints(); pt_idx >= 1; --pt_idx )
      {
        gp_Pnt P = Defl.Value(pt_idx);
        pts.Append(P);
        ++nContourVerts;
        Pm += P.XYZ();

#if defined DRAW_DEBUG
        DRAW_INITGROUP(CC_ReapproxContour_Raw)
        DRAW_POINT(P, CC_ReapproxContour_Raw, Draw_bleu, Draw_Plus)
#endif
      }
    }
    else
    {
      for ( int pt_idx = 1; pt_idx <= Defl.NbPoints(); ++pt_idx )
      {
        gp_Pnt P = Defl.Value(pt_idx);
        pts.Append(P);
        ++nContourVerts;
        Pm += P.XYZ();

#if defined DRAW_DEBUG
        DRAW_INITGROUP(CC_ReapproxContour_Raw)
        DRAW_POINT(P, CC_ReapproxContour_Raw, Draw_rouge, Draw_Plus)
#endif
      }
    }
  }
  if ( nContourVerts )
    Pm /= nContourVerts;

  // Set center point
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

    // Assess fracture angle only for non-boundary points
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
          // We seem to return back to seed. That's very bad
          stop   = true;
          failed = true;
          break;
        }

        P_next = refined_pts(next_idx_ranged);

        gp_Dir Principal_next = P_next.XYZ() - P_prev.XYZ();
        ang = Abs( Principal_prev.Angle(Principal_next) ) + cumul_ang;

        if ( ang > ToRad(BarrierAngleDeg) )
        {
          cumul_ang = 0.0;
          stop      = true;
          Principal = Principal_prev;
          P_last    = P_prev;
          last_idx  = next_idx - 1;

#if defined DRAW_DEBUG
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

    Segment seg;

    // Populate points
    seg.Pts = new TColgp_HSequenceOfPnt;
    for ( int k = pt_idx_origin; k <= last_idx; ++k )
      seg.Pts->Append( refined_pts( ranged(k, num_refined_pts) ) );

    // Set principal if detected
    if ( Principal.Magnitude() > Precision::Confusion() )
    {
      gp_Lin Principal_lin(P_origin, Principal);
      seg.Principal    = Principal_lin;
      seg.HasPrincipal = true;
    }
    else
      seg.HasPrincipal = false;

    // Populate result
    Segments.Append(seg);

    // Continue
    pt_idx_origin = last_idx;
  }

#if defined DRAW_DEBUG
  // Dump to Draw
  DRAW_INITGROUP(CC_ReapproxContour_Seg)
  DRAW_INITGROUP(CC_ReapproxContour_Principal)
  DRAW_INITGROUP(CC_ReapproxContour_Pt)
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

//! Approximates the extracted segments using projection to principals for
//! parameterization.
//! \param Segments [in]     point sets to approximate.
//! \param Curves   [out]    approximation results (one-to-one for each segment).
//! \param Journal  [in/out] journal instance.
//! \return true in case of success, false -- otherwise.
bool geom_reapprox_contour::doApprox(const NCollection_Sequence<Segment>&       Segments,
                                      NCollection_Sequence<Handle(Geom_Curve)>& Curves,
                                      ActAPI_ProgressEntry                      ASitus_NotUsed(Journal))
{
  // Check if segmentation was done successfully. Otherwise we cannot
  // re-approximate segment-by-segment
  bool isSegmented = true;
  for ( int seg_idx = 1; seg_idx <= Segments.Length(); ++seg_idx )
  {
    if ( !Segments(seg_idx).HasPrincipal )
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
  for ( int seg_idx = 1; seg_idx <= Segments.Length(); ++seg_idx )
  {
    const Segment& seg = Segments(seg_idx);

    Handle(TColStd_HSequenceOfReal) params_seq = (isSegmented ? new TColStd_HSequenceOfReal : NULL);
    Handle(TColgp_HSequenceOfPnt)   points_seq = (isSegmented ? new TColgp_HSequenceOfPnt   : all_points);

    // Evaluate parameters filtering out those conflicting against the
    // principal. Thus we ensure that the resulting curve does not contain
    // reverse points
    for ( int pt_idx = 1; pt_idx <= seg.Pts->Length(); ++pt_idx )
    {
      const gp_Pnt& P = seg.Pts->Value(pt_idx);

      bool isOk = true;
      if ( isSegmented )
      {
        // Project point on principal
        GeomAPI_ProjectPointOnCurve Proj( P, new Geom_Line(seg.Principal) );
        const double param = Proj.Parameter(1);

        // Check that the coming parameter is greater than existing ones
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

    // Make proper arrays
    Handle(TColStd_HArray1OfReal) params = new TColStd_HArray1OfReal( 1, params_seq->Length() );
    Handle(TColgp_HArray1OfPnt)   points = new TColgp_HArray1OfPnt  ( 1, points_seq->Length() );

    // Populate arrays
    const int nSegPts = points->Length();
    for ( int k = 1; k <= nSegPts; ++k )
    {
      if ( !params.IsNull() )
        params->ChangeValue(k) = params_seq->Value(k);

      points->ChangeValue(k) = points_seq->Value(k);

      const gp_Pnt& P1 = points_seq->Value(k);
      const gp_Pnt& P2 = points_seq->Value(k == nSegPts ? 1 : k + 1);

      gp_XYZ n = ( P1.XYZ() - m_center.XYZ() ).Crossed( P2.XYZ() - m_center.XYZ() );
      if ( n.SquareModulus() > Square(m_fPrec) )
      {
        ori += n.Normalized();
        ++nTerms;
      }
    }

    // Interpolate
    GeomAPI_Interpolate Interp( points, params, 0, Precision::Confusion() );
    Interp.Perform();
    Curves.Append( Interp.Curve() );

#if defined DRAW_DEBUG
    DRAW_INITGROUP(CC_ReapproxContour_curve)
    DRAW_CURVE(Interp.Curve(), CC_ReapproxContour_curve, Draw_blanc)
#endif
  }

  // If no segmentation was done, let's interpolate all points
  if ( !isSegmented )
  {
    // Remove coincident points
    TColgp_SequenceOfPnt sparsed;
    this->makeCoarser(all_points->Sequence(), m_fPrec, sparsed);

    // Repack to array
    Handle(TColgp_HArray1OfPnt) all_points_arr = new TColgp_HArray1OfPnt( 1, sparsed.Length() );
    for ( int k = 1; k <= sparsed.Length(); ++k )
    {
      all_points_arr->ChangeValue(k) = sparsed(k);
    }

    // Interpolate
    GeomAPI_Interpolate Interp( all_points_arr, m_bClosed ? 1 : 0, Precision::Confusion() );
    Interp.Perform();
    Curves.Append( Interp.Curve() );

#if defined DRAW_DEBUG
    DRAW_INITGROUP(CC_ReapproxContour_curve)
    DRAW_CURVE(Interp.Curve(), CC_ReapproxContour_curve, Draw_blanc)
#endif
  }

  if ( nTerms )
    ori /= nTerms;

  // Save orientation
  m_ori = ori;
  return true;
}

//! Prepares wire for the given collection of curves.
//! \param Curves  [in]     curves to make edges and subsequently a wire from.
//! \param W       [out]    resulting wire.
//! \param Journal [in/out] journal instance.
//! \return true in case of success, false -- otherwise.
bool geom_reapprox_contour::doWire(const NCollection_Sequence<Handle(Geom_Curve)>& Curves,
                                    TopoDS_Wire&                                   W,
                                    ActAPI_ProgressEntry                           Journal) const
{
  Handle(TopTools_HSequenceOfShape) edges = new TopTools_HSequenceOfShape;
  for ( int crv_idx = 1; crv_idx <= Curves.Length(); ++crv_idx )
  {
    const Handle(Geom_Curve)& C = Curves(crv_idx);
    TopoDS_Edge E = BRepBuilderAPI_MakeEdge(C);
    edges->Append(E);
  }

  Handle(TopTools_HSequenceOfShape) wires;
  ShapeAnalysis_FreeBounds::ConnectEdgesToWires(edges, Precision::Confusion(), 0, wires);

  if ( wires.IsNull() || wires->Length() != 1 )
  {
    Journal.SendLogMessage( LogWarn(Normal) << "geom_reapprox_contour.CannotAssembleSingleWire" );
    return false;
  }

  // Set result
  W = TopoDS::Wire( wires->Value(1) );
  return true;
}

//! Makes the passed contour coarser in sense that it looses the points whose
//! distances from each other are too small with respect to the given chord
//! length.
//! \param source     [in]  source points.
//! \param resolution [in]  max allowed chord length.
//! \param result     [out] resulting points.
void geom_reapprox_contour::makeCoarser(const TColgp_SequenceOfPnt& source,
                                         const double               resolution,
                                         TColgp_SequenceOfPnt&      result) const
{
  int          pt_idx     = 1;
  const int    nSource    = source.Length();
  const double cornerPrec = resolution / 20;

#if defined DRAW_DEBUG
  DRAW_INITGROUP(CC_ReapproxContour_Refine)
  DRAW_POINT(source(1), CC_ReapproxContour_Refine, Draw_blanc, Draw_Circle)
#endif
  result.Append( source(1) ); // First point is always in a result

  // Add other points
  while ( pt_idx < nSource )
  {
    const gp_Pnt& P = source(pt_idx);

    // Select next point which is distant enough
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
      // fracture in the traverse direction
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
          if ( ang > ToRad(BarrierAngleDeg) )
            letPoint2Result = false;
        }
      }
    }
    while ( !letPoint2Result );

    // Add next point to the refined set
    if ( letPoint2Result )
    {
#if defined DRAW_DEBUG
      DRAW_POINT(P_next, CC_ReapproxContour_Refine, Draw_blanc, Draw_Circle)
#endif

      // Check if such point is not already in the result
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
