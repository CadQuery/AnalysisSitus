//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_intersection_curve_ss_h
#define geom_intersection_curve_ss_h

// Analysis Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_Sequence.hxx>

DEFINE_STANDARD_HANDLE(geom_intersection_curve_ss, Standard_Transient)

//! Curve representing 1-dimensional intersection between two surfaces. The
//! first intersected surface corresponds to the parameters with index 1.
class geom_intersection_curve_ss : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_intersection_curve_ss, Standard_Transient)

public:

  //! Default constructor.
  inline geom_intersection_curve_ss()
  : Uncertainty(0.0)
  {}

  //! Constructor.
  //! \param _U  [in] uncertainty of the intersection method.
  //! \param _C  [in] intersection result as a parametric curve.
  //! \param _S1 [in] first intersected surface.
  //! \param _S2 [in] second intersected surface.
  inline geom_intersection_curve_ss(const double                _U,
                                    const Handle(Geom_Curve)&   _C,
                                    const Handle(Geom_Surface)& _S1,
                                    const Handle(Geom_Surface)& _S2)
  : Uncertainty (_U),
    C           (_C),
    S1          (_S1),
    S2          (_S2)
  {}

public:

  double               Uncertainty; //!< Precision of the numerical method.
  Handle(Geom_Curve)   C;           //!< Parametric curve itself.
  Handle(Geom_Surface) S1;          //!< First intersected surface.
  Handle(Geom_Surface) S2;          //!< Second intersected surface.

};

//! Collection of intersection curves.
typedef NCollection_Sequence<Handle(geom_intersection_curve_ss)> geom_intersection_curves_ss;

#endif
