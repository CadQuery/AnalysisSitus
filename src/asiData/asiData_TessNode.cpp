//-----------------------------------------------------------------------------
// Created on: 26 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

// Own include
#include <asiData_TessNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// Mesh (Active Data) includes
#include <Mesh_Node.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_TessNode::asiData_TessNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name, PID_Name);
  REGISTER_PARAMETER(Mesh, PID_Mesh);
  REGISTER_PARAMETER(Int,  PID_DisplayMode);
  REGISTER_PARAMETER(Bool, PID_HasColor);
  REGISTER_PARAMETER(Int,  PID_Color);
}

//! Returns new DETACHED instance of Mesh Node ensuring its correct
//! allocation in a heap.
//! \return new instance of Mesh Node.
Handle(ActAPI_INode) asiData_TessNode::Instance()
{
  return new asiData_TessNode();
}

//! Performs initial actions required to make Mesh Node WELL-FORMED.
void asiData_TessNode::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");

  // Set empty initial mesh
  ActParamTool::AsMesh( this->Parameter(PID_Mesh) )->SetMesh(new ActData_Mesh, MT_Silent);

  // Set Presentation values
  this->SetHasColor(false);
  this->SetColor(-1); // No color
  this->SetDisplayMode(1); // No display mode
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_TessNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void asiData_TessNode::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! \return stored tessellation.
Handle(ActData_Mesh) asiData_TessNode::GetMesh() const
{
  return ActParamTool::AsMesh( this->Parameter(PID_Mesh) )->GetMesh();
}

//! Sets tessellation to store.
//! \param mesh [in] tessellation to store.
void asiData_TessNode::SetMesh(const Handle(ActData_Mesh)& mesh)
{
  ActParamTool::AsMesh( this->Parameter(PID_Mesh) )->SetMesh(mesh);
}

//! Sets the Boolean value indicating whether the color Parameter of this
//! Data Node is in force.
//! \param hasColor [in] value to set.
void asiData_TessNode::SetHasColor(const bool hasColor) const
{
  ActParamTool::AsBool( this->Parameter(PID_HasColor) )->SetValue(hasColor);
}

//! Accessor for the value of the Boolean Parameter indicating whether the
//! Color Parameter of this Data Node is in force.
//! \return true/false.
bool asiData_TessNode::HasColor() const
{
  return ActParamTool::AsBool( this->Parameter(PID_HasColor) )->GetValue();
}

//! Sets color.
//! \param theColor [in] color to set.
void asiData_TessNode::SetColor(const int theColor) const
{
  ActParamTool::AsInt( this->Parameter(PID_Color) )->SetValue(theColor);
}

//! Accessor for the stored color value.
//! \return color value.
int asiData_TessNode::GetColor() const
{
  return ActParamTool::AsInt( this->Parameter(PID_Color) )->GetValue();
}

//! Sets display mode.
//! \param theMode [in] display mode value to set.
void asiData_TessNode::SetDisplayMode(const int theMode) const
{
  ActParamTool::AsInt( this->Parameter(PID_DisplayMode) )->SetValue(theMode);
}

//! Accessor for the stored display mode value.
//! \return display mode value.
int asiData_TessNode::GetDisplayMode() const
{
  return ActParamTool::AsInt( this->Parameter(PID_DisplayMode) )->GetValue();
}
