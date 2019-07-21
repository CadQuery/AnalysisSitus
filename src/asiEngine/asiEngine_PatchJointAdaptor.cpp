//-----------------------------------------------------------------------------
// Created on: 28 June 2019
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
#include <asiEngine_PatchJointAdaptor.h>

// asiEngine includes
#include <asiEngine_RE.h>

//-----------------------------------------------------------------------------

asiEngine_PatchJointAdaptor::asiEngine_PatchJointAdaptor(const Handle(asiEngine_Model)& model,
                                                         ActAPI_ProgressEntry           progress,
                                                         ActAPI_PlotterEntry            plotter)
: asiAlgo_PatchJointAdaptor (progress, plotter),
  m_model                   (model)
{}

//-----------------------------------------------------------------------------

bool asiEngine_PatchJointAdaptor::Init(const Handle(asiData_ReEdgeNode)& edgeNode)
{
  if ( edgeNode.IsNull() || !edgeNode->IsWellFormed() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Null or inconsistent edge.");
    return false;
  }

  m_edge = edgeNode;

  // Prepare RE API.
  asiEngine_RE reApi(m_model, m_progress);

  // Get owner patches.
  if ( !reApi.GetPatchesByEdge(m_edge,
                               m_coedgeLeft, m_coedgeRight,
                               m_patchLeft, m_patchRight) )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot get patches by edge.");
    return false;
  }

  if ( !m_patchLeft.IsNull() )
    m_progress.SendLogMessage( LogInfo(Normal) << "Left patch: %1." << m_patchLeft->GetName() );

  if ( !m_patchRight.IsNull() )
    m_progress.SendLogMessage( LogInfo(Normal) << "Right patch: %1." << m_patchRight->GetName() );

  if ( m_patchRight.IsNull() || m_patchRight.IsNull() )
  {
    m_progress.SendLogMessage( LogWarn(Normal) << "Naked edge: there is no left or/and right patch." );
    return false; // Naked edge.
  }

  if ( m_coedgeLeft.IsNull() || m_coedgeRight.IsNull() )
  {
    m_progress.SendLogMessage( LogWarn(Normal) << "Incomplete topology: there is no left or/and right coedge." );
    return false; // Incomplete topology.
  }

  // Get next and previous coedges.
  m_coedgeLeftTop  = reApi.GetNext     (m_coedgeLeft);
  m_coedgeLeftBot  = reApi.GetPrevious (m_coedgeLeft);
  m_coedgeRightTop = reApi.GetPrevious (m_coedgeRight);
  m_coedgeRightBot = reApi.GetNext     (m_coedgeRight);
  //
  if ( m_coedgeLeftTop.IsNull() )
  {
    m_progress.SendLogMessage( LogWarn(Normal) << "Incomplete topology: there is no left-top coedge." );
    return false; // Incomplete topology.
  }
  //
  if ( m_coedgeLeftBot.IsNull() )
  {
    m_progress.SendLogMessage( LogWarn(Normal) << "Incomplete topology: there is no left-bottom coedge." );
    return false; // Incomplete topology.
  }
  //
  if ( m_coedgeRightTop.IsNull() )
  {
    m_progress.SendLogMessage( LogWarn(Normal) << "Incomplete topology: there is no right-top coedge." );
    return false; // Incomplete topology.
  }
  //
  if ( m_coedgeRightBot.IsNull() )
  {
    m_progress.SendLogMessage( LogWarn(Normal) << "Incomplete topology: there is no right-bottom coedge." );
    return false; // Incomplete topology.
  }

  return true;
}
