//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_classify_point_solid.h>

// Geometry includes
#include <geom_classify_point_face_N_OOP.h>
#include <geom_classify_point_surface.h>
#include <geom_intersect_ray_surface.h>
#include <geom_overlapping_ray_surface.h>
#include <geom_transition.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>

#define MAX_RAY_ATTEMPTS 100

//! Constructor.
//! \param S             [in] working solid.
//! \param isOpenCascade [in] switch to OpenCascade's classifier.
geom_classify_point_solid::geom_classify_point_solid(const TopoDS_Solid& S,
                                                     const bool          isOpenCascade)
: m_S(S),
  m_bOcctMode(isOpenCascade)
{
  m_occtClassifier.Load(m_S);
}

//! Performs point-solid classification.
//! \param P             [in]     point to classify.
//! \param Journal       [in/out] journal instance.
//! \return classification result.
geom_membership
  geom_classify_point_solid::operator()(const gp_Pnt&        P,
                                        ActAPI_ProgressEntry Journal)
{
  if ( m_bOcctMode )
  {
    m_occtClassifier.Perform(P, 1.0e-7);
    const TopAbs_State state = m_occtClassifier.State();
    //
    if ( state == TopAbs_IN )
      return Membership_In;
    if ( state == TopAbs_ON )
      return Membership_On;
    if ( state == TopAbs_OUT )
      return Membership_Out;

    return Membership_Unknown;
  }

  /* =============================================================
   *  Check if the point belongs to a face: if so, it is ON solid
   * ============================================================= */

  // Loop over the tentative faces
  for ( TopExp_Explorer exp_f(m_S, TopAbs_FACE); exp_f.More(); exp_f.Next() )
  {
    const TopoDS_Face&   F     = TopoDS::Face( exp_f.Current() );
    Handle(Geom_Surface) S     = BRep_Tool::Surface(F);
    const double         tol_f = BRep_Tool::Tolerance(F);

    // Classify against the host surface
    gp_Pnt2d PonS;
    geom_classify_point_surface ClassPtSurf(S, tol_f);
    if ( ClassPtSurf(P, PonS, Journal) & Membership_InOn )
    {
      // Classify against the face
      geom_classify_point_face ClassPtFace(F, tol_f, tol_f);
      if ( ClassPtFace(PonS, Journal) & Membership_InOn )
        return Membership_On;
    }
  }

  /* =========================================================
   *  Now shoot a random ray to check the intersection parity
   * ========================================================= */

  int attempt = 0;
  math_BullardGenerator rng; // Out of loop to re-use seed
  do
  {
    attempt++;

    // Choose a random ray
    gp_Ax1 ray = geom_classifier::CastRay(P, rng);

    // Intersection points to check transition (the way to operate with
    // the neighborhood information)
    geom_intersection_points_ray_surface IntList;

    // Loop over the tentative faces
    for ( TopExp_Explorer exp_f(m_S, TopAbs_FACE); exp_f.More(); exp_f.Next() )
    {
      const TopoDS_Face&   F     = TopoDS::Face( exp_f.Current() );
      Handle(Geom_Surface) S     = BRep_Tool::Surface(F);
      const double         tol_f = BRep_Tool::Tolerance(F);

      // Check if the ray lies on the surface. If so, it is to be skipped
      geom_overlapping_ray_surface OverlapRaySurf(S, tol_f);
      if ( OverlapRaySurf(ray, Journal) == Overlapping_Yes )
        continue;

      // Intersect ray with the surface
      geom_intersection_points_ray_surface PList;
      geom_intersect_ray_surface IntersectRaySurf(S, tol_f);
      IntersectRaySurf(ray, PList, Journal);

      // Classify intersection points against the working face
      for ( int ip = 1; ip <= PList.Length(); ++ip )
      {
        geom_intersection_point_ray_surface& q = PList(ip);

        // Classify against the face
        Handle(geom_epsilon_base) nbh;
        geom_classify_point_face_N_OOP ClassPtFaceN(F, tol_f, tol_f);
        geom_membership q_class = ClassPtFaceN(q.UV, nbh, Journal);
        if ( q_class & Membership_InOn )
        {
          // Attach neighborhood information
          q.Neighborhood = nbh;

          // Process singularity
          if ( q_class == Membership_On )
            q.Singular = true; // Intersection on boundary

          // Populate the list of intersection points with singularity information
          IntList.Append(q);
        }
      }
    } // Loop by faces

    // No intersections, so we are definitely outside
    if ( IntList.IsEmpty() )
      return Membership_Out;

    // Get the closest point and check its singularity
    const geom_intersection_point_ray_surface& r = geom_classifier::ClosestPoint(IntList);
    if ( r.Singular )
      continue; // Let's panic and hope that another ray will be better

    // Singularity is avoided in the closest point. Therefore, we can
    // assess the neighborhood of this point with respect to the solid and
    // derive its transition property along the casted ray

    if ( geom_transition::Resolve(r) == geom_transition::InOut )
      return Membership_In;

    return Membership_Out;
  }
  while ( attempt <= MAX_RAY_ATTEMPTS );

  // If we are here, then something is completely wrong... No panic anymore, just r.i.p.
  return Membership_Unknown;
}
