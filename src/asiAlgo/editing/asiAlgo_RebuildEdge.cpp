//-----------------------------------------------------------------------------
// Created on: 14 May (*) 2018
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
#include <asiAlgo_RebuildEdge.h>

// asiAlgo includes
#include <asiAlgo_ModConstructEdge.h>

// OCCT includes
#include <BRepTools_Modifier.hxx>

//-----------------------------------------------------------------------------

asiAlgo_RebuildEdge::asiAlgo_RebuildEdge(const TopoDS_Shape&  masterCAD,
                                         ActAPI_ProgressEntry progress,
                                         ActAPI_PlotterEntry  plotter)
//
: ActAPI_IAlgorithm(progress, plotter)
{
  m_aag = new asiAlgo_AAG(masterCAD, true);
}

//-----------------------------------------------------------------------------

asiAlgo_RebuildEdge::asiAlgo_RebuildEdge(const Handle(asiAlgo_AAG)& aag,
                                         ActAPI_ProgressEntry       progress,
                                         ActAPI_PlotterEntry        plotter)
: ActAPI_IAlgorithm(progress, plotter), m_aag(aag)
{}

//-----------------------------------------------------------------------------

bool asiAlgo_RebuildEdge::Perform(const TopoDS_Edge& edge)
{
  /* ===================
   *  Preparation stage
   * =================== */

  // Prepare history.
  if ( m_history.IsNull() )
    m_history = new asiAlgo_History;

  // Get part shape.
  const TopoDS_Shape& partShape = m_aag->GetMasterCAD();

  /* ==========================
   *  Apply geometric operator
   * ========================== */

  // Prepare Modification.
  Handle(asiAlgo_ModConstructEdge)
    Mod = new asiAlgo_ModConstructEdge(m_aag,
                                       m_progress,
                                       m_plotter);

  // Initialize Modification.
  if ( !Mod->Init(edge) )
    return false;

  // Initialize Modifier.
  BRepTools_Modifier Modifier;
  Modifier.Init(partShape);

  // Perform Modification.
  Modifier.Perform(Mod);

  // Check.
  if ( !Modifier.IsDone() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "BRepTools_Modifier is not done.");
    return false;
  }

  /* ==========
   *  Finalize
   * ========== */

  // Get result.
  m_result = Modifier.ModifiedShape(partShape);

  // Populate history.
  {
    const asiAlgo_ModEdgeInfo& edgeInfo = Mod->GetEdgeInfo();

    const TopoDS_Shape&
      e_new = Modifier.ModifiedShape(edgeInfo.situation.e);

    const TopoDS_Shape&
      e_1_next_new = Modifier.ModifiedShape(edgeInfo.situation.e_1_next);

    const TopoDS_Shape&
      e_1_prev_new = Modifier.ModifiedShape(edgeInfo.situation.e_1_prev);

    const TopoDS_Shape&
      e_2_next_new = Modifier.ModifiedShape(edgeInfo.situation.e_2_next);

    const TopoDS_Shape&
      e_2_prev_new = Modifier.ModifiedShape(edgeInfo.situation.e_2_prev);

    const TopoDS_Shape&
      f_1_new = Modifier.ModifiedShape(edgeInfo.situation.f_1);

    const TopoDS_Shape&
      f_2_new = Modifier.ModifiedShape(edgeInfo.situation.f_2);

    const TopoDS_Shape&
      f_first_new = Modifier.ModifiedShape(edgeInfo.situation.f_first);

    const TopoDS_Shape&
      f_last_new = Modifier.ModifiedShape(edgeInfo.situation.f_last);

    const TopoDS_Shape&
      v_first_new = Modifier.ModifiedShape(edgeInfo.situation.v_first);

    const TopoDS_Shape&
      v_last_new = Modifier.ModifiedShape(edgeInfo.situation.v_last);

    // Populate history.
    m_history->AddModified(edgeInfo.situation.e,        e_new);
    m_history->AddModified(edgeInfo.situation.e_1_next, e_1_next_new);
    m_history->AddModified(edgeInfo.situation.e_1_prev, e_1_prev_new);
    m_history->AddModified(edgeInfo.situation.e_2_next, e_2_next_new);
    m_history->AddModified(edgeInfo.situation.e_2_prev, e_2_prev_new);
    m_history->AddModified(edgeInfo.situation.f_1,      f_1_new);
    m_history->AddModified(edgeInfo.situation.f_2,      f_2_new);
    m_history->AddModified(edgeInfo.situation.f_first,  f_first_new);
    m_history->AddModified(edgeInfo.situation.f_last,   f_last_new);
    m_history->AddModified(edgeInfo.situation.v_first,  v_first_new);
    m_history->AddModified(edgeInfo.situation.v_last,   v_last_new);
  }

  return true; // Success.
}
