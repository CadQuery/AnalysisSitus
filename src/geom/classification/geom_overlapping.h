//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_overlapping_h
#define geom_overlapping_h

// Analysis Situs includes
#include <analysis_situs.h>

//! Overlapping relation between two sets.
enum geom_overlapping
{
  Overlapping_Error = 0x00,
  Overlapping_Yes   = 0x01,
  Overlapping_No    = 0x02
};

#endif
