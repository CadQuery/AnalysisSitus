//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_overlapping_ray_surface_h
#define geom_overlapping_ray_surface_h

// Geometry includes
#include <geom_classifier.h>

// OCCT includes
#include <Geom_Surface.hxx>
#include <gp_Ax1.hxx>

//! Checker for Ray-Surface overlapping.
class geom_overlapping_ray_surface
{
public:

  ASitus_EXPORT
    geom_overlapping_ray_surface(const Handle(Geom_Surface)& Surf,
                                 const double                precision);

public:

  ASitus_EXPORT geom_overlapping
    operator()(const gp_Ax1&        Ray,
               ActAPI_ProgressEntry Journal);

private:

  double               m_fPrec; //!< Required precision.
  Handle(Geom_Surface) m_surf;  //!< Working surface.

};

#endif
