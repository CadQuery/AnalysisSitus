//-----------------------------------------------------------------------------
// Created on: 08 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_iv_point_set_node.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
visu_iv_point_set_node::visu_iv_point_set_node() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,      PID_Name);
  REGISTER_PARAMETER(RealArray, PID_Geometry);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) visu_iv_point_set_node::Instance()
{
  return new visu_iv_point_set_node();
}

//! Performs initial actions required to make Node WELL-FORMED.
void visu_iv_point_set_node::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");
  //
  this->SetPoints(NULL);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString visu_iv_point_set_node::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void visu_iv_point_set_node::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy API
//-----------------------------------------------------------------------------

//! \return stored point cloud.
Handle(geom_point_cloud) visu_iv_point_set_node::GetPoints() const
{
  Handle(TColStd_HArray1OfReal)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Geometry) )->GetArray();
  //
  Handle(geom_point_cloud) result = new geom_point_cloud(coords);
  return result;
}

//! Sets point cloud to store.
//! \param points [in] points to store.
void visu_iv_point_set_node::SetPoints(const Handle(geom_point_cloud)& points)
{
  ActParamTool::AsRealArray( this->Parameter(PID_Geometry) )->SetArray( points.IsNull() ? NULL : points->GetPoints() );
}
