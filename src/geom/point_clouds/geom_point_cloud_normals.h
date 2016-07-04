//-----------------------------------------------------------------------------
// Created on: 22 June 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_point_cloud_normals_h
#define geom_point_cloud_normals_h

// A-Situs includes
#include <analysis_situs.h>

// Geometry includes
#include <geom_point_cloud.h>

//! Functions to deal with point cloud normals.
namespace geom_point_cloud_normals
{
  void Calculate(const Handle(geom_point_cloud)& points);
};

#endif
