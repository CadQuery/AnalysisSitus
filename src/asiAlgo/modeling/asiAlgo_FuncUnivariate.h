//-----------------------------------------------------------------------------
// Created on: 04 February 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_FuncUnivariate_h
#define asiAlgo_FuncUnivariate_h

// A-Situs includes
#include <asiAlgo.h>

// OCCT includes
#include <Standard_Type.hxx>

DEFINE_STANDARD_HANDLE(asiAlgo_FuncUnivariate, Standard_Transient)

//! Interface for univariate function f(x) in explicit form.
class asiAlgo_FuncUnivariate : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_FuncUnivariate, Standard_Transient)

public:

  virtual double operator() (const double x)       = 0;
  virtual double Min_X      ()               const = 0;
  virtual double Max_X      ()               const = 0;

};

#endif
