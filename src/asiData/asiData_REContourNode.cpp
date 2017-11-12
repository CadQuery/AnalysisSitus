//-----------------------------------------------------------------------------
// Created on: 06 April 2016
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
#include <asiData_REContourNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <TopoDS.hxx>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_REContourNode::asiData_REContourNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,  PID_Name);
  REGISTER_PARAMETER(Shape, PID_Contour);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_REContourNode::Instance()
{
  return new asiData_REContourNode();
}

//! Performs initial actions required to make Node WELL-FORMED.
//! \param contour [in] contour to store.
void asiData_REContourNode::Init(const TopoDS_Wire& contour)
{
  this->InitParameter(PID_Name, "Name");
  //
  this->SetContour(contour);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_REContourNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void asiData_REContourNode::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Convenience methods
//-----------------------------------------------------------------------------

//! Sets contour to store.
//! \param contour [in] contour to store.
void asiData_REContourNode::SetContour(const TopoDS_Wire& contour)
{
  ActParamTool::AsShape( this->Parameter(PID_Contour) )->SetShape(contour);
}

//! \return stored contour.
TopoDS_Wire asiData_REContourNode::GetContour() const
{
  TopoDS_Shape S = ActParamTool::AsShape( this->Parameter(PID_Contour) )->GetShape();
  //
  if ( S.IsNull() || S.ShapeType() != TopAbs_WIRE )
    return TopoDS_Wire();
  //
  return TopoDS::Wire(S);
}
