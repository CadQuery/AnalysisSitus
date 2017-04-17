//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_PointsDataProvider_h
#define asiVisu_PointsDataProvider_h

// asiVisu includes
#include <asiVisu_DataProvider.h>

// asiAlgo includes
#include <asiAlgo_BaseCloud.h>

// OCCT includes
#include <TColStd_HPackedMapOfInteger.hxx>

DEFINE_STANDARD_HANDLE(asiVisu_PointsDataProvider, asiVisu_DataProvider)

//! Data provider for a single point cloud.
class asiVisu_PointsDataProvider : public asiVisu_DataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_PointsDataProvider, asiVisu_DataProvider)

public:

  asiVisu_EXPORT
    asiVisu_PointsDataProvider(const Handle(ActAPI_INode)& N);

public:

  asiVisu_EXPORT virtual ActAPI_DataObjectId
    GetNodeID() const;

public:

  virtual Handle(asiAlgo_BaseCloud<double>)
    GetPoints() const = 0;

  virtual Handle(TColStd_HPackedMapOfInteger)
    GetIndices() const = 0;

protected:

  Handle(ActAPI_INode) m_node; //!< Source Node.

};

#endif
