//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_epsilon_solid_interior_full_h
#define geom_epsilon_solid_interior_full_h

// Geometry includes
#include <geom_epsilon_base.h>

// OCCT includes
#include <gp_Dir.hxx>

DEFINE_STANDARD_HANDLE(geom_epsilon_solid_interior_full, geom_epsilon_base)

//! Neighborhood for interior point of a solid. This is a full
//! epsilon-interior meaning that there is no any side specification
//! like in hemisphere epsilon.
class geom_epsilon_solid_interior_full : public geom_epsilon_base
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_epsilon_solid_interior_full, geom_epsilon_base)

public:

  //! Constructor.
  inline geom_epsilon_solid_interior_full() : geom_epsilon_base() {}

};

#endif
