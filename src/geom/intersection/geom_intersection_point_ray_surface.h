//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_intersection_point_ray_surface_h
#define geom_intersection_point_ray_surface_h

// Analysis Situs includes
#include <analysis_situs.h>

// Geometry includes
#include <geom_epsilon_base.h>

// OCCT includes
#include <gp_Dir.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Type.hxx>

//! Point representing intersection between a ray and a surface.
struct geom_intersection_point_ray_surface
{
public:

  //! Default constructor.
  inline geom_intersection_point_ray_surface() : W(0.0), Singular(false) {}

  //! Complete constructor.
  //! \param _Dir      [in] ray direction.
  //! \param _UV       [in] intersection parameters on the surface.
  //! \param _W        [in] intersection parameter on the curve.
  //! \param _Singular [in] singularity flag.
  inline geom_intersection_point_ray_surface(const gp_Dir&   _Dir,
                                             const gp_Pnt2d& _UV,
                                             const double    _W,
                                             const bool      _Singular)
  : Dir(_Dir), UV(_UV), W(_W), Singular(_Singular)
  {}

public:

  gp_Dir                    Dir;          //!< Ray direction.
  gp_Pnt2d                  UV;           //!< Intersection point in the surface domain.
  double                    W;            //!< Intersection parameter on the curve.
  bool                      Singular;     //!< Indicates whether the intersection point
                                          //!< falls into singularity (by singularity we
                                          //!< mean "touching" the boundary).
  Handle(geom_epsilon_base) Neighborhood; //!< Optional neighborhood information.

};

//! Collection of intersection points.
typedef NCollection_Sequence<geom_intersection_point_ray_surface> geom_intersection_points_ray_surface;

#endif
