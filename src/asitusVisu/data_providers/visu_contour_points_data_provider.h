//-----------------------------------------------------------------------------
// Created on: 19 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_contour_points_data_provider_h
#define visu_contour_points_data_provider_h

// A-Situs visualization includes
#include <visu_points_data_provider.h>

DEFINE_STANDARD_HANDLE(visu_contour_points_data_provider, visu_points_data_provider)

//! Data provider for base points of a contour.
class visu_contour_points_data_provider : public visu_points_data_provider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_contour_points_data_provider, visu_points_data_provider)

public:

  visu_contour_points_data_provider();

public:

  virtual Handle(geom_point_cloud)
    GetPoints() const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

};

#endif
