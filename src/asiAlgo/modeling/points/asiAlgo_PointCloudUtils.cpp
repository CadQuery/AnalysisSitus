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

// Own include
#include <asiAlgo_PointCloudUtils.h>

//-----------------------------------------------------------------------------

Handle(TColStd_HArray1OfReal)
  asiAlgo_PointCloudUtils::AsRealArray(const Handle(asiAlgo_BaseCloud<double>)& pointCloud)
{
  if ( pointCloud.IsNull() )
    return NULL;

  Handle(TColStd_HArray1OfReal)
    result = new TColStd_HArray1OfReal(0, (int) pointCloud->ChangeCoords().size() - 1);
  //
  for ( int i = result->Lower(); i <= result->Upper(); ++i )
    result->ChangeValue(i) = pointCloud->ChangeCoords()[i];

  return result;
}

//-----------------------------------------------------------------------------

Handle(TColStd_HArray1OfReal)
  asiAlgo_PointCloudUtils::AsRealArray(const Handle(asiAlgo_BaseCloud<float>)& pointCloud)
{
  if ( pointCloud.IsNull() )
    return NULL;

  Handle(TColStd_HArray1OfReal)
    result = new TColStd_HArray1OfReal(0, (int) pointCloud->ChangeCoords().size() - 1);
  //
  for ( int i = result->Lower(); i <= result->Upper(); ++i )
    result->ChangeValue(i) = pointCloud->ChangeCoords()[i];

  return result;
}

//-----------------------------------------------------------------------------

Handle(asiAlgo_BaseCloud<double>)
  asiAlgo_PointCloudUtils::AsPointCloud(const Handle(TColStd_HArray1OfReal)& arr)
{
  Handle(asiAlgo_BaseCloud<double>) result = new asiAlgo_BaseCloud<double>;
  //
  for ( int i = arr->Lower(); i <= arr->Upper(); ++i )
    result->ChangeCoords().push_back( arr->Value(i) );

  return result;
}

//-----------------------------------------------------------------------------

Handle(asiAlgo_BaseCloud<float>)
  asiAlgo_PointCloudUtils::AsPointCloudf(const Handle(TColStd_HArray1OfReal)& arr)
{
  Handle(asiAlgo_BaseCloud<float>) result = new asiAlgo_BaseCloud<float>;
  //
  for ( int i = arr->Lower(); i <= arr->Upper(); ++i )
    result->ChangeCoords().push_back( (float) ( arr->Value(i) ) );

  return result;
}

//-----------------------------------------------------------------------------

Handle(asiAlgo_BaseCloud<double>)
  asiAlgo_PointCloudUtils::PointCloudfAsPointCloudd(const Handle(asiAlgo_BaseCloud<float>)& pointCloud)
{
  Handle(asiAlgo_BaseCloud<double>) result = new asiAlgo_BaseCloud<double>;
  //
  for ( int i = 0; i < pointCloud->GetNumberOfElements(); ++i )
  {
    float px, py, pz;
    pointCloud->GetElement(i, px, py, pz);

    // Add
    result->AddElement(px, py, pz);
  }

  return result;
}
