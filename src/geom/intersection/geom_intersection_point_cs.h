//-----------------------------------------------------------------------------
// Created on: 19 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_intersection_point_cs_h
#define geom_intersection_point_cs_h

// Analysis Situs includes
#include <analysis_situs.h>

DEFINE_STANDARD_HANDLE(geom_intersection_point_cs, Standard_Transient)

//! Point representing 0-dimensional intersection between a curve
//! and a surface.
class geom_intersection_point_cs : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_intersection_point_cs, Standard_Transient)

public:

  //! Default constructor.
  inline geom_intersection_point_cs()
  : Uncertainty (0.0),
    W           (0.0)
  {}

  //! Constructor.
  //! \param _P [in] three-dimensional point.
  inline geom_intersection_point_cs(const gp_Pnt& _P)
  : P           (_P),
    Uncertainty (0.0),
    W           (0.0)
  {}

  //! Constructor.
  //! \param _P  [in] three-dimensional point.
  //! \param _UV [in] intersection parameters on the surface.
  //! \param _W  [in] intersection parameter on the curve.
  inline geom_intersection_point_cs(const gp_Pnt&   _P,
                                    const gp_Pnt2d& _UV,
                                    const double    _W)
  : P           (_P),
    Uncertainty (0.0),
    UV          (_UV),
    W           (_W)
  {}

  //! Constructor.
  //! \param _P  [in] three-dimensional point.
  //! \param _T  [in] tangency vector at the intersection point.
  //! \param _UV [in] intersection parameters on the surface.
  //! \param _W  [in] intersection parameter on the curve.
  inline geom_intersection_point_cs(const gp_Pnt&   _P,
                                    const gp_Dir&   _T,
                                    const gp_Pnt2d& _UV,
                                    const double    _W)
  : P           (_P),
    T           (_T),
    Uncertainty (0.0),
    UV          (_UV),
    W           (_W)
  {}

  //! Constructor.
  //! \param _P   [in] three-dimensional point.
  //! \param _T   [in] tangency vector at the intersection point.
  //! \param _Eps [in] uncertainty.
  //! \param _UV  [in] intersection parameters on the surface.
  //! \param _W   [in] intersection parameter on the curve.
  inline geom_intersection_point_cs(const gp_Pnt&   _P,
                                    const gp_Dir&   _T,
                                    const double    _Eps,
                                    const gp_Pnt2d& _UV,
                                    const double    _W)
  : P           (_P),
    T           (_T),
    Uncertainty (_Eps),
    UV          (_UV),
    W           (_W)
  {}

public:

  //! Comparator.
  //! \param pt [in] point to compare with.
  //! \return true/false.
  inline bool operator>(const geom_intersection_point_cs& pt) const
  {
    return this->W > pt.W;
  }

  //! Equality comparator.
  //! \param pt [in] point to compare with.
  //! \return true/false.
  inline bool operator==(const geom_intersection_point_cs& pt) const
  {
    return this->W == pt.W;
  }

public:

  gp_Pnt   P;           //!< Intersection point in 3D.
  gp_Dir   T;           //!< Tangent direction of the curve at the intersection point.
  double   Uncertainty; //!< Uncertainty in 3D point position.
  gp_Pnt2d UV;          //!< Intersection parameters on the surface.
  double   W;           //!< Intersection parameter on the curve.

};

//! Collection of intersection points.
typedef NCollection_Sequence<Handle(geom_intersection_point_cs)> geom_intersection_points_cs;

#endif
