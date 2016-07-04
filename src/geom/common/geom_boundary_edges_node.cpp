//-----------------------------------------------------------------------------
// Created on: 22 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_boundary_edges_node.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
geom_boundary_edges_node::geom_boundary_edges_node() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,  PID_Name);
  REGISTER_PARAMETER(Shape, PID_Green);
  REGISTER_PARAMETER(Shape, PID_Red);
  REGISTER_PARAMETER(Shape, PID_Ordinary);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) geom_boundary_edges_node::Instance()
{
  return new geom_boundary_edges_node();
}

//! Performs initial actions required to make this Node WELL-FORMED.
void geom_boundary_edges_node::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");

  // Set empty initial shapes
  ActParamTool::AsShape( this->Parameter(PID_Green)    )->SetShape( TopoDS_Shape(), MT_Silent );
  ActParamTool::AsShape( this->Parameter(PID_Red)      )->SetShape( TopoDS_Shape(), MT_Silent );
  ActParamTool::AsShape( this->Parameter(PID_Ordinary) )->SetShape( TopoDS_Shape(), MT_Silent );
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString geom_boundary_edges_node::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void geom_boundary_edges_node::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}
