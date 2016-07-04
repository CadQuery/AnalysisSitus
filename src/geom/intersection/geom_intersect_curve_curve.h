//-----------------------------------------------------------------------------
// Created on: 22 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_intersect_curve_curve_h
#define geom_intersect_curve_curve_h

// CC includes
#include <geom_intersection_point_probe_curve.h>

// OCCT includes
#include <Geom2d_Curve.hxx>
#include <Geom_Surface.hxx>

//! Curve-curve intersection tool in three-dimensional space.
class geom_intersect_curve_curve
{
public:

  ASitus_EXPORT
    geom_intersect_curve_curve(const Handle(Geom_Curve)& Curve,
                               const double              f,
                               const double              l,
                               const double              precision);

public:

  ASitus_EXPORT void
    operator()(const Handle(Geom_Curve)&             Curve,
               const double                          f,
               const double                          l,
               geom_intersection_points_probe_curve& Result,
               ActAPI_ProgressEntry                  Journal);

private:

  Handle(Geom_Curve) m_curve;  //!< Working curve.
  double             m_fFirst; //!< First parameter on the working curve.
  double             m_fLast;  //!< Last parameter on the working curve.
  double             m_fPrec;  //!< Required precision.

};

#endif
