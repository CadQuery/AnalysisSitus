//-----------------------------------------------------------------------------
// Created on: 18 April 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_Function_h
#define asiAlgo_Function_h

// asiAlgo includes
#include <asiAlgo.h>

// OCCT includes
#include <Standard_Transient.hxx>

//! Target function to optimize.
template <typename T_DIMENSION>
class asiAlgo_Function : public Standard_Transient
{
public:

  typedef T_DIMENSION t_argument;

public:

  virtual double Value(const t_argument& x) = 0;

};

#endif
