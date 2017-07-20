//-----------------------------------------------------------------------------
// Created on: 02 December 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
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

  asiAlgo_EXPORT Handle(asiAlgo_BaseCloud<double>)
    PointCloudfAsPointCloudd(const Handle(asiAlgo_BaseCloud<float>)& pointCloud);
};

#endif
