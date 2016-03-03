//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_ubend_laws_node.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
geom_ubend_laws_node::geom_ubend_laws_node() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name, PID_Name);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of Node.
Handle(ActAPI_INode) geom_ubend_laws_node::Instance()
{
  return new geom_ubend_laws_node();
}

//! Performs initial actions required to make Node WELL-FORMED.
void geom_ubend_laws_node::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString geom_ubend_laws_node::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void geom_ubend_laws_node::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}
