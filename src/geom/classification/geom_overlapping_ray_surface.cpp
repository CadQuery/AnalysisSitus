//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_overlapping_ray_surface.h>

// Geometry includes
#include <geom_classify_point_surface.h>

// OCCT includes
#include <Geom_Plane.hxx>

//! Constructor.
//! \param Surf      [in] surface to classify the ray against.
//! \param precision [in] precision for the underlying classification method.
geom_overlapping_ray_surface::geom_overlapping_ray_surface(const Handle(Geom_Surface)& Surf,
                                                           const double                precision)
: m_fPrec(precision), m_surf(Surf)
{
}

//! Performs ray-surface classification.
//! \param Ray     [in]     ray to classify.
//! \param Journal [in/out] journal instance.
//! \return classification result.
geom_overlapping
  geom_overlapping_ray_surface::operator()(const gp_Ax1&        Ray,
                                           ActAPI_ProgressEntry Journal)
{
  geom_classify_point_surface ClassPtSurf(m_surf, m_fPrec);

  // In case of a plane, it is enough to check 2 points
  if ( m_surf->IsInstance( STANDARD_TYPE(Geom_Plane) ) )
  {
    const gp_Pnt& P1 = Ray.Location();
    gp_Pnt        P2 = Ray.Direction().XYZ();

    if ( ( ClassPtSurf(P1, Journal) & Membership_InOn ) &&
         ( ClassPtSurf(P2, Journal) & Membership_InOn ) )
      return Overlapping_Yes;
  }

  // TODO: there are more cases to consider, e.g. with cylindrical surface

  return Overlapping_No;
}
