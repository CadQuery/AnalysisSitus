//-----------------------------------------------------------------------------
// Created on: 08 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_iv_curves_node.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
visu_iv_curves_node::visu_iv_curves_node() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name, PID_Name);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) visu_iv_curves_node::Instance()
{
  return new visu_iv_curves_node();
}

//! Performs initial actions required to make Node WELL-FORMED.
void visu_iv_curves_node::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString visu_iv_curves_node::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void visu_iv_curves_node::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! Returns a child by its one-based index.
//! \param oneBased_id [in] 1-based index.
//! \return child Node.
Handle(visu_iv_curve_node) visu_iv_curves_node::Curve(const int oneBased_idx)
{
  return Handle(visu_iv_curve_node)::DownCast( this->GetChildNode(oneBased_idx) );
}