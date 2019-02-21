//-----------------------------------------------------------------------------
// Created on: 06 October 2018
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
#include <asiData_ReTopoNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default ctor. Registers all involved Parameters.
asiData_ReTopoNode::asiData_ReTopoNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,      PID_Name);
  REGISTER_PARAMETER(Reference, PID_CurrentPatchRef);
  REGISTER_PARAMETER(Reference, PID_CurrentCoEdgeRef);
  REGISTER_PARAMETER(Reference, PID_FirstVertexRef);
  REGISTER_PARAMETER(Reference, PID_LastVertexRef);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_ReTopoNode::Instance()
{
  return new asiData_ReTopoNode();
}

//! Performs initial actions required to make Node WELL-FORMED.
void asiData_ReTopoNode::Init()
{
  // Initialize Parameters
  this->InitParameter(PID_Name,             "Name");
  this->InitParameter(PID_CurrentPatchRef,  "Current patch");
  this->InitParameter(PID_CurrentCoEdgeRef, "Current coedge");
  this->InitParameter(PID_FirstVertexRef,   "First vertex");
  this->InitParameter(PID_LastVertexRef,    "Last vertex");
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_ReTopoNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param[in] name name to set.
void asiData_ReTopoNode::SetName(const TCollection_ExtendedString& name)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(name);
}

//-----------------------------------------------------------------------------

//! Sets reference to the currently composed Patch.
//! \param[in] patch reference to set.
void asiData_ReTopoNode::SetCurrentPatch(const Handle(asiData_RePatchNode)& patch)
{
  if ( patch.IsNull() )
    this->DisconnectReference(PID_CurrentPatchRef);
  else
    this->ConnectReference(PID_CurrentPatchRef, patch);
}

//! \return referenced Patch Node.
Handle(asiData_RePatchNode) asiData_ReTopoNode::GetCurrentPatch() const
{
  Handle(ActAPI_IDataCursor)
    dc = ActParamTool::AsReference( this->Parameter(PID_CurrentPatchRef) )->GetTarget();
  //
  if ( dc.IsNull() ||
      !dc->IsWellFormed() ||
      !dc->IsInstance( STANDARD_TYPE(asiData_RePatchNode) ) )
    //
    return NULL;

  return Handle(asiData_RePatchNode)::DownCast(dc);
}

//! Sets reference to the currently composed CoEdge.
//! \param[in] coedge reference to set.
void asiData_ReTopoNode::SetCurrentCoEdge(const Handle(asiData_ReCoEdgeNode)& coedge)
{
  if ( coedge.IsNull() )
    this->DisconnectReference(PID_CurrentCoEdgeRef);
  else
    this->ConnectReference(PID_CurrentCoEdgeRef, coedge);
}

//! \return referenced CoEdge Node.
Handle(asiData_ReCoEdgeNode) asiData_ReTopoNode::GetCurrentCoEdge() const
{
  Handle(ActAPI_IDataCursor)
    dc = ActParamTool::AsReference( this->Parameter(PID_CurrentCoEdgeRef) )->GetTarget();
  //
  if ( dc.IsNull() ||
      !dc->IsWellFormed() ||
      !dc->IsInstance( STANDARD_TYPE(asiData_ReCoEdgeNode) ) )
    //
    return NULL;

  return Handle(asiData_ReCoEdgeNode)::DownCast(dc);
}

//! Sets reference to the first created Vertex.
//! \param[in] vertex reference to set.
void asiData_ReTopoNode::SetFirstVertex(const Handle(asiData_ReVertexNode)& vertex)
{
  if ( vertex.IsNull() )
    this->DisconnectReference(PID_FirstVertexRef);
  else
    this->ConnectReference(PID_FirstVertexRef, vertex);
}

//! \return referenced Vertex Node.
Handle(asiData_ReVertexNode) asiData_ReTopoNode::GetFirstVertex() const
{
  Handle(ActAPI_IDataCursor)
    dc = ActParamTool::AsReference( this->Parameter(PID_FirstVertexRef) )->GetTarget();
  //
  if ( dc.IsNull() ||
      !dc->IsWellFormed() ||
      !dc->IsInstance( STANDARD_TYPE(asiData_ReVertexNode) ) )
    //
    return NULL;

  return Handle(asiData_ReVertexNode)::DownCast(dc);
}

//! Sets reference to the last created Vertex.
//! \param[in] vertex reference to set.
void asiData_ReTopoNode::SetLastVertex(const Handle(asiData_ReVertexNode)& vertex)
{
  if ( vertex.IsNull() )
    this->DisconnectReference(PID_LastVertexRef);
  else
    this->ConnectReference(PID_LastVertexRef, vertex);
}

//! \return referenced Vertex Node.
Handle(asiData_ReVertexNode) asiData_ReTopoNode::GetLastVertex() const
{
  Handle(ActAPI_IDataCursor)
    dc = ActParamTool::AsReference( this->Parameter(PID_LastVertexRef) )->GetTarget();
  //
  if ( dc.IsNull() ||
      !dc->IsWellFormed() ||
      !dc->IsInstance( STANDARD_TYPE(asiData_ReVertexNode) ) )
    //
    return NULL;

  return Handle(asiData_ReVertexNode)::DownCast(dc);
}
