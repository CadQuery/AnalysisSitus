//-----------------------------------------------------------------------------
// Created on: 21 July 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_iv_text_item_node.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
visu_iv_text_item_node::visu_iv_text_item_node() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,        PID_Name);
  REGISTER_PARAMETER(AsciiString, PID_Text);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) visu_iv_text_item_node::Instance()
{
  return new visu_iv_text_item_node();
}

//! Performs initial actions required to make Node WELL-FORMED.
void visu_iv_text_item_node::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");

  // Initialize text Parameter
  this->SetText("");
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString visu_iv_text_item_node::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void visu_iv_text_item_node::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! \return stored text.
TCollection_AsciiString visu_iv_text_item_node::GetText() const
{
  return ActParamTool::AsAsciiString( this->Parameter(PID_Text) )->GetValue();
}

//! Sets text to store.
//! \param text [in] text to store.
void visu_iv_text_item_node::SetText(const TCollection_AsciiString& text)
{
  ActParamTool::AsAsciiString( this->Parameter(PID_Text) )->SetValue(text);
}
