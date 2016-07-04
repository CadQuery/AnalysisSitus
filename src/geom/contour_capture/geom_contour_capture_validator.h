//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_contour_capture_validator_h
#define geom_contour_capture_validator_h

// Analysis Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <TopoDS_Shape.hxx>

//! Tool to validate the contour capturing results. It accepts the original
//! contour together with the captured shell. The idea is to check that the
//! outer wire of the captured shell does not deviate significantly from the
//! given contour. Several options can be proposed for such validation. The
//! simplest one (however, not very precise) consists in comparison of the
//! axis-aligned bounding boxes for the two arguments (contour and shell).
//! If the deviation between these boxes is negligible (configured with
//! a dedicated input parameter), then the capturing is said to be done
//! correctly.
class geom_contour_capture_validator
{
public:

  ASitus_EXPORT
    geom_contour_capture_validator();

public:

  ASitus_EXPORT bool
    Compare_AABBs(const TopoDS_Shape&  Body,
                  const TopoDS_Shape&  Contour,
                  const int            deviation_percentage,
                  const double         tess_precision,
                  ActAPI_ProgressEntry Journal) const;

};

#endif
