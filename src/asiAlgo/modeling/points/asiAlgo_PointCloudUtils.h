//-----------------------------------------------------------------------------
// Created on: 02 December 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_PointCloudUtils_h
#define asiAlgo_PointCloudUtils_h

// A-Situs includes
#include <asiAlgo_BaseCloud.h>

//-----------------------------------------------------------------------------

//! Point cloud processing utilities.
namespace asiAlgo_PointCloudUtils
{
  asiAlgo_EXPORT Handle(TColStd_HArray1OfReal)
    AsRealArray(const Handle(asiAlgo_BaseCloud<double>)& pointCloud);

  asiAlgo_EXPORT Handle(TColStd_HArray1OfReal)
    AsRealArray(const Handle(asiAlgo_BaseCloud<float>)& pointCloud);

  asiAlgo_EXPORT Handle(asiAlgo_BaseCloud<double>)
    AsPointCloud(const Handle(TColStd_HArray1OfReal)& arr);

  asiAlgo_EXPORT Handle(asiAlgo_BaseCloud<float>)
    AsPointCloudf(const Handle(TColStd_HArray1OfReal)& arr);
};

#endif
