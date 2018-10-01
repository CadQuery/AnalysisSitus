//-----------------------------------------------------------------------------
// Created on: 01 October 2018
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
#include <asiAlgo_RecognizeBlends.h>

// asiAlgo includes
#include <asiAlgo_FindBlendFaces.h>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiAlgo_RecognizeBlends::asiAlgo_RecognizeBlends(const TopoDS_Shape&  masterCAD,
                                                 ActAPI_ProgressEntry progress,
                                                 ActAPI_PlotterEntry  plotter)
//
: asiAlgo_Recognizer(masterCAD, NULL, progress, plotter)
{}

//-----------------------------------------------------------------------------

asiAlgo_RecognizeBlends::asiAlgo_RecognizeBlends(const TopoDS_Shape&        masterCAD,
                                                 const Handle(asiAlgo_AAG)& aag,
                                                 ActAPI_ProgressEntry       progress,
                                                 ActAPI_PlotterEntry        plotter)
//
: asiAlgo_Recognizer(masterCAD, aag, progress, plotter)
{}

//-----------------------------------------------------------------------------

bool asiAlgo_RecognizeBlends::Perform(const double radius)
{
  /* ====================
   *  Stage 1: build AAG
   * ==================== */

  // Build master AAG if necessary.
  if ( m_aag.IsNull() )
  {
#if defined COUT_DEBUG
    TIMER_NEW
    TIMER_GO
#endif

    // We do not allow smooth transitions here.
    m_aag = new asiAlgo_AAG(m_master, false);

#if defined COUT_DEBUG
    TIMER_FINISH
    TIMER_COUT_RESULT_MSG("Construct AAG")
#endif
  }

  /* ==========================
   *  Stage 2: attribute faces
   * ========================== */

  // Find candidate faces in AAG.
  asiAlgo_FindBlendFaces attribution( m_master,
                                      this->Progress(),
                                      this->Plotter() );
  attribution.SetAAG(m_aag);
  attribution.SetFaces(m_faces2Analyze);
  //
  if ( !attribution.Perform() )
  {
    this->Progress().SendLogMessage( LogErr(Normal) << "Failed to attribute blend faces." );
    return false;
  }

  /* ===============================
   *  Stage 3: extract blend chains
   * =============================== */

  // Set results of attribution as final results.
  m_result.ids   = attribution.GetResultIndices();
  m_result.faces = attribution.GetResultFaces();
  //
  return true;
}
