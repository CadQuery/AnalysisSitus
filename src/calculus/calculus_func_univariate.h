//-----------------------------------------------------------------------------
// Created on: 04 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef calculus_func_univariate_h
#define calculus_func_univariate_h

// A-Situs includes
#include <analysis_situs.h>

DEFINE_STANDARD_HANDLE(calculus_func_univariate, Standard_Transient)

//! Interface for univariate function f(x) in explicit form.
class calculus_func_univariate : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(calculus_func_univariate, Standard_Transient)

public:

  virtual double operator() (const double x)       = 0;
  virtual double Min_X      ()               const = 0;
  virtual double Max_X      ()               const = 0;

};

#endif
