//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_refine_tolerance_h
#define geom_refine_tolerance_h

// Analysis Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <TopoDS_Shape.hxx>

//! Attempts to refine tolerance to as small value as possible.
class geom_refine_tolerance
{
public:

  ASitus_EXPORT
    geom_refine_tolerance();

public:

  ASitus_EXPORT bool
    RebuildBounds(TopoDS_Shape&        Body,
                  ActAPI_ProgressEntry Journal) const;

  ASitus_EXPORT bool
    Degrade(TopoDS_Shape&        Body,
            const double         inaccuracy,
            ActAPI_ProgressEntry Journal) const;

};

#endif
