//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_classifier.h>

//! Generates a random ray from the given point using the passed random
//! number generator (RNG). It is important to pass the latter generator as
//! normally in "panic" classification methods we are going to select
//! different rays on each iteration (thus, we have to reuse the random seed).
//!
//! \param pos [in]     position in the model space to cast a ray from.
//! \param rng [in/out] random number generator to choose direction.
//! \return generated random ray.
gp_Ax1 geom_classifier::CastRay(const gp_Pnt&          pos,
                                math_BullardGenerator& rng)
{
  // Generate random angles in range [0, 2PI]
  const double rnd_phi_rad = 0.0 + 2*M_PI*rng.NextReal();
  const double rnd_psi_rad = 0.0 + 2*M_PI*rng.NextReal();

  // Switch to Cartesian coordinates for the random ray
  gp_Dir rnd_dir( cos(rnd_psi_rad)*cos(rnd_phi_rad),
                  cos(rnd_psi_rad)*sin(rnd_phi_rad),
                  sin(rnd_psi_rad) );

  return gp_Ax1(pos, rnd_dir);
}

//! Selects a point with the minimal [w] parameter on the ray. By construction,
//! such a point is the closest one to the ray's origin.
//! \param pts [in] collection of points to select the closest one from.
//! \return the closest intersection point.
geom_intersection_point_ray_surface
  geom_classifier::ClosestPoint(const geom_intersection_points_ray_surface& pts)
{
  int ip_result = -1;
  double min = DBL_MAX;
  for ( int ip = 1; ip <= pts.Length(); ++ip )
  {
    if ( pts(ip).W < min )
    {
      min = pts(ip).W;
      ip_result = ip;
    }
  }

  return pts(ip_result);
}

//! Removes coincident points from the passed collection and overrides it.
//! \param prec      [in]     resolution precision.
//! \param IntListNC [in/out] working collection to purify.
void geom_classifier::RemoveCoincidentPoints(const double                    prec,
                                             geom_intersection_points_edges& IntListNC)
{
  geom_intersection_points_edges purifiedList;
  int ip = 1;
  do
  {
    const geom_intersection_point_edges& p = IntListNC(ip);

    // Traverse till the end skipping all coincident points
    int jp = ip + 1;
    geom_intersection_point_edges p_next, p_best = p;
    for ( ; ; )
    {
      ip = jp;

      if ( jp > IntListNC.Length() )
        break;

      p_next = IntListNC(jp++);

      if ( p_next.P.Distance(p.P) < prec )
      {
        // If there are coincident points, prefer the one which has
        // more complete definition
        if ( !p_next.E1.IsNull() && !p_next.E2.IsNull() )
          p_best = p_next;
      }
      else
        break;
    }

    purifiedList.Append(p_best);
  }
  while ( ip <= IntListNC.Length() );

  // Override the passed list
  IntListNC = purifiedList;
}

//! Removes coincident points from the passed collection and overrides it.
//! \param prec      [in]     resolution precision.
//! \param IntListNC [in/out] working collection to purify.
void geom_classifier::RemoveCoincidentPoints(const double                            prec,
                                             geom_intersection_points_probe_surface& IntListNC)
{
  geom_intersection_points_probe_surface purifiedList;
  int ip = 1;
  do
  {
    const geom_intersection_point_probe_surface& p = IntListNC(ip);
    purifiedList.Append(p);

    // Traverse till the end skipping all coincident points
    int jp = ip + 1;
    geom_intersection_point_probe_surface p_next;
    do
    {
      ip = jp;

      if ( jp > IntListNC.Length() )
        break;

      p_next = IntListNC(jp++);
    }
    while ( p_next.P.Distance(p.P) < prec );
  }
  while ( ip <= IntListNC.Length() );

  // Override transition list
  IntListNC = purifiedList;
}
