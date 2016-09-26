//-----------------------------------------------------------------------------
// Created on: 19 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_contour_points_data_provider.h>

// Common includes
#include <common_facilities.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>

//-----------------------------------------------------------------------------

//! Constructor.
visu_contour_points_data_provider::visu_contour_points_data_provider()
: visu_points_data_provider( common_facilities::Instance()->Model->GetPartNode()->GetContour() )
{}

//-----------------------------------------------------------------------------

//! \return point cloud to visualize.
Handle(geom_point_cloud) visu_contour_points_data_provider::GetPoints() const
{
  Handle(geom_contour_node)
    contour_n = Handle(geom_contour_node)::DownCast(m_node);
  //
  if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
    return NULL;

  // Get B-Rep representation of the contour
  TopoDS_Wire contourWire = contour_n->AsShape();

  // Number of vertices
  TopTools_IndexedMapOfShape vertices;
  TopExp::MapShapes(contourWire, TopAbs_VERTEX, vertices);
  const int nVertices = vertices.Extent();
  //
  if ( !nVertices )
    return NULL;

  // Build point cloud from vertices
  Handle(HRealArray) coords = new HRealArray(0, nVertices*3 - 1);
  //
  for ( int v = 0; v < nVertices; ++v )
  {
    const TopoDS_Vertex& V = TopoDS::Vertex( vertices(v + 1) );
    gp_Pnt               P = BRep_Tool::Pnt(V);

    coords->SetValue( v*3,     P.X() );
    coords->SetValue( v*3 + 1, P.Y() );
    coords->SetValue( v*3 + 2, P.Z() );
  }
  //
  return new geom_point_cloud(coords);
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) visu_contour_points_data_provider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  Handle(geom_contour_node)
    points_n = Handle(geom_contour_node)::DownCast(m_node);
  //
  if ( points_n.IsNull() || !points_n->IsWellFormed() )
    return out;

  // Register Parameter as sensitive
  out << points_n->Parameter(geom_contour_node::PID_Coords)
      << points_n->Parameter(geom_contour_node::PID_Geometry);

  return out;
}
