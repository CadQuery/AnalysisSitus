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
#include <asiData_ReCoedgeNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default ctor. Registers all involved Parameters.
asiData_ReCoedgeNode::asiData_ReCoedgeNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,      PID_Name);
  REGISTER_PARAMETER(Reference, PID_EdgeRef);
  REGISTER_PARAMETER(Bool,      PID_SameSense);
}

//-----------------------------------------------------------------------------

Handle(ActAPI_INode) asiData_ReCoedgeNode::Instance()
{
  return new asiData_ReCoedgeNode();
}

//-----------------------------------------------------------------------------

void asiData_ReCoedgeNode::Init(const Handle(asiData_ReEdgeNode)& edge,
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

TCollection_ExtendedString asiData_ReCoedgeNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//-----------------------------------------------------------------------------

void asiData_ReCoedgeNode::SetName(const TCollection_ExtendedString& name)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(name);
}

//-----------------------------------------------------------------------------

Handle(asiData_ReEdgeNode) asiData_ReCoedgeNode::GetEdge() const
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

void asiData_ReCoedgeNode::SetEdge(const Handle(asiData_ReEdgeNode)& edge)
{
  if ( edge.IsNull() )
    this->DisconnectReference(PID_EdgeRef);
  else
    this->ConnectReference(PID_EdgeRef, edge);
}

//-----------------------------------------------------------------------------

Handle(asiData_ReVertexNode) asiData_ReCoedgeNode::GetFirstVertex() const
{
  Handle(asiData_ReEdgeNode)   edge = this->GetEdge();
  Handle(asiData_ReVertexNode) vf   = edge->GetFirstVertex();
  Handle(asiData_ReVertexNode) vl   = edge->GetLastVertex();

  if ( this->IsSameSense() )
    return vf;

  return vl;
}

//-----------------------------------------------------------------------------

Handle(asiData_ReVertexNode) asiData_ReCoedgeNode::GetLastVertex() const
{
  Handle(asiData_ReEdgeNode)   edge = this->GetEdge();
  Handle(asiData_ReVertexNode) vf   = edge->GetFirstVertex();
  Handle(asiData_ReVertexNode) vl   = edge->GetLastVertex();

  if ( this->IsSameSense() )
    return vl;

  return vf;
}

//-----------------------------------------------------------------------------

bool asiData_ReCoedgeNode::IsSameSense() const
{
  return ActParamTool::AsBool( this->Parameter(PID_SameSense) )->GetValue();
}

//-----------------------------------------------------------------------------

void asiData_ReCoedgeNode::SetSameSense(const bool isSameSense)
{
  ActParamTool::AsBool( this->Parameter(PID_SameSense) )->SetValue(isSameSense);
}
