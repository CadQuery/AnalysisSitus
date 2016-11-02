//-----------------------------------------------------------------------------
// Created on: 19 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_ContourPointsDataProvider_h
#define asiVisu_ContourPointsDataProvider_h

// A-Situs visualization includes
#include <asiVisu_PointsDataProvider.h>

// asiData includes
#include <asiData_ContourNode.h>

DEFINE_STANDARD_HANDLE(asiVisu_ContourPointsDataProvider, asiVisu_PointsDataProvider)

//! Data provider for base points of a contour.
class asiVisu_ContourPointsDataProvider : public asiVisu_PointsDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_ContourPointsDataProvider, asiVisu_PointsDataProvider)

public:

  asiVisu_EXPORT
    asiVisu_ContourPointsDataProvider(const Handle(asiData_ContourNode)& contour);

public:

  asiVisu_EXPORT virtual Handle(asiAlgo_PointCloud)
    GetPoints() const;

private:

  asiVisu_EXPORT virtual Handle(ActAPI_HParameterList)
    translationSources() const;

};

#endif
