//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_contour_orientation_h
#define geom_contour_orientation_h

// Analysis Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <TopoDS_Shape.hxx>

//! Tool to calculate original orientation of a contour.
class geom_contour_orientation
{
public:

  ASitus_EXPORT
    geom_contour_orientation();

public:

  ASitus_EXPORT bool
    operator()(const TopoDS_Shape&  Contour,
               const double         precision,
               gp_Pnt&              result_Center,
               gp_Vec&              result_Vector,
               ActAPI_ProgressEntry PEntry) const;

};

#endif
