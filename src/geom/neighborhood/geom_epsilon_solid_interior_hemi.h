//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_epsilon_solid_interior_hemi_h
#define geom_epsilon_solid_interior_hemi_h

// Geometry includes
#include <geom_epsilon_base.h>

// OCCT includes
#include <gp_Dir.hxx>

DEFINE_STANDARD_HANDLE(geom_epsilon_solid_interior_hemi, geom_epsilon_base)

//! Neighborhood for interior point on a solid in 3D. In our convention this
//! neighborhood is simply a normal vector pointing outside the material.
class geom_epsilon_solid_interior_hemi : public geom_epsilon_base
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_epsilon_solid_interior_hemi, geom_epsilon_base)

public:

  //! Constructor.
  //! \param _N [in] normal direction to the face taking into account its
  //!                orientation.
  inline geom_epsilon_solid_interior_hemi(const gp_Dir& _N) : geom_epsilon_base(), N(_N) {}

public:

  gp_Dir N; //!< Normal vector to the face with respect to the "pointing outwards"
            //!< convention of solid modeling.

};

#endif
