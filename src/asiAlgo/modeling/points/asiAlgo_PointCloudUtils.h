//-----------------------------------------------------------------------------
// Created on: 02 December 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiAlgo_PointCloudUtils_h
#define asiAlgo_PointCloudUtils_h

// A-Situs includes
#include <asiAlgo_PointCloud.h>

//-----------------------------------------------------------------------------

//! Point cloud processing utilities.
namespace asiAlgo_PointCloudUtils
{
  asiAlgo_EXPORT Handle(TColStd_HArray1OfReal)
    AsRealArray(const Handle(asiAlgo_PointCloud<double>)& pointCloud);

  asiAlgo_EXPORT Handle(TColStd_HArray1OfReal)
    AsRealArray(const Handle(asiAlgo_PointCloud<float>)& pointCloud);

  asiAlgo_EXPORT Handle(asiAlgo_PointCloud<double>)
    AsPointCloud(const Handle(TColStd_HArray1OfReal)& arr);

  asiAlgo_EXPORT Handle(asiAlgo_PointCloud<float>)
    AsPointCloudf(const Handle(TColStd_HArray1OfReal)& arr);
};

#endif
