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
    points_n = Handle(geom_contour_node)::DownCast(m_node);
  //
  if ( points_n.IsNull() || !points_n->IsWellFormed() )
    return NULL;

  Handle(HRealArray) coords = points_n->GetCoords();
  Handle(geom_point_cloud) cloud = new geom_point_cloud(coords);
  return cloud;
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
  out << points_n->Parameter(geom_contour_node::PID_Coords);

  return out;
}
