//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_overlapping_curve_curve_h
#define geom_overlapping_curve_curve_h

// Geometry includes
#include <geom_classifier.h>

// OCCT includes
#include <Geom_Curve.hxx>

//! Checker for Curve-Curve overlapping.
class geom_overlapping_curve_curve
{
public:

  ASitus_EXPORT
    geom_overlapping_curve_curve(const Handle(Geom_Curve)& Curve,
                                 const double              f,
                                 const double              l,
                                 const double              precision);

public:

  ASitus_EXPORT geom_overlapping
    operator()(const Handle(Geom_Curve)& Curve,
               const double              f,
               const double              l,
               ActAPI_ProgressEntry      Journal);

private:

  double             m_fPrec;  //!< Required precision.
  Handle(Geom_Curve) m_curve;  //!< Working curve.
  double             m_fFirst; //!< First parameter on the working curve.
  double             m_fLast;  //!< Last parameter on the working curve.

};

#endif
