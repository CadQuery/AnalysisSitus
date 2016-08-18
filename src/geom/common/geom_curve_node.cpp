//-----------------------------------------------------------------------------
// Created on: 18 August 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_curve_node.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
geom_curve_node::geom_curve_node() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name, PID_Name);
  REGISTER_PARAMETER(Int,  PID_SelectedEdge);
}

//! Returns new DETACHED instance of Curve Node ensuring its correct
//! allocation in a heap.
//! \return new instance of Curve Node.
Handle(ActAPI_INode) geom_curve_node::Instance()
{
  return new geom_curve_node();
}

//! Performs initial actions required to make Curve Node WELL-FORMED.
void geom_curve_node::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");

  // Set default values to primitive Parameters
  this->SetSelectedEdge(0);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString geom_curve_node::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param name [in] name to set.
void geom_curve_node::SetName(const TCollection_ExtendedString& name)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(name);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! Sets index of the active edge.
//! \param edgeId [in] index of the active edge.
void geom_curve_node::SetSelectedEdge(const int edgeId)
{
  ActParamTool::AsInt( this->Parameter(PID_SelectedEdge) )->SetValue(edgeId);
}

//! \return index of the selected edge.
int geom_curve_node::GetSelectedEdge() const
{
  return ActParamTool::AsInt( this->Parameter(PID_SelectedEdge) )->GetValue();
}
