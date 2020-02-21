//-----------------------------------------------------------------------------
// Created on: 08 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiData_IVTopoItemNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_IVTopoItemNode::asiData_IVTopoItemNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,  PID_Name);
  REGISTER_PARAMETER(Shape, PID_Geometry);
  REGISTER_PARAMETER(Group, PID_GroupTess);
  REGISTER_PARAMETER(Real,  PID_TessLinDefl);
  REGISTER_PARAMETER(Real,  PID_TessAngDefl);
  REGISTER_PARAMETER(Group, PID_GroupPrs);
  REGISTER_PARAMETER(Bool,  PID_HasColor);
  REGISTER_PARAMETER(Int,   PID_Color);
}

//-----------------------------------------------------------------------------

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_IVTopoItemNode::Instance()
{
  return new asiData_IVTopoItemNode();
}

//-----------------------------------------------------------------------------

//! Performs initial actions required to make Node WELL-FORMED.
void asiData_IVTopoItemNode::Init()
{
  // Set default values.
  this->SetShape             ( TopoDS_Shape() );
  this->SetLinearDeflection  ( 0.0 );
  this->SetAngularDeflection ( 0.0 );
  this->SetHasColor          ( true );
  this->SetColor             ( 150 << 16 | 150 << 8 | 150 );

  // Initialize Parameter flags.
  this->InitParameter(PID_Name,        "Name",               "", ParameterFlag_IsVisible, true);
  this->InitParameter(PID_GroupTess,   "Tessellation",       "", ParameterFlag_IsVisible, true);
  this->InitParameter(PID_TessLinDefl, "Linear deflection",  "", ParameterFlag_IsVisible, true);
  this->InitParameter(PID_TessAngDefl, "Angular deflection", "", ParameterFlag_IsVisible, true);
  //
  this->InitParameter(PID_GroupPrs, "Presentation", "",               ParameterFlag_IsVisible, true);
  this->InitParameter(PID_HasColor, "Colorized",    "",               ParameterFlag_IsVisible, true);
  this->InitParameter(PID_Color,    "Color",        "PrsCustomColor", ParameterFlag_IsVisible, true);
}

//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_IVTopoItemNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//-----------------------------------------------------------------------------

//! Sets name for the Node.
//! \param[in] name name to set.
void asiData_IVTopoItemNode::SetName(const TCollection_ExtendedString& name)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(name);
}

//-----------------------------------------------------------------------------

//! \return stored geometry.
TopoDS_Shape asiData_IVTopoItemNode::GetShape() const
{
  return ActParamTool::AsShape( this->Parameter(PID_Geometry) )->GetShape();
}

//-----------------------------------------------------------------------------

//! Sets shape to store.
//! \param[in] shape shape to store.
void asiData_IVTopoItemNode::SetShape(const TopoDS_Shape& shape)
{
  ActParamTool::AsShape( this->Parameter(PID_Geometry) )->SetShape(shape);
}

//-----------------------------------------------------------------------------

//! Sets linear deflection.
//! \param[in] defl value to set.
void asiData_IVTopoItemNode::SetLinearDeflection(const double defl)
{
  ActParamTool::AsReal( this->Parameter(PID_TessLinDefl) )->SetValue(defl);
}

//-----------------------------------------------------------------------------

//! \return linear deflection.
double asiData_IVTopoItemNode::GetLinearDeflection() const
{
  return ActParamTool::AsReal( this->Parameter(PID_TessLinDefl) )->GetValue();
}

//-----------------------------------------------------------------------------

//! Sets angular deflection.
//! \param[in] defl value to set.
void asiData_IVTopoItemNode::SetAngularDeflection(const double defl)
{
  ActParamTool::AsReal( this->Parameter(PID_TessAngDefl) )->SetValue(defl);
}

//-----------------------------------------------------------------------------

//! \return angular deflection.
double asiData_IVTopoItemNode::GetAngularDeflection() const
{
  return ActParamTool::AsReal( this->Parameter(PID_TessAngDefl) )->GetValue();
}

//-----------------------------------------------------------------------------

//! Sets the Boolean value indicating whether the color Parameter of this
//! Data Node is in force.
//! \param[in] hasColor value to set.
void asiData_IVTopoItemNode::SetHasColor(const bool hasColor)
{
  ActParamTool::AsBool( this->Parameter(PID_HasColor) )->SetValue(hasColor);
}

//-----------------------------------------------------------------------------

//! Accessor for the value of the Boolean Parameter indicating whether the
//! Color Parameter of this Data Node is in force.
//! \return true/false.
bool asiData_IVTopoItemNode::HasColor() const
{
  return ActParamTool::AsBool( this->Parameter(PID_HasColor) )->GetValue();
}

//-----------------------------------------------------------------------------

//! Sets color.
//! \param[in] color color to set.
void asiData_IVTopoItemNode::SetColor(const int color)
{
  ActParamTool::AsInt( this->Parameter(PID_Color) )->SetValue(color);
}

//-----------------------------------------------------------------------------

//! Accessor for the stored color value.
//! \return color value.
int asiData_IVTopoItemNode::GetColor() const
{
  return ActParamTool::AsInt( this->Parameter(PID_Color) )->GetValue();
}
