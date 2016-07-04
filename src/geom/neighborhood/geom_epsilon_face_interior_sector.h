//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_epsilon_face_interior_sector_h
#define geom_epsilon_face_interior_sector_h

// Geometry includes
#include <geom_epsilon_base.h>

// OCCT includes
#include <gp_Dir.hxx>

DEFINE_STANDARD_HANDLE(geom_epsilon_face_interior_sector, geom_epsilon_base)

//! Neighborhood for interior point falling into face's vertex where two
//! edges join. If you consider traversal order of a face boundary
//! (looking from position of the natural surface normal), the sector
//! neighborhood will give you information on which edge is traversed
//! the first and which one -- the second. Sector is swept from T2 to T1
//! counterclockwise (looking from natural orientation), so T2 is a tangent
//! for the previous edges, while T1 -- for the next one.
class geom_epsilon_face_interior_sector : public geom_epsilon_base
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_epsilon_face_interior_sector, geom_epsilon_base)

public:

  //! Constructor.
  //! \param _T1 [in] tangent vector at the extremity of the first edge.
  //! \param _T2 [in] tangent vector at the extremity of the second edge.
  //! \param _n  [in] normal direction to the face surface which was used to
  //!                 compute this neighborhood.
  inline geom_epsilon_face_interior_sector(const gp_Dir& _T1,
                                           const gp_Dir& _T2,
                                           const gp_Dir& _n)
  : geom_epsilon_base(), T1(_T1), T2(_T2), n(_n) {}

public:

  gp_Dir T1; //!< Tangent vector at the extremity of the first vector.
  gp_Dir T2; //!< Tangent vector at the extremity of the second vector.
  gp_Dir n;  //!< Natural surface orientation vector at the probe point
             //!< where the neighborhood information was calculated.

};

#endif
