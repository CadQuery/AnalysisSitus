//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiAlgo_IntersectionCurveSS_h
#define asiAlgo_IntersectionCurveSS_h

// Analysis Situs includes
#include <asiAlgo.h>

// OCCT includes
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_Sequence.hxx>

DEFINE_STANDARD_HANDLE(asiAlgo_IntersectionCurveSS, Standard_Transient)

//! Curve representing 1-dimensional intersection between two surfaces. The
//! first intersected surface corresponds to the parameters with index 1.
class asiAlgo_IntersectionCurveSS : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_IntersectionCurveSS, Standard_Transient)

public:

  //! Default constructor.
  asiAlgo_IntersectionCurveSS() : Uncertainty(0.0)
  {}

  //! Constructor.
  //! \param _U  [in] uncertainty of the intersection method.
  //! \param _C  [in] intersection result as a parametric curve.
  //! \param _S1 [in] first intersected surface.
  //! \param _S2 [in] second intersected surface.
  asiAlgo_IntersectionCurveSS(const double                _U,
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
typedef NCollection_Sequence<Handle(asiAlgo_IntersectionCurveSS)> asiAlgo_IntersectionCurvesSS;

#endif
