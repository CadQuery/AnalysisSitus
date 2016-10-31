//-----------------------------------------------------------------------------
// Created on: 22 June 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiAlgo_PointCloudNormals_h
#define asiAlgo_PointCloudNormals_h

// A-Situs includes
#include <asiAlgo.h>

// Geometry includes
#include <asiAlgo_PointCloud.h>

//! Functions to deal with point cloud normals.
namespace asiAlgo_PointCloudNormals
{
  asiAlgo_EXPORT void
    Calculate(const Handle(asiAlgo_PointCloud)& points);
};

#endif
