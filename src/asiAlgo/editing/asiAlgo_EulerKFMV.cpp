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
#include <asiAlgo_EulerKFMV.h>

// asiAlgo includes
#include <asiAlgo_EulerKEV.h>

// OCCT includes
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiAlgo_EulerKFMV::asiAlgo_EulerKFMV(const TopoDS_Shape&  masterCAD,
                                     const TopoDS_Face&   face,
                                     ActAPI_ProgressEntry progress,
                                     ActAPI_PlotterEntry  plotter)
//
: asiAlgo_Euler(masterCAD, progress, plotter),
  m_face       (face)
{}

//-----------------------------------------------------------------------------

bool asiAlgo_EulerKFMV::perform(const bool doApply)
{
  // Prepare history.
  if ( m_history.IsNull() )
    m_history = new asiAlgo_History;

  /* =====================================================
   *  Perform series of KEV to kill all edges of the face
   * ===================================================== */

  TopoDS_Shape operand = m_master;
  TopoDS_Face  face    = m_face;
  bool         stop    = false;

  // Kill all edges one by one.
  do
  {
    // Take alive edges.
    TopTools_IndexedMapOfShape faceEdges;
    TopExp::MapShapes(face, TopAbs_EDGE, faceEdges);

    // If no edges remain, stop.
    if ( faceEdges.IsEmpty() )
      stop = true;
    else
    {
      const TopoDS_Edge& edge = TopoDS::Edge( faceEdges(1) );

      // KEV.
      asiAlgo_EulerKEV kev(operand, edge, m_progress, m_plotter);
      //
      kev.SetHistory(m_history);
      //
      if ( !kev.Perform(true) )
      {
        m_progress.SendLogMessage(LogErr(Normal) << "KEV failed.");
        return false;
      }

      // Update operand.
      operand = kev.GetResult();

      // Update face.
      std::vector<TopoDS_Shape> modified;
      //
      if ( !m_history->GetModified(face, modified) )
      {
        m_progress.SendLogMessage(LogErr(Normal) << "Modification on face expected.");
        return false;
      }
      //
      if ( modified.size() != 1 )
      {
        m_progress.SendLogMessage(LogErr(Normal) << "Unexpected number of face images after KEV (%1 while expected %2)."
                                                 << int( modified.size() ) << 1 );
        return false;
      }
      //
      face = TopoDS::Face(modified[0]);

      ///
      /*m_result = operand;
      return true;*/
      ///
    }
  }
  while ( !stop );

  /* ===========================================================
   *  Now kill the remaining face (without edges at this stage)
   * =========================================================== */

  // Prepare killer.
  if ( m_killer.IsNull() )
  {
    m_killer = new asiAlgo_TopoKill(operand, m_progress, m_plotter);
    m_killer->SetHistory(m_history);
  }

  // Kill face.
  if ( !m_killer->AskRemove(face) )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Request on face removal rejected.");
    return false;
  }

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
