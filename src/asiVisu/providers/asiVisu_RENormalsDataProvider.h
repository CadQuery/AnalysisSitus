//-----------------------------------------------------------------------------
// Created on: 08 December 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_RENormalsDataProvider_h
#define asiVisu_RENormalsDataProvider_h

// asiVisu includes
#include <asiVisu_NormalsDataProvider.h>

// asiData includes
#include <asiData_RENormalsNode.h>

DEFINE_STANDARD_HANDLE(asiVisu_RENormalsDataProvider, asiVisu_NormalsDataProvider)

//! Data provider for re-engineering normals.
class asiVisu_RENormalsDataProvider : public asiVisu_NormalsDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_RENormalsDataProvider, asiVisu_NormalsDataProvider)

public:

  asiVisu_EXPORT
    asiVisu_RENormalsDataProvider(const Handle(asiData_RENormalsNode)& normalsNode);

public:

  asiVisu_EXPORT virtual Handle(asiAlgo_PointCloud<float>)
    GetPointsf() const;

  asiVisu_EXPORT virtual Handle(asiAlgo_PointCloud<float>)
    GetNormalsf() const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

};

#endif
