//-----------------------------------------------------------------------------
// Created on: 04 August 2019
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
#include <asiEngine_SmoothenCornersFunc.h>

// asiEngine includes
#include <asiEngine_PatchJointAdaptor.h>
#include <asiEngine_RE.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

Handle(asiEngine_SmoothenCornersFunc) asiEngine_SmoothenCornersFunc::Instance()
{
  return new asiEngine_SmoothenCornersFunc();
}

//-----------------------------------------------------------------------------

const char* asiEngine_SmoothenCornersFunc::GUID()
{
  return "0031FF6A-018C-4706-8ECE-1150CBB868DB";
}

//-----------------------------------------------------------------------------

const char* asiEngine_SmoothenCornersFunc::GetGUID() const
{
  return GUID();
}

//-----------------------------------------------------------------------------

bool asiEngine_SmoothenCornersFunc::MustExecuteIntact(const Handle(ActAPI_HParameterList)& inputs,
                                                      const Handle(Standard_Transient)&    userData) const
{
  if ( inputs.IsNull() )
    return false;

  // Get Data Model.
  Handle(asiEngine_Model) M = Handle(asiEngine_Model)::DownCast(userData);

  // Get edge in question.
  Handle(asiData_ReEdgeNode)
    edgeNode = Handle(asiData_ReEdgeNode)::DownCast( inputs->First()->GetNode() );

  // Initialize utility to extract local topology of an edge.
  asiEngine_PatchJointAdaptor jointAdt(M, m_progress, m_plotter);
  //
  if ( !jointAdt.Init(edgeNode) )
    return false;

  // Get coedges.
  const Handle(asiData_ReCoedgeNode)& coedgeLeftBot  = jointAdt.GetCoedgeLeftBot();
  const Handle(asiData_ReCoedgeNode)& coedgeRightBot = jointAdt.GetCoedgeRightBot();
  const Handle(asiData_ReCoedgeNode)& coedgeLeftTop  = jointAdt.GetCoedgeLeftTop();
  const Handle(asiData_ReCoedgeNode)& coedgeRightTop = jointAdt.GetCoedgeRightTop();

  // Get referenced edges.
  Handle(asiData_ReEdgeNode) edgeLeftBot  = coedgeLeftBot  ->GetEdge();
  Handle(asiData_ReEdgeNode) edgeRightBot = coedgeRightBot ->GetEdge();
  Handle(asiData_ReEdgeNode) edgeLeftTop  = coedgeLeftTop  ->GetEdge();
  Handle(asiData_ReEdgeNode) edgeRightTop = coedgeRightTop ->GetEdge();

  if ( ActData_LogBook::IsModifiedCursor( edgeLeftBot->Parameter(asiData_ReEdgeNode::PID_Curve) ) )
    return true;

  if ( ActData_LogBook::IsModifiedCursor( edgeRightBot->Parameter(asiData_ReEdgeNode::PID_Curve) ) )
    return true;

  if ( ActData_LogBook::IsModifiedCursor( edgeLeftTop->Parameter(asiData_ReEdgeNode::PID_Curve) ) )
    return true;

  if ( ActData_LogBook::IsModifiedCursor( edgeRightTop->Parameter(asiData_ReEdgeNode::PID_Curve) ) )
    return true;

  return false;
}

//-----------------------------------------------------------------------------

int asiEngine_SmoothenCornersFunc::execute(const Handle(ActAPI_HParameterList)& inputs,
                                           const Handle(ActAPI_HParameterList)& asiEngine_NotUsed(outputs), // All the job is done by service API.
                                           const Handle(Standard_Transient)&    userData) const
{
  // Get Data Model.
  Handle(asiEngine_Model) M = Handle(asiEngine_Model)::DownCast(userData);

  // Get edge in question.
  Handle(asiData_ReEdgeNode)
    edgeNode = Handle(asiData_ReEdgeNode)::DownCast( inputs->First()->GetNode() );

  m_progress.SendLogMessage( LogNotice(Normal) << "Executing tree function '%1%2'..."
                                               << this->GetName()
                                               << edgeNode->RootLabel().Tag() );

  // Initialize utility to extract local topology of an edge.
  asiEngine_PatchJointAdaptor jointAdt(M, m_progress/*, m_plotter*/);
  //
  if ( !jointAdt.Init(edgeNode) )
    return 1;

  // Align curves.
  if ( !jointAdt.AlignEdges() )
    return 1; // Failure.

  return 0; // Success.
}

//-----------------------------------------------------------------------------

ActAPI_ParameterTypeStream
  asiEngine_SmoothenCornersFunc::inputSignature() const
{
  return ActAPI_ParameterTypeStream() << Parameter_Bool
                                      << Parameter_Shape;
}

//-----------------------------------------------------------------------------

bool asiEngine_SmoothenCornersFunc::validateOutput(const Handle(ActAPI_HParameterList)& asiEngine_NotUsed(outputs)) const
{
  return true;
}

//-----------------------------------------------------------------------------

ActAPI_ParameterTypeStream
  asiEngine_SmoothenCornersFunc::outputSignature() const
{
  return ActAPI_ParameterTypeStream(); // Unused.
}
