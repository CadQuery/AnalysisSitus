//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_project_point_curve_h
#define geom_project_point_curve_h

// Analysis Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <Geom_Curve.hxx>

//! Point-to-Curve projection tool.
class geom_project_point_curve
{
public:

  ASitus_EXPORT
    geom_project_point_curve(const Handle(Geom_Curve)& Crv,
                             const double              f,
                             const double              l,
                             const double              precision);

public:

  ASitus_EXPORT void
    operator()(const gp_Pnt&        P,
               gp_Pnt&              Result_P,
               double&              Result_w,
               double&              gap,
               ActAPI_ProgressEntry PEntry);

private:

  Handle(Geom_Curve) m_curve; //!< Working curve.
  double             m_fPrec; //!< Required precision.

};

#endif
