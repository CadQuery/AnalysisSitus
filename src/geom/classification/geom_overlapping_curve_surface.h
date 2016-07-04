//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_overlapping_curve_surface_h
#define geom_overlapping_curve_surface_h

// Geometry includes
#include <geom_classifier.h>

// OCCT includes
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>

//! Checker for Curve-Surface overlapping.
class geom_overlapping_curve_surface
{
public:

  ASitus_EXPORT
    geom_overlapping_curve_surface(const Handle(Geom_Surface)& Surf,
                                 const double                precision);

public:

  ASitus_EXPORT geom_overlapping
    operator()(const Handle(Geom_Curve)& Curve,
               ActAPI_ProgressEntry      Journal);

private:

  double               m_fPrec; //!< Required precision.
  Handle(Geom_Surface) m_surf;  //!< Working surface.

};

#endif
