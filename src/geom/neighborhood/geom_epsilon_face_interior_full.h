//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_epsilon_face_interior_full_h
#define geom_epsilon_face_interior_full_h

// Geometry includes
#include <geom_epsilon_base.h>

// OCCT includes
#include <gp_Dir.hxx>

DEFINE_STANDARD_HANDLE(geom_epsilon_face_interior_full, geom_epsilon_base)

//! Neighborhood for interior point on a face. It represents a full disk with
//! the associated surface normal (natural face orientation).
class geom_epsilon_face_interior_full : public geom_epsilon_base
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_epsilon_face_interior_full, geom_epsilon_base)

public:

  //! Constructor.
  //! \param _n [in] normal direction to the face surface.
  inline geom_epsilon_face_interior_full(const gp_Dir& _n)
  : geom_epsilon_base(), n(_n) {}

public:

  gp_Dir n; //!< Natural surface orientation vector.

};

#endif
