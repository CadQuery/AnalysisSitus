//-----------------------------------------------------------------------------
// Created on: 15 December 2017
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
#include <asiAlgo_EulerKEF.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiAlgo_EulerKEF::asiAlgo_EulerKEF(const TopoDS_Shape&  masterCAD,
                                   const TopoDS_Face&   face,
                                   const TopoDS_Edge&   edge2Kill,
                                   const TopoDS_Edge&   edge2Survive,
                                   ActAPI_ProgressEntry progress,
                                   ActAPI_PlotterEntry  plotter)
//
: asiAlgo_Euler  (masterCAD, progress, plotter),
  m_face         (face),
  m_edge2Kill    (edge2Kill),
  m_edge2Survive (edge2Survive)
{}

//-----------------------------------------------------------------------------

asiAlgo_EulerKEF::asiAlgo_EulerKEF(const TopoDS_Shape&  masterCAD,
                                   const TopoDS_Face&   face,
                                   const TopoDS_Edge&   edge,
                                   ActAPI_ProgressEntry progress,
                                   ActAPI_PlotterEntry  plotter)
//
: asiAlgo_Euler  (masterCAD, progress, plotter),
  m_face         (face),
  m_edge2Kill    (edge)
{}

//-----------------------------------------------------------------------------

bool asiAlgo_EulerKEF::perform(const bool doApply)
{
  /* ======================================
   *  Prepare tools from the "engine room"
   * ====================================== */

  // Prepare history.
  if ( m_history.IsNull() )
    m_history = new asiAlgo_History;

  // Prepare killer.
  if ( m_killer.IsNull() )
  {
    m_killer = new asiAlgo_TopoKill(m_master, m_progress, m_plotter);
    m_killer->SetHistory(m_history);
  }

  /* =============================
   *  Define modification request
   * ============================= */

  // Set modification request.
  if ( !m_killer->AskRemove(m_face) )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Request on face removal rejected.");
    return false;
  }

  // Merge edges if an edge-to-survive is defined.
  if ( !m_edge2Survive.IsNull() )
  {
    if ( !m_killer->AskReplace(m_edge2Kill, m_edge2Survive) )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "Request on edge merging rejected.");
      return false;
    }
  }
  else
  {
    if ( !m_killer->AskRemove(m_edge2Kill) )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "Request on edge removal rejected.");
      return false;
    }
  }

  /* =================================
   *  Apply modification and finalize
   * ================================= */

  if ( doApply )
  {
    if ( !m_killer->Apply() )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "Topological killer failed.");
      return false;
    }

    // Get result.
    m_result = m_killer->GetResult();
  }

  return true; // Success.
}
