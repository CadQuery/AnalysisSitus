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
#include <asiAlgo_FindBlendFaces.h>

// asiAlgo includes
#include <asiAlgo_AAGIterator.h>
#include <asiAlgo_AttrBlendCandidate.h>
#include <asiAlgo_FindCrossEdges.h>
#include <asiAlgo_FindSmoothEdges.h>
#include <asiAlgo_FindSpringEdges.h>
#include <asiAlgo_FindTermEdges.h>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #pragma message("===== warning: DRAW_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiAlgo_FindBlendFaces::asiAlgo_FindBlendFaces(const TopoDS_Shape&  masterCAD,
                                               ActAPI_ProgressEntry progress,
                                               ActAPI_PlotterEntry  plotter)
: asiAlgo_Recognizer(masterCAD, NULL, progress, plotter)
{}

//-----------------------------------------------------------------------------

bool asiAlgo_FindBlendFaces::Perform()
{
  // Build master AAG if necessary.
  if ( m_aag.IsNull() )
  {
#if defined COUT_DEBUG
    TIMER_NEW
    TIMER_GO
#endif

    m_aag = new asiAlgo_AAG(m_master);

#if defined COUT_DEBUG
    TIMER_FINISH
    TIMER_COUT_RESULT_MSG("Construct AAG")
#endif
  }

  ActAPI_PlotterEntry IV( this->Plotter() );

  // Prepare tool to find smooth edges.
  asiAlgo_FindSmoothEdges
    findSmoothEdges( m_master,
                     m_aag,
                     this->Progress(),
                     this->Plotter() );

  // Prepare tool to find spring edges.
  asiAlgo_FindSpringEdges
    findSpringEdges( m_master,
                     m_aag,
                     this->Progress(),
                     this->Plotter() );

  // Prepare tool to find cross edges.
  asiAlgo_FindCrossEdges
    findCrossEdges( m_master,
                    m_aag,
                    this->Progress(),
                    this->Plotter() );

  // Prepare tool to find terminating edges.
  asiAlgo_FindTermEdges
    findTermEdges( m_master,
                   m_aag,
                   this->Progress(),
                   this->Plotter() );

  const bool onlySelectedFaces = !m_faces2Analyze.IsEmpty();

  // Loop over the faces.
  Handle(asiAlgo_AAGRandomIterator) it = new asiAlgo_AAGRandomIterator(m_aag);
  //
  for ( ; it->More(); it->Next() )
  {
    const int          current_face_idx = it->GetFaceId();
    const TopoDS_Face& current_face     = m_aag->GetFace(current_face_idx);

    // Skip non-selected faces if partial recognition is requested.
    if ( onlySelectedFaces && !m_faces2Analyze.Contains(current_face_idx) )
      continue;

    /* =========================================
     *  STAGE 1: Identify smooth edges which is
     *           the first clue for recognition
     * ========================================= */

    // Clean up the result.
    findSmoothEdges.ClearResult();

    // Detect smooth edges for a face.
    if ( !findSmoothEdges.PerformForFace(current_face_idx) )
    {
      this->Progress().SendLogMessage( LogErr(Normal) << "Cannot find smooth edges." );
      return false;
    }

    const TopTools_IndexedMapOfShape& smoothEdges       = findSmoothEdges.GetResultEdges();
    const TColStd_PackedMapOfInteger& smoothEdgeIndices = findSmoothEdges.GetResultIndices();
    //
    if ( smoothEdgeIndices.IsEmpty() )
      continue;

#if defined DRAW_DEBUG
    for ( int eidx = 1; eidx <= detect_smooth_edges.GetResultEdges().Extent(); ++eidx )
    {
      IV.DRAW_SHAPE( detect_smooth_edges.GetResultEdges()(eidx), Color_Blue );
    }
#endif

    /* ================================
     *  STAGE 2: Identify spring edges
     * ================================ */

    // Clean up the result.
    findSpringEdges.ClearResult();

    // Detect spring edges.
    bool   isCandidateBlend;
    double candidateRadius;
    //
    if ( !findSpringEdges.PerformForFace(current_face_idx,
                                         smoothEdgeIndices,
                                         isCandidateBlend,
                                         candidateRadius) )
    {
      this->Progress().SendLogMessage( LogErr(Normal) << "Cannot find spring edges." );
      return false;
    }

    if ( !isCandidateBlend )
      continue;

    // Prepare face attribute.
    Handle(asiAlgo_AttrBlendCandidate)
      BlendAttr = new asiAlgo_AttrBlendCandidate(0);
    //
    if ( !m_aag->SetNodeAttribute(current_face_idx, BlendAttr) )
    {
      this->Progress().SendLogMessage( LogErr(Normal) << "Weird iteration: blend attribute is already there." );
      return false;
    }

    const TopTools_IndexedMapOfShape& springEdges       = findSpringEdges.GetResultEdges();
    const TColStd_PackedMapOfInteger& springEdgeIndices = findSpringEdges.GetResultIndices();

    // Get number of spring edges and decide the blend type.
    const int nSpringEdges = springEdges.Extent();
    //
    if ( nSpringEdges == 2 )
      BlendAttr->Kind = BlendType_Ordinary;
    else if ( nSpringEdges == 1)
      BlendAttr->Kind = BlendType_Cliff;
    else
      continue;

    IV.DRAW_SHAPE(current_face, Color_Blue, "Candidate blend after spring edge detection");
    //
    for ( int eidx = 1; eidx <= nSpringEdges; ++eidx )
    {
      IV.DRAW_SHAPE( springEdges(eidx), Color_Green, 1.0, true, "Spring edge" );
    }

    // Populate blend candidate attribute with smooth and spring edges.
    BlendAttr->SmoothEdgeIndices = smoothEdgeIndices;
    BlendAttr->SpringEdgeIndices = springEdgeIndices;

    /* ===============================
     *  STAGE 3: Identify cross edges
     * =============================== */

    findCrossEdges.ClearResult();

    // Detect cross edges.
    if ( !findCrossEdges.PerformForFace( current_face_idx,
                                         smoothEdges,
                                         springEdges,
                                         candidateRadius ) )
    {
      this->Progress().SendLogMessage( LogErr(Normal) << "Cannot find cross edges." );
      return false;
    }

    const TopTools_IndexedMapOfShape& crossEdges       = findCrossEdges.GetResultEdges();
    const TColStd_PackedMapOfInteger& crossEdgeIndices = findCrossEdges.GetResultIndices();

    // Get number of cross edges and precise the blend type.
    const int nCrossEdges = crossEdges.Extent();
    //
    if ( nCrossEdges == 1 || nCrossEdges == 2 )
    {
      BlendAttr->Confirmed = true;

      // Push to result.
      m_result.ids.Add(current_face_idx);
      m_result.faces.Add(current_face);
    }

    for ( int eidx = 1; eidx <= nCrossEdges; ++eidx )
    {
      IV.DRAW_SHAPE( crossEdges(eidx), Color_Red, 1.0, true, "Cross edge" );
    }

    // Populate blend candidate attribute with cross edges.
    BlendAttr->CrossEdgeIndices = crossEdgeIndices;

    /* =====================================
     *  STAGE 4: Identify terminating edges
     * ===================================== */

    findTermEdges.ClearResult();

    // Detect terminating edges.
    if ( !findTermEdges.PerformForFace(current_face_idx,
                                       smoothEdges,
                                       springEdges,
                                       crossEdges,
                                       candidateRadius) )
    {
      this->Progress().SendLogMessage( LogErr(Normal) << "Cannot find cross edges." );
      return false;
    }

    const TopTools_IndexedMapOfShape& terminatingEdges       = findTermEdges.GetResultEdges();
    const TColStd_PackedMapOfInteger& terminatingEdgeIndices = findTermEdges.GetResultIndices();

    // Get number of terminating edges to precise the blend type.
    const int nTerminatingEdges = terminatingEdges.Extent();
    //
    if ( nTerminatingEdges == 2 )
    {
      BlendAttr->Confirmed = true;

      // Push to result.
      m_result.ids.Add(current_face_idx);
      m_result.faces.Add(current_face);
    }

    for ( int eidx = 1; eidx <= nTerminatingEdges; ++eidx )
    {
      IV.DRAW_SHAPE( terminatingEdges(eidx), Color_Magenta, 1.0, true, "Terminating edge" );
    }

    // Populate blend candidate attribute with terminating edges.
    BlendAttr->TerminatingEdgeIndices = terminatingEdgeIndices;
  }

  return true;
}
