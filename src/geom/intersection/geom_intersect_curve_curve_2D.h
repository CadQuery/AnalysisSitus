//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_intersect_curve_curve_2D_h
#define geom_intersect_curve_curve_2D_h

// Geometry includes
#include <geom_intersection_point_cc_2D.h>

// OCCT includes
#include <Geom2d_Curve.hxx>
#include <Geom_Surface.hxx>

//! Curve-curve intersection tool in two-dimensional space.
class geom_intersect_curve_curve_2D
{
public:

  ASitus_EXPORT
    geom_intersect_curve_curve_2D(const Handle(Geom2d_Curve)& Curve,
                                  const Handle(Geom_Surface)& Surface,
                                  const double                precision);

public:

  ASitus_EXPORT void
    operator()(const Handle(Geom2d_Curve)&     Curve,
               const double                    f,
               const double                    l,
               geom_intersection_points_cc_2D& Result,
               ActAPI_ProgressEntry            Journal);

private:

  Handle(Geom2d_Curve) m_curve; //!< Working curve.
  Handle(Geom_Surface) m_surf;  //!< Host surface.
  double               m_fPrec; //!< Required precision.

};

#endif
