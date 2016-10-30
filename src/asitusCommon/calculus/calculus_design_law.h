//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef calculus_design_law_h
#define calculus_design_law_h

// A-Situs (calculus) includes
#include <calculus_func_univariate.h>

// OCCT includes
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(calculus_design_law, calculus_func_univariate)

//! Evolution law for a scalar design variable.
class calculus_design_law : public calculus_func_univariate
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(calculus_design_law, calculus_func_univariate)

public:

  calculus_design_law(const Handle(TColStd_HArray1OfReal)& poles,
                      const double                         last);

public:

  virtual double operator() (const double x);
  virtual double Min_X      ()               const;
  virtual double Max_X      ()               const;

public:

  inline Handle(TColStd_HArray1OfReal)&    ChangePoles() { return m_poles; }
  inline Handle(TColStd_HArray1OfReal)&    ChangeKnots() { return m_knots; }
  inline Handle(TColStd_HArray1OfInteger)& ChangeMults() { return m_mults; }

protected:

  Handle(TColStd_HArray1OfReal)    m_poles; //!< Poles.
  Handle(TColStd_HArray1OfReal)    m_knots; //!< Knots.
  Handle(TColStd_HArray1OfInteger) m_mults; //!< Multiplicities.

};

#endif
