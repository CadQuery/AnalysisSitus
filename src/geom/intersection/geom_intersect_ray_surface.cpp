//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_intersect_ray_surface.h>

// OCCT includes
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI_IntCS.hxx>
#include <Precision.hxx>

//! Constructor.
//! \param Surf      [in] surface to intersect with the ray.
//! \param precision [in] precision for the underlying intersection method.
geom_intersect_ray_surface::geom_intersect_ray_surface(const Handle(Geom_Surface)& Surf,
                                                       const double                precision)
: m_surf(Surf), m_fPrec(precision)
{
}

//! Performs ray-surface intersection.
//! \param Ray     [in]     ray to intersect with the surface.
//! \param Result  [out]    intersection points.
//! \param Journal [in/out] journal instance.
void geom_intersect_ray_surface::operator()(const gp_Ax1&                         Ray,
                                            geom_intersection_points_ray_surface& Result,
                                            ActAPI_ProgressEntry                  ASitus_NotUsed(Journal))
{
  Handle(Geom_Line)         baseLine = new Geom_Line(Ray);
  Handle(Geom_TrimmedCurve) rayLine  = new Geom_TrimmedCurve( baseLine, 0.0, Precision::Infinite() );

  // Intersect and populate the result list
  GeomAPI_IntCS Inter(rayLine, m_surf);
  if ( Inter.IsDone() )
  {
    for ( int iSol = 1; iSol <= Inter.NbPoints(); ++iSol )
    {
      double u, v, w;
      Inter.Parameters(iSol, u, v, w);
      Result.Append( geom_intersection_point_ray_surface(Ray.Direction(), gp_Pnt2d(u, v), w, false) );
    }
  }
}
