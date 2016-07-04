//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_epsilon_face_interior_hemi_h
#define geom_epsilon_face_interior_hemi_h

// Geometry includes
#include <geom_epsilon_base.h>

// OCCT includes
#include <gp_Dir.hxx>

DEFINE_STANDARD_HANDLE(geom_epsilon_face_interior_hemi, geom_epsilon_base)

//! Neighborhood for interior point on a face's edge in 3D. In our convention
//! this neighborhood is (first of all, but not only) a normal vector pointing
//! outside the material.
class geom_epsilon_face_interior_hemi : public geom_epsilon_base
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_epsilon_face_interior_hemi, geom_epsilon_base)

public:

  //! Constructor.
  //! \param _N [in] normal direction to the face's edge taking into account
  //!                its orientation.
  //! \param _n [in] normal direction to the face surface which was used to
  //!                compute this neighborhood.
  inline geom_epsilon_face_interior_hemi(const gp_Dir& _N,
                                         const gp_Dir& _n)
  : geom_epsilon_base(), N(_N), n(_n) {}

public:

  gp_Dir N; //!< Normal vector to the face's edge with respect to the
            //!< "pointing outwards" convention.

  gp_Dir n; //!< Natural surface orientation vector at the probe point
            //!< where the neighborhood information was calculated.

};

#endif
