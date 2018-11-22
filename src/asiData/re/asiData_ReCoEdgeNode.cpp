//-----------------------------------------------------------------------------
// Created on: 22 November 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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
#include <asiData_ReCoEdgeNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default ctor. Registers all involved Parameters.
asiData_ReCoEdgeNode::asiData_ReCoEdgeNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,      PID_Name);
  REGISTER_PARAMETER(Reference, PID_EdgeRef);
  REGISTER_PARAMETER(Bool,      PID_SameSense);
}

//-----------------------------------------------------------------------------

Handle(ActAPI_INode) asiData_ReCoEdgeNode::Instance()
{
  return new asiData_ReCoEdgeNode();
}

//-----------------------------------------------------------------------------

void asiData_ReCoEdgeNode::Init(const Handle(asiData_ReEdgeNode)& edge,
                                const bool                        samesense)
{
  // Initialize Parameters.
  this->InitParameter(PID_Name,      "Name");
  this->InitParameter(PID_EdgeRef,   "Edge ref.");
  this->InitParameter(PID_SameSense, "Same sense");

  // Store the passed data in Parameters.
  this->SetEdge(edge);
  this->SetSameSense(samesense);
}

//-----------------------------------------------------------------------------

TCollection_ExtendedString asiData_ReCoEdgeNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//-----------------------------------------------------------------------------

void asiData_ReCoEdgeNode::SetName(const TCollection_ExtendedString& name)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(name);
}

//-----------------------------------------------------------------------------

Handle(asiData_ReEdgeNode) asiData_ReCoEdgeNode::GetEdge() const
{
  Handle(ActAPI_IDataCursor)
    dc = ActParamTool::AsReference( this->Parameter(PID_EdgeRef) )->GetTarget();
  //
  if ( dc.IsNull() ||
      !dc->IsWellFormed() ||
      !dc->IsInstance( STANDARD_TYPE(asiData_ReEdgeNode) ) )
    //
    return NULL;

  return Handle(asiData_ReEdgeNode)::DownCast(dc);
}

//-----------------------------------------------------------------------------

void asiData_ReCoEdgeNode::SetEdge(const Handle(asiData_ReEdgeNode)& edge)
{
  if ( edge.IsNull() )
    this->DisconnectReference(PID_EdgeRef);
  else
    this->ConnectReference(PID_EdgeRef, edge);
}

//-----------------------------------------------------------------------------

bool asiData_ReCoEdgeNode::IsSameSense() const
{
  return ActParamTool::AsBool( this->Parameter(PID_SameSense) )->GetValue();
}

//-----------------------------------------------------------------------------

void asiData_ReCoEdgeNode::SetSameSense(const bool isSameSense)
{
  ActParamTool::AsBool( this->Parameter(PID_SameSense) )->SetValue(isSameSense);
}
