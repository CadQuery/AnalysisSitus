//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_classify_edge_face.h>

// Geometry includes
#include <geom_intersect_AABBs.h>
#include <geom_classify_point_edge_N_IP.h>
#include <geom_classify_point_face.h>
#include <geom_curve_surface_dist.h>
#include <geom_edge_edge_dist.h>
#include <geom_epsilon_empty.h>
#include <geom_intersect_curve_curve.h>
#include <geom_overlapping_curve_curve.h>
#include <geom_transition.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <NCollection_Comparator.hxx>
#include <NCollection_QuickSort.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

//! Constructor.
//! \param F          [in] working face.
//! \param inaccuracy [in] inaccuracy of the input geometry.
geom_classify_edge_face::geom_classify_edge_face(const TopoDS_Face& F,
                                                 const double       inaccuracy)
: m_F(F), m_fInaccuracy(inaccuracy)
{
  BRepBndLib::Add(m_F, m_FBounds);
}

//! Performs actual classification.
//! \param Probe     [in]     edge to classify.
//! \param IntListNC [out]    list of intersection points and memberships.
//! \param Journal   [in/out] journal instance.
//! \return classification result.
geom_membership
  geom_classify_edge_face::operator()(const TopoDS_Edge&              Probe,
                                      geom_intersection_points_edges& IntListNC,
                                      ActAPI_ProgressEntry            Journal)
{
  Bnd_Box ProbeBounds;
  return this->operator()(Probe, ProbeBounds, IntListNC, Journal);
}

