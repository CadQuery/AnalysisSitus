//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_intersection_point_cc_h
#define geom_intersection_point_cc_h

// Analysis Situs includes
#include <analysis_situs.h>

// Geometry includes
#include <geom_intersection_curve_ss.h>

// OCCT includes
#include <gp_Pnt.hxx>

DEFINE_STANDARD_HANDLE(geom_intersection_point_cc, Standard_Transient)

//! Point representing 0-dimensional intersection between two curves in 3D. The
//! first intersected curve corresponds to the parameters with index 1, e.g. W1.
class geom_intersection_point_cc : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_intersection_point_cc, Standard_Transient)

public:

  //! Default constructor.
  inline geom_intersection_point_cc()
  : Uncertainty (0.0),
    W1          (0.0),
    W2          (0.0)
  {}

  //! Constructor.
  //! \param _P [in] three-dimensional point.
  inline geom_intersection_point_cc(const gp_Pnt& _P)
  : P           (_P),
    W1          (0.0),
    W2          (0.0),
    Uncertainty (0.0)
  {}

  //! Constructor.
  //! \param _P  [in] three-dimensional point.
  //! \param _W1 [in] intersection parameter on the first curve.
  //! \param _W2 [in] intersection parameter on the second curve.
  inline geom_intersection_point_cc(const gp_Pnt& _P,
                                    const double  _W1,
                                    const double  _W2)
  : P           (_P),
    Uncertainty (0.0),
    W1          (_W1),
    W2          (_W2)
  {}

  //! Constructor.
  //! \param _P  [in] three-dimensional point.
  //! \param _W1 [in] intersection parameter on the first curve.
  //! \param _W2 [in] intersection parameter on the second curve.
  //! \param _T1 [in] tangency of the first curve at the intersection point.
  //! \param _T2 [in] tangency of the second curve at the intersection point.
  inline geom_intersection_point_cc(const gp_Pnt& _P,
                                    const double  _W1,
                                    const double  _W2,
                                    const gp_Dir& _T1,
                                    const gp_Dir& _T2)
  : P           (_P),
    Uncertainty (0.0),
    W1          (_W1),
    W2          (_W2),
    T1          (_T1),
    T2          (_T2)
  {}

  //! Constructor.
  //! \param _P  [in] three-dimensional point.
  //! \param _U  [in] uncertainty.
  //! \param _W1 [in] intersection parameter on the first curve.
  //! \param _W2 [in] intersection parameter on the second curve.
  //! \param _T1 [in] tangency of the first curve at the intersection point.
  //! \param _T2 [in] tangency of the second curve at the intersection point.
  inline geom_intersection_point_cc(const gp_Pnt& _P,
                                    const double  _U,
                                    const double  _W1,
                                    const double  _W2,
                                    const gp_Dir& _T1,
                                    const gp_Dir& _T2)
  : P           (_P),
    Uncertainty (_U),
    W1          (_W1),
    W2          (_W2),
    T1          (_T1),
    T2          (_T2)
  {}

  //! Constructor.
  //! \param _P   [in] three-dimensional point.
  //! \param _U   [in] uncertainty.
  //! \param _W1  [in] intersection parameter on the first curve.
  //! \param _W2  [in] intersection parameter on the second curve.
  //! \param _T1  [in] tangency of the first curve at the intersection point.
  //! \param _T2  [in] tangency of the second curve at the intersection point.
  //! \param _pC1 [in] first intersected curve.
  //! \param _pC2 [in] second intersected curve.
  inline geom_intersection_point_cc(const gp_Pnt&                             _P,
                                    const double                              _U,
                                    const double                              _W1,
                                    const double                              _W2,
                                    const gp_Dir&                             _T1,
                                    const gp_Dir&                             _T2,
                                    const Handle(geom_intersection_curve_ss)& _pC1,
                                    const Handle(geom_intersection_curve_ss)& _pC2)
  : P           (_P),
    Uncertainty (_U),
    W1          (_W1),
    W2          (_W2),
    T1          (_T1),
    T2          (_T2),
    pC1         (_pC1),
    pC2         (_pC2)
  {}

public:

  //! Comparator.
  //! \param pt [in] point to compare with.
  //! \return true/false.
  inline bool operator>(const geom_intersection_point_cc& pt) const
  {
    return this->W1 > pt.W1;
  }

  //! Equality comparator.
  //! \param pt [in] point to compare with.
  //! \return true/false.
  inline bool operator==(const geom_intersection_point_cc& pt) const
  {
    return this->W1 == pt.W1;
  }

public:

  gp_Pnt P;           //!< Intersection point in 3D.
  double Uncertainty; //!< Uncertainty in 3D point position.
  double W1;          //!< Intersection parameter on the first curve.
  double W2;          //!< Intersection parameter on the second curve.
  gp_Dir T1;          //!< Tangent direction of the first curve at the intersection point.
  gp_Dir T2;          //!< Tangent direction of the second curve at the intersection point.
  //
  Handle(geom_intersection_curve_ss) pC1; //!< First intersected curve.
  Handle(geom_intersection_curve_ss) pC2; //!< Second intersected curve.

};

//! Collection of intersection points.
typedef NCollection_Sequence<Handle(geom_intersection_point_cc)> geom_intersection_points_cc;

#endif
