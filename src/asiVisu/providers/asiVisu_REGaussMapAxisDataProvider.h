//-----------------------------------------------------------------------------
// Created on: 27 April 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_REGaussMapAxisDataProvider_h
#define asiVisu_REGaussMapAxisDataProvider_h

// asiVisu includes
#include <asiVisu_VectorsDataProvider.h>

// asiData includes
#include <asiData_REGaussMapNode.h>

DEFINE_STANDARD_HANDLE(asiVisu_REGaussMapAxisDataProvider, asiVisu_VectorsDataProvider)

//! Data provider for axis vector stored in re-engineering Gauss map.
class asiVisu_REGaussMapAxisDataProvider : public asiVisu_VectorsDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_REGaussMapAxisDataProvider, asiVisu_VectorsDataProvider)

public:

  asiVisu_EXPORT
    asiVisu_REGaussMapAxisDataProvider(const Handle(asiData_REGaussMapNode)& gaussNode);

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
