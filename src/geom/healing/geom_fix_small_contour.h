//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_fix_small_contour_h
#define geom_fix_small_contour_h

// Analysis Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <TopoDS_Wire.hxx>

//! Fixes small edges which may exist in a contour.
class geom_fix_small_contour
{
public:

  ASitus_EXPORT
    geom_fix_small_contour();

public:

  ASitus_EXPORT bool
    operator()(const TopoDS_Shape&  Contour,
               const double         precision,
               TopoDS_Wire&         Result,
               ActAPI_ProgressEntry Journal);

};

#endif
