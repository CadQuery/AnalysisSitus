//-----------------------------------------------------------------------------
// Created on: 25 May 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_FuncUnivariateDataProvider_h
#define asiVisu_FuncUnivariateDataProvider_h

// asiVisu includes
#include <asiVisu_DataProvider.h>

// asiAlgo includes
#include <asiAlgo_FuncUnivariate.h>

//! Data provider for explicitly defined univariate function.
class asiVisu_FuncUnivariateDataProvider : public asiVisu_DataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_FuncUnivariateDataProvider, asiVisu_DataProvider)

public:

  asiVisu_EXPORT virtual Handle(asiAlgo_FuncUnivariate)
    GetFunc() const = 0;

  asiVisu_EXPORT virtual ActAPI_DataObjectId
    GetNodeID() const = 0;

};

#endif
