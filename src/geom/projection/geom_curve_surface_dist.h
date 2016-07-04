//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_curve_surface_dist_h
#define geom_curve_surface_dist_h

// Analysis Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom2d_Curve.hxx>

//! Measures curve-to-surface distance.
class geom_curve_surface_dist
{
public:

  ASitus_EXPORT
    geom_curve_surface_dist(const Handle(Geom_Surface)& Surf);

public:

  ASitus_EXPORT void
    operator()(Handle(Geom_Curve)&  Curve,
               double&              gap,
               ActAPI_ProgressEntry Journal);

private:

  Handle(Geom_Surface) m_surf;  //!< Working surface.

};

#endif
