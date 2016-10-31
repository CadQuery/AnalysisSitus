//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_points_data_provider_h
#define visu_points_data_provider_h

// A-Situs visualization includes
#include <visu_data_provider.h>

// Geometry includes
#include <asiAlgo_PointCloud.h>

DEFINE_STANDARD_HANDLE(visu_points_data_provider, visu_data_provider)

//! Data provider for a single point cloud.
class visu_points_data_provider : public visu_data_provider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_points_data_provider, visu_data_provider)

public:

  visu_points_data_provider(const Handle(ActAPI_INode)& N);

public:

  virtual ActAPI_DataObjectId GetNodeID() const;

public:

  virtual Handle(asiAlgo_PointCloud)
    GetPoints() const = 0;

protected:

  Handle(ActAPI_INode) m_node; //!< Source Node.

};

#endif
