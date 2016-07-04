//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_intersection_point_probe_surface_h
#define geom_intersection_point_probe_surface_h

// Analysis Situs includes
#include <analysis_situs.h>

// Geometry includes
#include <geom_epsilon_base.h>
#include <geom_membership.h>

// OCCT includes
#include <gp_Dir.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Type.hxx>

//! Point representing intersection between a curve and a surface.
struct geom_intersection_point_probe_surface
{
public:

  //! Default constructor.
  inline geom_intersection_point_probe_surface()
  : W(0.0), Membership(Membership_Unknown)
  {}

  //! Constructor.
  //! \param _P  [in] three-dimensional point.
  inline geom_intersection_point_probe_surface(const gp_Pnt& _P)
  : P(_P), Membership(Membership_Unknown)
  {}

  //! Constructor.
  //! \param _P  [in] three-dimensional point.
  //! \param _W  [in] intersection parameter on the curve.
  inline geom_intersection_point_probe_surface(const gp_Pnt& _P,
                                               const double  _W)
  : P(_P), W(_W), Membership(Membership_Unknown)
  {}

  //! Constructor.
  //! \param _P  [in] three-dimensional point.
  //! \param _UV [in] intersection parameters on the surface.
  //! \param _W  [in] intersection parameter on the curve.
  inline geom_intersection_point_probe_surface(const gp_Pnt&   _P,
                                               const gp_Pnt2d& _UV,
                                               const double    _W)
  : P(_P), UV(_UV), W(_W), Membership(Membership_Unknown)
  {}

  //! Constructor.
  //! \param _P  [in] three-dimensional point.
  //! \param _T  [in] curve tangency at intersection point.
  //! \param _UV [in] intersection parameters on the surface.
  //! \param _W  [in] intersection parameter on the curve.
  inline geom_intersection_point_probe_surface(const gp_Pnt&   _P,
                                               const gp_Dir&   _T,
                                               const gp_Pnt2d& _UV,
                                               const double    _W)
  : P(_P), T(_T), UV(_UV), W(_W), Membership(Membership_Unknown)
  {}

public:

  //! Comparator.
  //! \param pt [in] point to compare with.
  //! \return true/false.
  inline bool operator>(const geom_intersection_point_probe_surface& pt) const
  {
    return this->W > pt.W;
  }

  //! Equality comparator.
  //! \param pt [in] point to compare with.
  //! \return true/false.
  inline bool operator==(const geom_intersection_point_probe_surface& pt) const
  {
    return this->W == pt.W;
  }

public:

  gp_Pnt                    P;            //!< Three-dimensional point.
  gp_Dir                    T;            //!< Curve tangency at the intersection point.
  gp_Pnt2d                  UV;           //!< Intersection point in the surface domain.
  double                    W;            //!< Intersection parameter on the curve.
  Handle(geom_epsilon_base) Neighborhood; //!< Optional neighborhood information.
  geom_membership           Membership;   //!< Membership associated with the point.

};

//! Collection of intersection points.
typedef NCollection_Sequence<geom_intersection_point_probe_surface> geom_intersection_points_probe_surface;

#endif
