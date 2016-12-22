//-----------------------------------------------------------------------------
// Created on: 21 July 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiData_IVTextItemNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_IVTextItemNode::asiData_IVTextItemNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,        PID_Name);
  REGISTER_PARAMETER(AsciiString, PID_Text);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_IVTextItemNode::Instance()
{
  return new asiData_IVTextItemNode();
}

//! Performs initial actions required to make Node WELL-FORMED.
void asiData_IVTextItemNode::Init()
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
TCollection_ExtendedString asiData_IVTextItemNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void asiData_IVTextItemNode::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! \return stored text.
TCollection_AsciiString asiData_IVTextItemNode::GetText() const
{
  return ActParamTool::AsAsciiString( this->Parameter(PID_Text) )->GetValue();
}

//! Sets text to store.
//! \param text [in] text to store.
void asiData_IVTextItemNode::SetText(const TCollection_AsciiString& text)
{
  ActParamTool::AsAsciiString( this->Parameter(PID_Text) )->SetValue(text);
}
