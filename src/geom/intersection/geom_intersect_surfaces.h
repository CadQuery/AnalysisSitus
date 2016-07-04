//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_intersect_surfaces_h
#define geom_intersect_surfaces_h

// A-Situs includes
#include <analysis_situs.h>

// Geometry includes
#include <geom_intersection_curve_ss.h>

// OCCT includes
#include <TColGeom_SequenceOfSurface.hxx>

//-----------------------------------------------------------------------------

//! Utility to intersect parametric surfaces.
class geom_intersect_surfaces
{
public:

  ASitus_EXPORT
    geom_intersect_surfaces(const double prec);

public:

  ASitus_EXPORT void
    operator()(const TColGeom_SequenceOfSurface& surfaces,
               geom_intersection_curves_ss&      result) const;

private:

  double m_fPrec; //!< Precision.

};

#endif
