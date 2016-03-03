//-----------------------------------------------------------------------------
// Created on: 09 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_sections_node.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
geom_sections_node::geom_sections_node() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,  PID_Name);
  REGISTER_PARAMETER(Shape, PID_Geometry);
}

//! Returns new DETACHED instance of Sections Node ensuring its correct
//! allocation in a heap.
//! \return new instance of Sections Node.
Handle(ActAPI_INode) geom_sections_node::Instance()
{
  return new geom_sections_node();
}

//! Performs initial actions required to make Sections Node WELL-FORMED.
void geom_sections_node::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");

  // Set empty geometry
  this->SetGeometry( TopoDS_Shape() );
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString geom_sections_node::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param name [in] name to set.
void geom_sections_node::SetName(const TCollection_ExtendedString& name)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(name);
}

//-----------------------------------------------------------------------------

//! \return stored geometry.
TopoDS_Shape geom_sections_node::GetGeometry() const
{
  return ActParamTool::AsShape( this->Parameter(PID_Geometry) )->GetShape();
}

//! Sets geometry to store.
//! \param shape [in] shape to store.
void geom_sections_node::SetGeometry(const TopoDS_Shape& shape)
{
  ActParamTool::AsShape( this->Parameter(PID_Geometry) )->SetShape(shape);
}
