//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_intersection_point_cc_2D_h
#define geom_intersection_point_cc_2D_h

// Geometry includes
#include <geom_epsilon_base.h>
#include <geom_membership.h>

// OCCT includes
#include <gp_Pnt2d.hxx>
#include <NCollection_Sequence.hxx>

//! Point representing intersection between two curves in 2D.
struct geom_intersection_point_cc_2D
{
public:

  //! Default constructor.
  inline geom_intersection_point_cc_2D()
  : W1(0.0), W2(0.0), Membership(Membership_Unknown)
  {}

  //! Constructor.
  //! \param _UV [in] two-dimensional point.
  inline geom_intersection_point_cc_2D(const gp_Pnt2d& _UV)
  : UV(_UV), Membership(Membership_Unknown)
  {}

  //! Constructor.
  //! \param _UV [in] two-dimensional point.
  //! \param _W1 [in] intersection parameter on the first curve.
  //! \param _W2 [in] intersection parameter on the second curve.
  inline geom_intersection_point_cc_2D(const gp_Pnt2d& _UV,
                                       const double    _W1,
                                       const double    _W2)
  : UV(_UV), W1(_W1), W2(_W2), Membership(Membership_Unknown)
  {}

  //! Constructor.
  //! \param _UV [in] two-dimensional point.
  //! \param _W1 [in] intersection parameter on the first curve.
  //! \param _W2 [in] intersection parameter on the second curve.
  //! \param _T  [in] tangency of the probe curve at the intersection point.
  inline geom_intersection_point_cc_2D(const gp_Pnt2d& _UV,
                                       const double    _W1,
                                       const double    _W2,
                                       const gp_Dir2d& _T)
  : UV(_UV), W1(_W1), W2(_W2), T(_T), Membership(Membership_Unknown)
  {}

  //! Constructor.
  //! \param _UV [in] two-dimensional point.
  //! \param _W1 [in] intersection parameter on the first curve.
  //! \param _W2 [in] intersection parameter on the second curve.
  //! \param _T  [in] tangency of the probe curve at the intersection point.
  //! \param _P  [in] three-dimensional intersection point evaluated against
  //!                 the host surface.
  inline geom_intersection_point_cc_2D(const gp_Pnt2d& _UV,
                                       const double    _W1,
                                       const double    _W2,
                                       const gp_Dir2d& _T,
                                       const gp_Pnt&   _P)
  : UV(_UV), W1(_W1), W2(_W2), T(_T), P(_P), Membership(Membership_Unknown)
  {}

public:

  //! Comparator.
  //! \param pt [in] point to compare with.
  //! \return true/false.
  inline bool operator>(const geom_intersection_point_cc_2D& pt) const
  {
    return this->W1 > pt.W1;
  }

  //! Equality comparator.
  //! \param pt [in] point to compare with.
  //! \return true/false.
  inline bool operator==(const geom_intersection_point_cc_2D& pt) const
  {
    return this->W1 == pt.W1;
  }

public:

  gp_Pnt                    P;            //!< Optional point in 3D.
  gp_Pnt2d                  UV;           //!< Intersection point in the surface domain.
  double                    W1;           //!< Intersection parameter on the first curve.
  double                    W2;           //!< Intersection parameter on the second curve.
  gp_Dir2d                  T;            //!< Tangency of the probe curve at the intersection point.
  Handle(geom_epsilon_base) Neighborhood; //!< Optional neighborhood information.
  geom_membership           Membership;   //!< Membership associated with the point.

};

//! Collection of intersection points.
typedef NCollection_Sequence<geom_intersection_point_cc_2D> geom_intersection_points_cc_2D;

#endif
