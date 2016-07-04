//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_classify_point_surface_h
#define geom_classify_point_surface_h

// Geometry includes
#include <geom_classifier.h>

// OCCT includes
#include <ShapeAnalysis_Surface.hxx>

//! Point-Surface classifier.
class geom_classify_point_surface
{
public:

  ASitus_EXPORT
    geom_classify_point_surface(const Handle(Geom_Surface)& Surf,
                                const double                precision);

public:

  ASitus_EXPORT geom_membership
    operator()(const gp_Pnt&        P,
               gp_Pnt2d&            PonS,
               ActAPI_ProgressEntry Journal);

  ASitus_EXPORT geom_membership
    operator()(const gp_Pnt&        P,
               ActAPI_ProgressEntry Journal);

private:

  double                m_fPrec; //!< Required precision.
  ShapeAnalysis_Surface m_SAS;   //!< Surface tools.

};

#endif
