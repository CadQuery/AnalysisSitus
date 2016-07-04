//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_re_points_data_provider_h
#define visu_re_points_data_provider_h

// A-Situs visualization includes
#include <visu_points_data_provider.h>

DEFINE_STANDARD_HANDLE(visu_re_points_data_provider, visu_points_data_provider)

//! Data provider for re-engineering point cloud.
class visu_re_points_data_provider : public visu_points_data_provider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_re_points_data_provider, visu_points_data_provider)

public:

  visu_re_points_data_provider();

public:

  virtual Handle(geom_point_cloud)
    GetPoints() const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

};

#endif
