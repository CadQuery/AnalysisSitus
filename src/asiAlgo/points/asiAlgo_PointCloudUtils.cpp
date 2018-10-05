//-----------------------------------------------------------------------------
// Created on: 02 December 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of Sergey Slyadnev nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
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
  asiAlgo_PointCloudUtils::AsCloudd(const Handle(TColStd_HArray1OfReal)& arr)
{
  Handle(asiAlgo_BaseCloud<double>) result = new asiAlgo_BaseCloud<double>;
  //
  for ( int i = arr->Lower(); i <= arr->Upper(); ++i )
    result->ChangeCoords().push_back( arr->Value(i) );

  return result;
}

//-----------------------------------------------------------------------------

Handle(asiAlgo_BaseCloud<float>)
  asiAlgo_PointCloudUtils::AsCloudf(const Handle(TColStd_HArray1OfReal)& arr)
{
  Handle(asiAlgo_BaseCloud<float>) result = new asiAlgo_BaseCloud<float>;
  //
  for ( int i = arr->Lower(); i <= arr->Upper(); ++i )
    result->ChangeCoords().push_back( (float) ( arr->Value(i) ) );

  return result;
}

//-----------------------------------------------------------------------------

Handle(asiAlgo_BaseCloud<double>)
  asiAlgo_PointCloudUtils::CloudfAsCloudd(const Handle(asiAlgo_BaseCloud<float>)& pointCloud)
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
