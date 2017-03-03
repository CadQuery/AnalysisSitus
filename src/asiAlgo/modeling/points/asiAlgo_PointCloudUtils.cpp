//-----------------------------------------------------------------------------
// Created on: 02 December 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
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
