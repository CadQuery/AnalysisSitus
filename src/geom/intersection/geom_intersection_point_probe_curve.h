//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_intersection_point_probe_curve_h
#define geom_intersection_point_probe_curve_h

// Analysis Situs includes
#include <analysis_situs.h>

// Geometry includes
#include <geom_epsilon_base.h>
#include <geom_membership.h>

// OCCT includes
#include <gp_Pnt2d.hxx>
#include <NCollection_Sequence.hxx>

//! Point representing intersection between two curves in 3D. Probe is
//! emphasized here as by convention its parameters are stored in W1
//! property.
struct geom_intersection_point_probe_curve
{
public:

  //! Default constructor.
  inline geom_intersection_point_probe_curve()
  : Uncertainty(0.0), W1(0.0), W2(0.0), Membership(Membership_Unknown)
  {}

  //! Constructor.
  //! \param _P [in] three-dimensional point.
  inline geom_intersection_point_probe_curve(const gp_Pnt& _P)
  : P(_P), Uncertainty(0.0), Membership(Membership_Unknown)
  {}

  //! Constructor.
  //! \param _P  [in] three-dimensional point.
  //! \param _W1 [in] intersection parameter on the first curve.
  //! \param _W2 [in] intersection parameter on the second curve.
  inline geom_intersection_point_probe_curve(const gp_Pnt& _P,
                                             const double  _W1,
                                             const double  _W2)
  : P(_P), Uncertainty(0.0), W1(_W1), W2(_W2), Membership(Membership_Unknown)
  {}

  //! Constructor.
  //! \param _P  [in] three-dimensional point.
  //! \param _W1 [in] intersection parameter on the first curve.
  //! \param _W2 [in] intersection parameter on the second curve.
  //! \param _T  [in] tangency of the probe curve at the intersection point.
  inline geom_intersection_point_probe_curve(const gp_Pnt& _P,
                                             const double  _W1,
                                             const double  _W2,
                                             const gp_Dir& _T)
  : P(_P), Uncertainty(0.0), W1(_W1), W2(_W2), T(_T), Membership(Membership_Unknown)
  {}

  //! Constructor.
  //! \param _P  [in] three-dimensional point.
  //! \param _U  [in] uncertainty. 
  //! \param _W1 [in] intersection parameter on the first curve.
  //! \param _W2 [in] intersection parameter on the second curve.
  //! \param _T  [in] tangency of the probe curve at the intersection point.
  inline geom_intersection_point_probe_curve(const gp_Pnt& _P,
                                             const double  _U,
                                             const double  _W1,
                                             const double  _W2,
                                             const gp_Dir& _T)
  : P(_P), Uncertainty(_U), W1(_W1), W2(_W2), T(_T), Membership(Membership_Unknown)
  {}

  //! Constructor.
  //! \param _P   [in] three-dimensional point.
  //! \param _U   [in] uncertainty. 
  //! \param _W1  [in] intersection parameter on the first curve.
  //! \param _W2  [in] intersection parameter on the second curve.
  //! \param _T   [in] tangency of the probe curve at the intersection point.
  //! \param _N   [in] neighborhood.
  //! \param _Mbs [in] membership.
  inline geom_intersection_point_probe_curve(const gp_Pnt&                    _P,
                                             const double                     _U,
                                             const double                     _W1,
                                             const double                     _W2,
                                             const gp_Dir&                    _T,
                                             const Handle(geom_epsilon_base)& _N,
                                             const geom_membership            _Mbs)
  : P(_P), Uncertainty(_U), W1(_W1), W2(_W2), T(_T), Neighborhood(_N), Membership(_Mbs)
  {}

public:

  //! Comparator.
  //! \param pt [in] point to compare with.
  //! \return true/false.
  inline bool operator>(const geom_intersection_point_probe_curve& pt) const
  {
    return this->W1 > pt.W1;
  }

  //! Equality comparator.
  //! \param pt [in] point to compare with.
  //! \return true/false.
  inline bool operator==(const geom_intersection_point_probe_curve& pt) const
  {
    return this->W1 == pt.W1;
  }

public:

  gp_Pnt                    P;            //!< Intersection point in 3D.
  double                    Uncertainty;  //!< Uncertainty in 3D point position.
  double                    W1;           //!< Intersection parameter on the first curve.
  double                    W2;           //!< Intersection parameter on the second curve.
  gp_Dir                    T;            //!< Tangency of the probe curve at the intersection point.
  Handle(geom_epsilon_base) Neighborhood; //!< Optional neighborhood information.
  geom_membership           Membership;   //!< Membership associated with the point.

};

//! Collection of intersection points.
typedef NCollection_Sequence<geom_intersection_point_probe_curve> geom_intersection_points_probe_curve;

#endif
