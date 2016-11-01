//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_IVPointSetDataProvider_h
#define asiVisu_IVPointSetDataProvider_h

// Visualization includes
#include <asiData_IVPointSetNode.h>
#include <asiVisu_PointsDataProvider.h>

DEFINE_STANDARD_HANDLE(asiVisu_IVPointSetDataProvider, asiVisu_PointsDataProvider)

//! Data provider for a point cloud represented in IV.
class asiVisu_IVPointSetDataProvider : public asiVisu_PointsDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_IVPointSetDataProvider, asiVisu_PointsDataProvider)

public:

  asiVisu_IVPointSetDataProvider(const Handle(ActAPI_INode)& pointSet);

public:

  virtual Handle(asiAlgo_PointCloud)
    GetPoints() const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

};

#endif
