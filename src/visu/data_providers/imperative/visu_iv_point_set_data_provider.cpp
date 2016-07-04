//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_iv_point_set_data_provider.h>

// Common includes
#include <common_facilities.h>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param pointSet [in] point set to source data from.
visu_iv_point_set_data_provider::visu_iv_point_set_data_provider(const Handle(ActAPI_INode)& pointSet)
: visu_points_data_provider(pointSet)
{}

//-----------------------------------------------------------------------------

//! \return point cloud to visualize.
Handle(geom_point_cloud) visu_iv_point_set_data_provider::GetPoints() const
{
  Handle(visu_iv_point_set_node)
    points_n = Handle(visu_iv_point_set_node)::DownCast(m_node);
  //
  if ( points_n.IsNull() || !points_n->IsWellFormed() )
    return NULL;

  return points_n->GetPoints();
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) visu_iv_point_set_data_provider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  Handle(visu_iv_point_set_node)
    points_n = Handle(visu_iv_point_set_node)::DownCast(m_node);
  //
  if ( points_n.IsNull() || !points_n->IsWellFormed() )
    return out;

  // Register Parameter as sensitive
  out << points_n->Parameter(visu_iv_point_set_node::PID_Geometry);

  return out;
}
