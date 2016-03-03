//-----------------------------------------------------------------------------
// Created on: 26 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_point_cloud.h>

// Qr (geometry) includes
#include <QrGeom3D_SectionCloud.h>

// Qr (tools) includes
#include <QrTools_ReadXYZ.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Vertex.hxx>

//! Performs point cloud import.
//! \param filename [in]  file to read.
//! \param result   [out] retrieved shape.
//! \return true in case of success, false -- otherwise.
bool geom_point_cloud::ReadSections(const TCollection_AsciiString& filename,
                                    TopoDS_Shape&                  result)
{
  // Read points distributed by sections
  QrTools_ReadXYZ ReadXYZ;
  QrPtr<QrGeom3D_PointCloud> Cloud;
  if ( !ReadXYZ(filename.ToCString(), QrTools_ReadXYZ::Type_CoherentSections, Cloud) )
  {
    std::cout << "Cannot read sections from file" << std::endl;
    return false;
  }

  // Convert to sections cloud
  QrPtr<QrGeom3D_SectionCloud> SectionCloud = QrPtr<QrGeom3D_SectionCloud>::DownCast(Cloud);
  if ( SectionCloud.IsNull() )
  {
    std::cout << "Cannot treat point cloud as sections cloud" << std::endl;
    return false;
  }

  // Convert to topology
  BRep_Builder BB;
  TopoDS_Compound CloudShape;
  BB.MakeCompound(CloudShape);
  //
  for ( int s = 0; s < SectionCloud->NumberOfSections(); ++s )
  {
    QrPtr<QrGeom3D_PositionCloud> section = SectionCloud->SectionByIndex(s)->Pts;
    //
    TopoDS_Compound SectionShape;
    BB.MakeCompound(SectionShape);

    for ( int p = 0; p < section->NumberOfPoints(); ++p )
    {
      TopoDS_Vertex v = BRepBuilderAPI_MakeVertex( gp_Pnt( section->Point(p).X(),
                                                           section->Point(p).Y(),
                                                           section->Point(p).Z() ) );
      BB.Add(SectionShape, v);
    }
    BB.Add(CloudShape, SectionShape);
  }

  // Set result
  result = CloudShape;
  return true;
}

//! Performs point cloud import.
//! \param filename [in]  file to read.
//! \param result   [out] retrieved shape.
//! \return true in case of success, false -- otherwise.
bool geom_point_cloud::ReadScattered(const TCollection_AsciiString& filename,
                                     TopoDS_Shape&                  result)
{
  // Read points as positional (scattered) cloud
  QrTools_ReadXYZ ReadXYZ;
  QrPtr<QrGeom3D_PointCloud> Cloud;
  if ( !ReadXYZ(filename.ToCString(), QrTools_ReadXYZ::Type_Positions, Cloud) )
  {
    std::cout << "Cannot read scattered points from file" << std::endl;
    return false;
  }

  // Convert to positional cloud
  QrPtr<QrGeom3D_PositionCloud> PositionCloud = QrPtr<QrGeom3D_PositionCloud>::DownCast(Cloud);
  if ( PositionCloud.IsNull() )
  {
    std::cout << "Cannot treat point cloud as scattered cloud" << std::endl;
    return false;
  }

  // Convert to topology
  BRep_Builder BB;
  TopoDS_Compound CloudShape;
  BB.MakeCompound(CloudShape);
  //
  for ( int p = 0; p < PositionCloud->NumberOfPoints(); ++p )
  {
    TopoDS_Vertex v = BRepBuilderAPI_MakeVertex( gp_Pnt( PositionCloud->Point(p).X(),
                                                         PositionCloud->Point(p).Y(),
                                                         PositionCloud->Point(p).Z() ) );
    BB.Add(CloudShape, v);
  }

  // Set result
  result = CloudShape;
  return true;
}
