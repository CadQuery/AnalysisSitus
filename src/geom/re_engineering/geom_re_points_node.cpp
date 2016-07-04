//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_re_points_node.h>

// Common includes
#include <common_facilities.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
geom_re_points_node::geom_re_points_node() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,      PID_Name);
  REGISTER_PARAMETER(RealArray, PID_Points);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) geom_re_points_node::Instance()
{
  return new geom_re_points_node();
}

//! Performs initial actions required to make Node WELL-FORMED.
void geom_re_points_node::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");
  //
  ActParamTool::AsRealArray( this->Parameter(PID_Points) )->SetArray(NULL);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString geom_re_points_node::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void geom_re_points_node::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy API
//-----------------------------------------------------------------------------

//! \return stored point cloud.
Handle(geom_point_cloud) geom_re_points_node::GetPoints() const
{
  Handle(TColStd_HArray1OfReal)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Points) )->GetArray();
  //
  Handle(geom_point_cloud) result = new geom_point_cloud(coords);
  return result;
}

//! Sets point cloud to store.
//! \param points [in] points to store.
void geom_re_points_node::TX_SetPoints(const Handle(geom_point_cloud)& points)
{
  common_facilities::Instance()->Model->OpenCommand();
  {
    ActParamTool::AsRealArray( this->Parameter(PID_Points) )->SetArray( points->GetPoints() );
  }
  common_facilities::Instance()->Model->CommitCommand();
}
