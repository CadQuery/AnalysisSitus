//-----------------------------------------------------------------------------
// Created on: 01 March 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiData_RESegmentNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_RESegmentNode::asiData_RESegmentNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,      PID_Name);
  REGISTER_PARAMETER(Selection, PID_Indices);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_RESegmentNode::Instance()
{
  return new asiData_RESegmentNode();
}

//! Performs initial actions required to make Node WELL-FORMED.
void asiData_RESegmentNode::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");
  //
  this->SetIndices(NULL);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_RESegmentNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param name [in] name to set.
void asiData_RESegmentNode::SetName(const TCollection_ExtendedString& name)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(name);
}

//-----------------------------------------------------------------------------
// API
//-----------------------------------------------------------------------------

//! \return indices of the points in a segment.
Handle(TColStd_HPackedMapOfInteger) asiData_RESegmentNode::GetIndices() const
{
  return ActParamTool::AsSelection( this->Parameter(PID_Indices) )->GetMask();
}

//! Sets indices for the points in a segment.
//! \param[in] indices indices to set.
void asiData_RESegmentNode::SetIndices(const Handle(TColStd_HPackedMapOfInteger)& indices)
{
  ActParamTool::AsSelection( this->Parameter(PID_Indices) )->SetMask(indices);
}