//! Performs actual classification.
//! \param Probe       [in]     edge to classify.
//! \param ProbeBounds [out]    bounding box of the edge to classify.
//! \param IntListNC   [out]    list of intersection points and memberships.
//! \param Journal     [in/out] journal instance.
//! \return classification result.
geom_membership
  geom_classify_edge_face::operator()(const TopoDS_Edge&              Probe,
                                      Bnd_Box&                        ProbeBounds,
                                      geom_intersection_points_edges& IntListNC,
                                      ActAPI_ProgressEntry            Journal)
{
  /* ===================
   *  Preparation stage
   * =================== */

  // Empty set of intersections. Each intersection point is
  // identified by its parameter along the edge, neighborhood
  // and classification (NC) information
  IntListNC.Clear();

  // Access face elements
  Handle(Geom_Surface) Surf = BRep_Tool::Surface(m_F);

  // Access edge elements
  double probe_f, probe_l;
  Handle(Geom_Curve) probe_C = BRep_Tool::Curve(Probe, probe_f, probe_l);

  // Check intersection by AABBs
  geom_intersect_AABBs AABBsIntersect(m_fInaccuracy);
  if ( !AABBsIntersect(m_F, Probe, m_FBounds, ProbeBounds) )
    return Membership_Out;

  // Point-Face classification tool
  geom_classify_point_face ClassifyPtFace(m_F, m_fInaccuracy, 0.1*m_fInaccuracy);

  // Overlapping checker
  geom_overlapping_curve_curve OverlapCurveCurve(probe_C, probe_f, probe_l, m_fInaccuracy);

  // Intersection tool
  geom_intersect_curve_curve InterCurveCurve(probe_C, probe_f, probe_l, m_fInaccuracy);

  // Overlapping checker
  geom_edge_edge_dist EEDist(Probe, m_fInaccuracy);

  /* =====================
   *  Accurate processing
   * ===================== */

  // Loop over the tentative edges
  for ( TopExp_Explorer exp_e(m_F, TopAbs_EDGE); exp_e.More(); exp_e.Next() )
  {
    const TopoDS_Edge& EF = TopoDS::Edge( exp_e.Current() );

    // Filter out bad candidates
    Bnd_Box EF_Bounds;
    if ( !AABBsIntersect(EF, Probe, EF_Bounds, ProbeBounds) )
      continue;

    // Access edge elements
    TopLoc_Location L;
    double EF_f, EF_l;
    Handle(Geom_Curve) EF_C = BRep_Tool::Curve(EF, L, EF_f, EF_l);

    if ( EF_C.IsNull() )
    {
      Journal.SendLogMessage( LogWarn(Normal) << "geom_classify_edge_face.EdgeWithNullCurve" << EF );
      continue;
    }

    // Check overlapping of edges
    bool hasOverlapping = false;
    double ee_dist = RealLast();
    if ( !EEDist(EF, true, ee_dist, m_fInaccuracy*10, m_fInaccuracy*100, hasOverlapping, Journal) )
    {
      Journal.SendLogMessage( LogWarn(Normal) << "geom_classify_edge_face.CannotCalculateEdgesDistance" << Probe << EF );
      return Membership_Unknown;
    }
    else if ( hasOverlapping )
    {
      Journal.SendLogMessage( LogInfo(Normal) << "geom_classify_edge_face.OverlappingWillBeEscaped" << Probe << EF );

#if defined DRAW_DEBUG
      DRAW_INITGROUP(overlap)
      DRAW_EDGE(EF, overlap, Draw_bleu)
#endif
      return Membership_Unknown;;
    }

    // Intersect the probe curve with the boundary
    geom_intersection_points_probe_curve PList;
    InterCurveCurve(EF_C, EF_f, EF_l, PList, Journal);

    // Classify intersection points against the working face
    for ( int ip = 1; ip <= PList.Length(); ++ip )
    {
      geom_intersection_point_edges p( PList(ip), m_F, Probe, EF );

      // Classify against the face
      Handle(geom_epsilon_base) nbh;
      geom_classify_point_edge_N_IP ClassPtEdgeN(EF, m_F, m_fInaccuracy, 0.1*m_fInaccuracy);
      geom_membership p_class;
      try
      {
        p_class = ClassPtEdgeN(p, nbh, Journal);
      }
      catch ( ... )
      {
        p_class = Membership_Unknown;
      }

      if ( p_class == Membership_Unknown )
      {
        Journal.SendLogMessage( LogWarn(Normal) << "geom_classify_edge_face.CannotClassifyPointEdge" << EF );
        return Membership_Unknown;
      }

      if ( p_class & Membership_InOn )
      {
        // Attach neighborhood information
        p.Neighborhood = nbh;

        // Populate the list of intersection points with neighborhood information
        IntListNC.Append(p);
      }
    }
  } // Loop by boundary edges

  // Check if there are no intersection points. If so, then the probe edge is
  // lying completely inside or outside the face. Thus it is easy to classify
  // the whole edge by a single point
  if ( IntListNC.IsEmpty() )
  {
    const gp_Pnt q = probe_C->Value( (probe_f + probe_l)*0.5 );
    return ClassifyPtFace(q, true, Journal); // Single point classification does the trick
  }

  // Add edge's extremities to the list of transition. Notice that convention
  // here is to put probe's parameters to W1
  IntListNC.Append( geom_intersection_point_probe_curve(probe_C->Value(probe_f), probe_f, 0.0) );
  IntListNC.Append( geom_intersection_point_probe_curve(probe_C->Value(probe_l), probe_l, 0.0) );

  // Sort transition points by parameters
  NCollection_Comparator<geom_intersection_point_edges> Comp;
  NCollection_QuickSort<geom_intersection_points_edges,
                        geom_intersection_point_edges>::Perform( IntListNC, Comp,
                                                                 1, IntListNC.Length() );

  // Remove duplicated points if any
  geom_classifier::RemoveCoincidentPoints(Precision::Confusion(), IntListNC);

  // Populate transition properties for each point
  for ( int i = 1; i <= IntListNC.Length(); ++i )
  {
    geom_intersection_point_edges& p = IntListNC(i);
    if ( p.Neighborhood.IsNull() || p.Neighborhood->IsInstance( STANDARD_TYPE(geom_epsilon_empty) ) )
    {
      p.Membership = Membership_Unknown;
    }
    else
    {
      // Compute transition and associate the corresponding membership
      // information with our current and previous point
      const geom_transition::Type transition = geom_transition::Resolve(p);
      if ( transition == geom_transition::InOut ||
           transition == geom_transition::OutOut )
      {
        p.Membership = Membership_Out;
      }
      else if ( transition == geom_transition::OutIn ||
                transition == geom_transition::InIn )
      {
        p.Membership = Membership_In;
      }
      else
      {
        p.Membership = Membership_Unknown;
      }
    }
  } // Loop by intersection points

  // There can be some points with Unknown classification just because
  // these points correspond to singularities at vertices. If E intersects
  // the face in a vertex, we agree to set Unknown classification to such
  // points
  for ( int i = 1; i < IntListNC.Length(); ++i )
  {
    geom_intersection_point_edges& p      = IntListNC(i);
    geom_intersection_point_edges& p_next = IntListNC(i + 1);

    if ( p.Membership == Membership_Unknown )
    {
      gp_XYZ q = probe_C->Value( (p.W1 + p_next.W1)*0.5 ).XYZ();
      p.Membership = ClassifyPtFace(q, true, Journal); // Classify wrt face
    }
  }

  // Classify the trailing point to have full transition portrait
  IntListNC.ChangeLast().Membership = ClassifyPtFace( IntListNC.ChangeLast().P, true, Journal );

#if defined DRAW_DEBUG
  DRAW_INITGROUP(geom_classify_edge_face)
#endif

  // Clean up neighborhood information
  for ( int i = 1; i <= IntListNC.Length(); ++i )
  {
    geom_intersection_point_edges& p = IntListNC(i);
    p.Neighborhood.Nullify();

#if defined DRAW_DEBUG
    Draw_ColorKind color;
    if ( p.Membership == Membership_In )
      color = Draw_vert;
    else if ( p.Membership == Membership_On )
      color = Draw_violet;
    else
      color = Draw_rouge;

    DRAW_POINT(p.P, geom_classify_edge_face, color, Draw_Circle)
#endif
  }

  return Membership_Composite;
}
