//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_transition.h>

// Geometry includes
#include <geom_epsilon_face_interior_hemi.h>
#include <geom_epsilon_solid_interior_hemi.h>

//! Resolves transition type at the passed point.
//! \param PtOnRaySurface [in] point to resolve the transition type in.
//! \return type of transition.
geom_transition::Type
  geom_transition::Resolve(const geom_intersection_point_ray_surface& PtOnRaySurface)
{
  if ( PtOnRaySurface.Neighborhood.IsNull() ||
      !PtOnRaySurface.Neighborhood->IsInstance( STANDARD_TYPE(geom_epsilon_solid_interior_hemi) ) )
    return Unknown;

  Handle(geom_epsilon_solid_interior_hemi)
    NbhSolidInterior = Handle(geom_epsilon_solid_interior_hemi)::DownCast(PtOnRaySurface.Neighborhood);

  return resolveByDirections(PtOnRaySurface.Dir, NbhSolidInterior->N);
}

//! Resolves transition type at the passed point.
//! \param PtOnCurveSurface [in] point to resolve the transition type in.
//! \return type of transition.
geom_transition::Type
  geom_transition::Resolve(const geom_intersection_point_probe_surface& PtOnCurveSurface)
{
  if ( PtOnCurveSurface.Neighborhood.IsNull() ||
      !PtOnCurveSurface.Neighborhood->IsInstance( STANDARD_TYPE(geom_epsilon_solid_interior_hemi) ) )
    return Unknown;

  Handle(geom_epsilon_solid_interior_hemi)
    NbhSolidInterior = Handle(geom_epsilon_solid_interior_hemi)::DownCast(PtOnCurveSurface.Neighborhood);

  return resolveByDirections(PtOnCurveSurface.T, NbhSolidInterior->N);
}

//! Resolves transition type at the passed point.
//! \param PtOnCurveCurve [in] point to resolve the transition type in.
//! \return type of transition.
geom_transition::Type
  geom_transition::Resolve(const geom_intersection_point_probe_curve& PtOnCurveCurve)
{
  if ( PtOnCurveCurve.Neighborhood.IsNull() ||
      !PtOnCurveCurve.Neighborhood->IsInstance( STANDARD_TYPE(geom_epsilon_face_interior_hemi) ) )
    return Unknown;

  Handle(geom_epsilon_face_interior_hemi)
    NbhFaceInterior = Handle(geom_epsilon_face_interior_hemi)::DownCast(PtOnCurveCurve.Neighborhood);

  return resolveByDirections(PtOnCurveCurve.T, NbhFaceInterior->N);
}

//! Resolves transition by the given pair of directions.
//! \param pathDir   [in] direction at intersection point between path and a face.
//! \param bndNormal [in] normal at the boundary.
//! \return transition type.
geom_transition::Type
  geom_transition::resolveByDirections(const gp_Dir& pathDir,
                                       const gp_Dir& bndNormal)
{
  if ( bndNormal.Dot(pathDir) > 0 )
    return InOut;

  return OutIn;
}
