//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_intersect_curves_h
#define geom_intersect_curves_h

// A-Situs includes
#include <analysis_situs.h>

// Geometry includes
#include <geom_intersection_point_cc.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_intersect_curves, ActAPI_IAlgorithm)

//! Utility to intersect parametric curves.
class geom_intersect_curves : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_intersect_curves, ActAPI_IAlgorithm)

public:

  ASitus_EXPORT
    geom_intersect_curves(const double         prec,
                          const double         uLimit,
                          ActAPI_ProgressEntry progress,
                          ActAPI_PlotterEntry  plotter);

public:

  ASitus_EXPORT void
    operator()(const geom_intersection_curves_ss& curves,
               geom_intersection_points_cc&       result);

private:

  double m_fPrec;  //!< Precision.
  double m_fLimit; //!< Parametric limit to use for infinite curves.

};

#endif
