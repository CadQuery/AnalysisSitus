//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_re_points_data_provider.h>

// Common includes
#include <common_facilities.h>

//-----------------------------------------------------------------------------

//! Constructor.
visu_re_points_data_provider::visu_re_points_data_provider()
: visu_points_data_provider( common_facilities::Instance()->Model->GetRENode()->Points() )
{}

//-----------------------------------------------------------------------------

//! \return point cloud to visualize.
Handle(geom_point_cloud) visu_re_points_data_provider::GetPoints() const
{
  Handle(geom_re_points_node)
    points_n = Handle(geom_re_points_node)::DownCast(m_node);
  //
  if ( points_n.IsNull() || !points_n->IsWellFormed() )
    return NULL;

  return points_n->GetPoints();
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) visu_re_points_data_provider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  Handle(geom_re_points_node)
    points_n = Handle(geom_re_points_node)::DownCast(m_node);
  //
  if ( points_n.IsNull() || !points_n->IsWellFormed() )
    return out;

  // Register Parameter as sensitive
  out << points_n->Parameter(geom_re_points_node::PID_Points);

  return out;
}
