//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_classify_edge_solid.h>

// Geometry includes
#include <geom_classify_point_face_N_OOP.h>
#include <geom_classify_point_solid.h>
#include <geom_epsilon_empty.h>
#include <geom_intersect_curve_surface.h>
#include <geom_overlapping_curve_surface.h>
#include <geom_transition.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <NCollection_QuickSort.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Vertex.hxx>

//! Constructor.
//! \param S [in] working solid.
geom_classify_edge_solid::geom_classify_edge_solid(const TopoDS_Solid& S)
: m_S(S)
{}

//! Performs actual classification.
//! \param E         [in]     edge to classify.
//! \param IntListNC [out]    list of intersection points and memberships.
//! \param Journal   [in/out] journal instance.
//! \return classification result.
geom_membership
  geom_classify_edge_solid::operator()(const TopoDS_Edge&                      E,
                                       geom_intersection_points_probe_surface& IntListNC,
                                       ActAPI_ProgressEntry                    Journal) const
{
  // Empty set of intersections. Each intersection point is
  // identified by its parameter along the edge, neighborhood and
  // classification (NC) information
  IntListNC.Clear();

  // Point-Solid classification tool which costs nothing to construct
  // by can be useful in some circumstances
  geom_classify_point_solid ClassifyPtSolid(m_S, false);

  // Access edge elements
  double f, l;
  Handle(Geom_Curve) C = BRep_Tool::Curve(E, f, l);

  // Loop over the tentative faces
  for ( TopExp_Explorer exp_f(m_S, TopAbs_FACE); exp_f.More(); exp_f.Next() )
  {
    // Access face elements
    const TopoDS_Face&   F     = TopoDS::Face( exp_f.Current() );
    Handle(Geom_Surface) Surf  = BRep_Tool::Surface(F);
    const double         tol_f = BRep_Tool::Tolerance(F);

    // Check if the curve lies on the surface
    geom_overlapping_curve_surface OverlapCurveSurf(Surf, tol_f);
    if ( OverlapCurveSurf(C, Journal) == Overlapping_Yes )
      continue;

    // Intersect curve with the surface
    geom_intersection_points_probe_surface PList;
    geom_intersect_curve_surface IntersectCurveSurf;
    IntersectCurveSurf(C, f, l, Surf, PList);

    // Classify intersection points against the working face
    for ( int ip = 1; ip <= PList.Length(); ++ip )
    {
      geom_intersection_point_probe_surface& p = PList(ip);

      // Classify against the face
      Handle(geom_epsilon_base) nbh;
      geom_classify_point_face_N_OOP ClassPtFaceN(F, tol_f, tol_f);
      geom_membership p_class = ClassPtFaceN(p.UV, nbh, Journal);
      if ( p_class & Membership_InOn )
      {
        // Attach neighborhood information
        p.Neighborhood = nbh;

        // Populate the list of intersection points with neighborhood information
        IntListNC.Append(p);
      }
    }
  } // Loop by faces

  // Check if there are no intersection points. If so, then the probe edge is
  // lying completely inside or outside the solid. Thus it is easy to classify
  // the whole edge by a single point
  if ( IntListNC.IsEmpty() )
  {
    const gp_Pnt q = C->Value( (f + l)*0.5 );
    return ClassifyPtSolid(q, Journal); // Single point classification does the trick
  }

  // Add edge's extremities to the list of transitions
  IntListNC.Append( geom_intersection_point_probe_surface(C->Value(f), f) );
  IntListNC.Append( geom_intersection_point_probe_surface(C->Value(l), l) );

  // Sort intersection points by parameters
  NCollection_Comparator<geom_intersection_point_probe_surface> Comp;
  NCollection_QuickSort<geom_intersection_points_probe_surface,
                        geom_intersection_point_probe_surface>::Perform( IntListNC, Comp,
                                                                         1, IntListNC.Length() );

  // Remove duplicated points if any
  geom_classifier::RemoveCoincidentPoints(Precision::Confusion(), IntListNC);

  // Populate transition properties for each point
  for ( int i = 1; i <= IntListNC.Length(); ++i )
  {
    geom_intersection_point_probe_surface& p = IntListNC(i);
    if ( p.Neighborhood.IsNull() || p.Neighborhood->IsInstance( STANDARD_TYPE(geom_epsilon_empty) ) )
    {
      p.Membership = Membership_Unknown;
    }
    else
    { 
      // Assess transition and associate the corresponding membership
      // information with our current and previous point
      const geom_transition::Type furcation = geom_transition::Resolve(p);
      if ( furcation == geom_transition::InOut ||
           furcation == geom_transition::OutOut )
      {
        p.Membership = Membership_Out;
      }
      else if ( furcation == geom_transition::OutIn ||
                furcation == geom_transition::InIn )
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
  // the solid in a vertex, we agree to set Unknown classification to such
  // points
  for ( int i = 1; i < IntListNC.Length(); ++i )
  {
    geom_intersection_point_probe_surface& p      = IntListNC(i);
    geom_intersection_point_probe_surface& p_next = IntListNC(i + 1);

    if ( p.Membership == Membership_Unknown )
    {
      gp_XYZ q = ( p.P.XYZ() + p_next.P.XYZ() )*0.5;
      p.Membership = ClassifyPtSolid(q, Journal); // Classify by solid
    }
  }

  // Classify the trailing point to have full transition portrait
  IntListNC.ChangeLast().Membership = ClassifyPtSolid( IntListNC.ChangeLast().P, Journal );

  // Clean up neighborhood information
  for ( int i = 1; i <= IntListNC.Length(); ++i )
  {
    geom_intersection_point_probe_surface& p = IntListNC(i);
    p.Neighborhood.Nullify();
  }

  return Membership_Composite;
}
