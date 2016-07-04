//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_intersect_ray_surface_h
#define geom_intersect_ray_surface_h

// Geometry includes
#include <geom_intersection_point_ray_surface.h>

// OCCT includes
#include <Geom_Surface.hxx>
#include <gp_Ax1.hxx>

//! Ray-Surface intersection tool.
class geom_intersect_ray_surface
{
public:

  ASitus_EXPORT
    geom_intersect_ray_surface(const Handle(Geom_Surface)& Surf,
                               const double                precision);

public:

  ASitus_EXPORT void
    operator()(const gp_Ax1&                         Ray,
               geom_intersection_points_ray_surface& Result,
               ActAPI_ProgressEntry                  Journal);

private:

  Handle(Geom_Surface) m_surf;  //!< Working surface.
  double               m_fPrec; //!< Required precision.

};

#endif
