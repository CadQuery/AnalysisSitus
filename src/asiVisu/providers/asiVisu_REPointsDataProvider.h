//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_REPointsDataProvider_h
#define asiVisu_REPointsDataProvider_h

// A-Situs visualization includes
#include <asiVisu_PointsDataProvider.h>

// asiData includes
#include <asiData_REPointsNode.h>

DEFINE_STANDARD_HANDLE(asiVisu_REPointsDataProvider, asiVisu_PointsDataProvider)

//! Data provider for re-engineering point cloud.
class asiVisu_REPointsDataProvider : public asiVisu_PointsDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_REPointsDataProvider, asiVisu_PointsDataProvider)

public:

  asiVisu_EXPORT
    asiVisu_REPointsDataProvider(const Handle(asiData_REPointsNode)& points_n);

public:

  asiVisu_EXPORT virtual Handle(asiAlgo_PointCloud<double>)
    GetPoints() const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

};

#endif
