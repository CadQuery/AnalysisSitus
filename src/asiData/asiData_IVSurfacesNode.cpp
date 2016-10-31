//-----------------------------------------------------------------------------
// Created on: 08 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiData_IVSurfacesNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_IVSurfacesNode::asiData_IVSurfacesNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name, PID_Name);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_IVSurfacesNode::Instance()
{
  return new asiData_IVSurfacesNode();
}

//! Performs initial actions required to make Node WELL-FORMED.
void asiData_IVSurfacesNode::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_IVSurfacesNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void asiData_IVSurfacesNode::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! Returns a child by its one-based index.
//! \param oneBased_id [in] 1-based index.
//! \return child Node.
Handle(asiData_IVSurfaceNode) asiData_IVSurfacesNode::Surface(const int oneBased_idx)
{
  return Handle(asiData_IVSurfaceNode)::DownCast( this->GetChildNode(oneBased_idx) );
}
