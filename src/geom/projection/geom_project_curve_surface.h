//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_project_curve_surface_h
#define geom_project_curve_surface_h

// Analysis Situs includes
#include <analysis_situs.h>

// Geometry includes
#include <geom_curve_surface_dist.h>

// OCCT includes
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom2d_Curve.hxx>

//! Curve-to-Surface projection tool.
class geom_project_curve_surface
{
public:

  ASitus_EXPORT
    geom_project_curve_surface(const Handle(Geom_Surface)& Surf,
                               const double                precision);

public:

  ASitus_EXPORT void
    operator()(Handle(Geom_Curve)&   Curve,
               const double          f,
               const double          l,
               Handle(Geom2d_Curve)& Result,
               double&               gap,
               ActAPI_ProgressEntry  Journal);

private:

  Handle(Geom_Surface)    m_surf;     //!< Working surface.
  double                  m_fPrec;    //!< Required precision.
  geom_curve_surface_dist m_distTool; //!< Tool to measure distance.

};

#endif
