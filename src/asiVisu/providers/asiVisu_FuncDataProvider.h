//-----------------------------------------------------------------------------
// Created on: 28 April 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_FuncDataProvider_h
#define asiVisu_FuncDataProvider_h

// asiVisu includes
#include <asiVisu_DataProvider.h>

// asiAlgo includes
#include <asiAlgo_Function.h>

//! Data provider for an abstract function.
template <typename T_VARIABLE>
class asiVisu_FuncDataProvider : public asiVisu_DataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_FuncDataProvider, asiVisu_DataProvider)

public:

  virtual Handle(asiAlgo_Function<T_VARIABLE>)
    GetFunction() const = 0;

  virtual T_VARIABLE
    GetMinCorner() const = 0;

  virtual T_VARIABLE
    GetMaxCorner() const = 0;

};

#endif
