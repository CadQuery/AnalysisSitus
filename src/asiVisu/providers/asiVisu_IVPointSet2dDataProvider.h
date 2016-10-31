//-----------------------------------------------------------------------------
// Created on: 16 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_iv_point_set_2d_data_provider_h
#define visu_iv_point_set_2d_data_provider_h

// Visualization includes
#include <asiData_IVPointSet2dNode.h>
#include <visu_points_data_provider.h>

DEFINE_STANDARD_HANDLE(visu_iv_point_set_2d_data_provider, visu_points_data_provider)

//! Data provider for a 2D point cloud represented in IV.
class visu_iv_point_set_2d_data_provider : public visu_points_data_provider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_iv_point_set_2d_data_provider, visu_points_data_provider)

public:

  visu_iv_point_set_2d_data_provider(const Handle(ActAPI_INode)& pointSet);

public:

  virtual Handle(asiAlgo_PointCloud)
    GetPoints() const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

};

#endif
