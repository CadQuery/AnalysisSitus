//-----------------------------------------------------------------------------
// Created on: 19 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiData_ContourPointsDataProvider_h
#define asiData_ContourPointsDataProvider_h

// A-Situs visualization includes
#include <asiVisu_PointsDataProvider.h>

DEFINE_STANDARD_HANDLE(asiData_ContourPointsDataProvider, asiVisu_PointsDataProvider)

//! Data provider for base points of a contour.
class asiData_ContourPointsDataProvider : public asiVisu_PointsDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_ContourPointsDataProvider, asiVisu_PointsDataProvider)

public:

  asiData_ContourPointsDataProvider();

public:

  virtual Handle(asiAlgo_PointCloud)
    GetPoints() const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

};

#endif
