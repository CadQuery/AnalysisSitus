//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_check_contour_h
#define geom_check_contour_h

// Analysis Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <TopoDS_Wire.hxx>

//! Contour checking tool.
class geom_check_contour
{
public:

  ASitus_EXPORT
    geom_check_contour();

public:

  ASitus_EXPORT bool
    Check_isSingleWire(const TopoDS_Shape&  Contour,
                       ActAPI_ProgressEntry Journal) const;

  ASitus_EXPORT bool
    Check_connectedWire(const TopoDS_Shape&  Contour,
                        const bool           doCheckSharing,
                        const double         prec,
                        ActAPI_ProgressEntry Journal) const;

};

#endif
