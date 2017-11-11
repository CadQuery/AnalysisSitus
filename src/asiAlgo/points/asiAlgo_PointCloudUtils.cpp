//-----------------------------------------------------------------------------
// Created on: 02 December 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_PointCloudUtils.h>

//-----------------------------------------------------------------------------

Handle(TColStd_HArray1OfReal)
  asiAlgo_PointCloudUtils::AsRealArray(const Handle(asiAlgo_BaseCloud<double>)& pointCloud)
{
  if ( pointCloud.IsNull() || pointCloud->IsEmpty() )
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
  if ( pointCloud.IsNull() || pointCloud->IsEmpty() )
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
