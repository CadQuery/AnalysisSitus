//-----------------------------------------------------------------------------
// Created on: 26 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_point_cloud_h
#define geom_point_cloud_h

// A-Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <Precision.hxx>
#include <TCollection_AsciiString.hxx>

//! Interoperability tool for point clouds.
class geom_point_cloud
{
public:

  static bool ReadSections(const TCollection_AsciiString& filename,
                           TopoDS_Shape&                  result);

  static bool ReadScattered(const TCollection_AsciiString& filename,
                            TopoDS_Shape&                  result);

private:

  geom_point_cloud() {}
  ~geom_point_cloud() {}

};

#endif
