//-----------------------------------------------------------------------------
// Created on: 22 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiData_BoundaryEdgesNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_BoundaryEdgesNode::asiData_BoundaryEdgesNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,  PID_Name);
  REGISTER_PARAMETER(Shape, PID_Green);
  REGISTER_PARAMETER(Shape, PID_Red);
  REGISTER_PARAMETER(Shape, PID_Ordinary);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_BoundaryEdgesNode::Instance()
{
  return new asiData_BoundaryEdgesNode();
}

//! Performs initial actions required to make this Node WELL-FORMED.
void asiData_BoundaryEdgesNode::Init()
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
TCollection_ExtendedString asiData_BoundaryEdgesNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void asiData_BoundaryEdgesNode::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}
