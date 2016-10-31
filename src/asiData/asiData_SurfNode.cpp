//-----------------------------------------------------------------------------
// Created on: 04 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiData_SurfNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_SurfNode::asiData_SurfNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name, PID_Name);
  REGISTER_PARAMETER(Int,  PID_SelectedFace);
}

//! Returns new DETACHED instance of Surface Node ensuring its correct
//! allocation in a heap.
//! \return new instance of Surface Node.
Handle(ActAPI_INode) asiData_SurfNode::Instance()
{
  return new asiData_SurfNode();
}

//! Performs initial actions required to make Surface Node WELL-FORMED.
void asiData_SurfNode::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");

  // Set default values to primitive Parameters
  this->SetSelectedFace(0);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_SurfNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void asiData_SurfNode::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! Sets index of the active face.
//! \param faceId [in] index of the active face.
void asiData_SurfNode::SetSelectedFace(const int faceId)
{
  ActParamTool::AsInt( this->Parameter(PID_SelectedFace) )->SetValue(faceId);
}

//! \return index of the selected face.
int asiData_SurfNode::GetSelectedFace() const
{
  return ActParamTool::AsInt( this->Parameter(PID_SelectedFace) )->GetValue();
}
