//-----------------------------------------------------------------------------
// Created on: 25 April 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_REGaussMapDataProvider_h
#define asiVisu_REGaussMapDataProvider_h

// asiVisu includes
#include <asiVisu_VectorsDataProvider.h>

// asiData includes
#include <asiData_REGaussMapNode.h>

DEFINE_STANDARD_HANDLE(asiVisu_REGaussMapDataProvider, asiVisu_VectorsDataProvider)

//! Data provider for re-engineering Gauss map.
class asiVisu_REGaussMapDataProvider : public asiVisu_VectorsDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_REGaussMapDataProvider, asiVisu_VectorsDataProvider)

public:

  asiVisu_EXPORT
    asiVisu_REGaussMapDataProvider(const Handle(asiData_REGaussMapNode)& gaussNode);

public:

  asiVisu_EXPORT virtual Handle(asiAlgo_BaseCloud<float>)
    GetPointsf();

  asiVisu_EXPORT virtual Handle(asiAlgo_BaseCloud<float>)
    GetVectorsf();

  asiVisu_EXPORT virtual double
    GetMaxVectorModulus() const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

};

#endif
