//-----------------------------------------------------------------------------
// Created on: 16 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_IVPointSet2dDataProvider_h
#define asiVisu_IVPointSet2dDataProvider_h

// Visualization includes
#include <asiData_IVPointSet2dNode.h>
#include <asiVisu_PointsDataProvider.h>

DEFINE_STANDARD_HANDLE(asiVisu_IVPointSet2dDataProvider, asiVisu_PointsDataProvider)

//! Data provider for a 2D point cloud represented in IV.
class asiVisu_IVPointSet2dDataProvider : public asiVisu_PointsDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_IVPointSet2dDataProvider, asiVisu_PointsDataProvider)

public:

  asiVisu_EXPORT
    asiVisu_IVPointSet2dDataProvider(const Handle(ActAPI_INode)& pointSet);

public:

  asiVisu_EXPORT virtual Handle(asiAlgo_PointCloud)
    GetPoints() const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

};

#endif
