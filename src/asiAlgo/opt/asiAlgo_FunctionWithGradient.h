//-----------------------------------------------------------------------------
// Created on: 24 April 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_FunctionWithGradient_h
#define asiAlgo_FunctionWithGradient_h

// asiAlgo includes
#include <asiAlgo_Function.h>

//! Base class for objective functions to optimize with gradient-wise methods.
template <typename T_DIMENSION>
class asiAlgo_FunctionWithGradient : public asiAlgo_Function<T_DIMENSION>
{
public:

  typedef T_DIMENSION t_coord;

public:

  virtual t_coord Gradient(const t_coord& x) = 0;

};

#endif
