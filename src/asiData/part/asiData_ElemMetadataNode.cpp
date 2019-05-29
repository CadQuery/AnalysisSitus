//-----------------------------------------------------------------------------
// Created on: 27 May 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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
#include <asiData_ElemMetadataNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_ElemMetadataNode::asiData_ElemMetadataNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,  PID_Name);
  REGISTER_PARAMETER(Shape, PID_Shape);
  REGISTER_PARAMETER(Int,   PID_Color);
}

//-----------------------------------------------------------------------------

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_ElemMetadataNode::Instance()
{
  return new asiData_ElemMetadataNode();
}

//-----------------------------------------------------------------------------

//! Performs initial actions required to make this Node WELL-FORMED.
void asiData_ElemMetadataNode::Init()
{
  // Initialize name Parameter.
  this->InitParameter(PID_Name, "Name");

  // Initialize Parameters with default values.
  this->SetShape    ( TopoDS_Shape() );
  this->SetColor    ( -1 );
}

//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_ElemMetadataNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//-----------------------------------------------------------------------------

//! Sets name for the Node.
//! \param[in] name name to set.
void asiData_ElemMetadataNode::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------

//! \return stored B-Rep shape.
TopoDS_Shape asiData_ElemMetadataNode::GetShape() const
{
  return ActParamTool::AsShape( this->Parameter(PID_Shape) )->GetShape();
}

//-----------------------------------------------------------------------------

//! Sets shape.
//! \param[in] shape B-Rep shape to set.
void asiData_ElemMetadataNode::SetShape(const TopoDS_Shape& shape)
{
  ActParamTool::AsShape( this->Parameter(PID_Shape) )->SetShape(shape);
}

//-----------------------------------------------------------------------------

//! Sets color.
//! \param[in] color color to set.
void asiData_ElemMetadataNode::SetColor(const int color)
{
  ActParamTool::AsInt( this->Parameter(PID_Color) )->SetValue(color);
}

//-----------------------------------------------------------------------------

//! Accessor for the stored color value.
//! \return color value.
int asiData_ElemMetadataNode::GetColor() const
{
  return ActParamTool::AsInt( this->Parameter(PID_Color) )->GetValue();
}

//-----------------------------------------------------------------------------

//! Creates a DTO object from the Data Node's persistent contents.
//! \return DTO instance.
Handle(asiData_ElemMetadataDTO) asiData_ElemMetadataNode::CreateDTO()
{
  Handle(asiData_ElemMetadataDTO) res = new asiData_ElemMetadataDTO;
  //
  res->Name  = this->GetName();
  res->Shape = this->GetShape();
  res->Color = this->GetColor();

  return res;
}
