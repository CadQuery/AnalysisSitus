//-----------------------------------------------------------------------------
// Created on: 18 April 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018, Sergey Slyadnev
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
//    * Neither the name of Sergey Slyadnev nor the
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
#include <asiData_TolerantRangeNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_TolerantRangeNode::asiData_TolerantRangeNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,  PID_Name);
  REGISTER_PARAMETER(Shape, PID_Shape);
  REGISTER_PARAMETER(Real,  PID_TolerMin);
  REGISTER_PARAMETER(Real,  PID_TolerMax);
}

//-----------------------------------------------------------------------------

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_TolerantRangeNode::Instance()
{
  return new asiData_TolerantRangeNode();
}

//-----------------------------------------------------------------------------

//! Performs initial actions required to make this Node WELL-FORMED.
void asiData_TolerantRangeNode::Init()
{
  // Initialize name Parameter.
  this->InitParameter(PID_Name, "Name");

  // Initialize Parameters with default values.
  this->SetShape    ( TopoDS_Shape() );
  this->SetMinToler ( 0.0 );
  this->SetMaxToler ( 0.0 );
}

//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_TolerantRangeNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//-----------------------------------------------------------------------------

//! Sets name for the Node.
//! \param[in] name name to set.
void asiData_TolerantRangeNode::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------

//! \return stored B-Rep shape.
TopoDS_Shape asiData_TolerantRangeNode::GetShape() const
{
  return ActParamTool::AsShape( this->Parameter(PID_Shape) )->GetShape();
}

//-----------------------------------------------------------------------------

//! Sets shape.
//! \param[in] shape B-Rep shape to set.
void asiData_TolerantRangeNode::SetShape(const TopoDS_Shape& shape)
{
  ActParamTool::AsShape( this->Parameter(PID_Shape) )->SetShape(shape);
}

//-----------------------------------------------------------------------------

//! \return min tolerance which is the lower bound of the stored range.
double asiData_TolerantRangeNode::GetMinToler() const
{
  return ActParamTool::AsReal( this->Parameter(PID_TolerMin) )->GetValue();
}

//-----------------------------------------------------------------------------

//! Sets min tolerance value.
//! \param[in] value tolerance value to set.
void asiData_TolerantRangeNode::SetMinToler(const double value)
{
  ActParamTool::AsReal( this->Parameter(PID_TolerMin) )->SetValue(value);
}

//-----------------------------------------------------------------------------

//! \return max tolerance which is the upper bound of the stored range.
double asiData_TolerantRangeNode::GetMaxToler() const
{
  return ActParamTool::AsReal( this->Parameter(PID_TolerMax) )->GetValue();
}

//-----------------------------------------------------------------------------

//! Sets max tolerance value.
//! \param[in] value tolerance value to set.
void asiData_TolerantRangeNode::SetMaxToler(const double value)
{
  ActParamTool::AsReal( this->Parameter(PID_TolerMax) )->SetValue(value);
}
