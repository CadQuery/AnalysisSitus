//-----------------------------------------------------------------------------
// Created on: 01 March 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_RESegmentDataProvider_h
#define asiVisu_RESegmentDataProvider_h

// A-Situs visualization includes
#include <asiVisu_PointsDataProvider.h>

// asiData includes
#include <asiData_RESegmentNode.h>

DEFINE_STANDARD_HANDLE(asiVisu_RESegmentDataProvider, asiVisu_PointsDataProvider)

//! Data provider for a segment of re-engineering point cloud.
class asiVisu_RESegmentDataProvider : public asiVisu_PointsDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_RESegmentDataProvider, asiVisu_PointsDataProvider)

public:

  asiVisu_EXPORT
    asiVisu_RESegmentDataProvider(const Handle(asiData_RESegmentNode)& segment_n);

public:

  asiVisu_EXPORT virtual Handle(asiAlgo_BaseCloud<double>)
    GetPoints() const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

};

#endif
