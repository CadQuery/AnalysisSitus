//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_REPointsDataProvider.h>

// Common includes
#include <common_facilities.h>

//-----------------------------------------------------------------------------

//! Constructor.
asiVisu_REPointsDataProvider::asiVisu_REPointsDataProvider()
: asiVisu_PointsDataProvider( common_facilities::Instance()->Model->GetRENode()->Points() )
{}

//-----------------------------------------------------------------------------

//! \return point cloud to visualize.
Handle(asiAlgo_PointCloud) asiVisu_REPointsDataProvider::GetPoints() const
{
  Handle(asiData_REPointsNode)
    points_n = Handle(asiData_REPointsNode)::DownCast(m_node);
  //
  if ( points_n.IsNull() || !points_n->IsWellFormed() )
    return NULL;

  return points_n->GetPoints();
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_REPointsDataProvider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  Handle(asiData_REPointsNode)
    points_n = Handle(asiData_REPointsNode)::DownCast(m_node);
  //
  if ( points_n.IsNull() || !points_n->IsWellFormed() )
    return out;

  // Register Parameter as sensitive
  out << points_n->Parameter(asiData_REPointsNode::PID_Points);

  return out;
}
