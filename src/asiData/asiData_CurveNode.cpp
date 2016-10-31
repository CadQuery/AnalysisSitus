//-----------------------------------------------------------------------------
// Created on: 18 August 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiData_CurveNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_CurveNode::asiData_CurveNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name, PID_Name);
  REGISTER_PARAMETER(Int,  PID_SelectedEdge);
}

//! Returns new DETACHED instance of Curve Node ensuring its correct
//! allocation in a heap.
//! \return new instance of Curve Node.
Handle(ActAPI_INode) asiData_CurveNode::Instance()
{
  return new asiData_CurveNode();
}

//! Performs initial actions required to make Curve Node WELL-FORMED.
void asiData_CurveNode::Init()
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
TCollection_ExtendedString asiData_CurveNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param name [in] name to set.
void asiData_CurveNode::SetName(const TCollection_ExtendedString& name)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(name);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! Sets index of the active edge.
//! \param edgeId [in] index of the active edge.
void asiData_CurveNode::SetSelectedEdge(const int edgeId)
{
  ActParamTool::AsInt( this->Parameter(PID_SelectedEdge) )->SetValue(edgeId);
}

//! \return index of the selected edge.
int asiData_CurveNode::GetSelectedEdge() const
{
  return ActParamTool::AsInt( this->Parameter(PID_SelectedEdge) )->GetValue();
}
