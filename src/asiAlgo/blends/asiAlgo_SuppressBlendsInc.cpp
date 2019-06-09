//-----------------------------------------------------------------------------
// Created on: 09 June 2019
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
#include <asiAlgo_SuppressBlendsInc.h>

// asiAlgo includes
#include <asiAlgo_AAG.h>
#include <asiAlgo_RecognizeBlends.h>
#include <asiAlgo_SuppressBlendChain.h>

// OCCT includes
#include <BRepBuilderAPI_Copy.hxx>

//-----------------------------------------------------------------------------

asiAlgo_SuppressBlendsInc::asiAlgo_SuppressBlendsInc(ActAPI_ProgressEntry progress,
                                                     ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter)
{}

//-----------------------------------------------------------------------------

bool asiAlgo_SuppressBlendsInc::Perform(const Handle(asiAlgo_AAG)& aag,
                                        const double               radius,
                                        TopoDS_Shape&              result,
                                        Handle(asiAlgo_History)&   history,
                                        int&                       numSuppressedChains) const
{
  m_progress.SetMessageKey("Incremental blend suppression");
  m_progress.Init();

  // Prepare a history instance to merge histories incrementally.
  if ( history.IsNull() )
    history = new asiAlgo_History(m_progress, m_plotter);

  // Initialize outputs.
  result              = aag->GetMasterCAD();
  numSuppressedChains = 0;

  TColStd_PackedMapOfInteger fids;

  // Faces to skip as already tried and known to be non-suppressible. Here
  // we use transient pointers to avoid any confusion with renumbering.
  TopTools_IndexedMapOfShape nonSuppressibleFaces;

  // Perform main loop for incremental suppression.
  bool                recognize = true;
  bool                stop      = false;
  Handle(asiAlgo_AAG) tempAAG   = aag;
  //
  do
  {
    if ( recognize )
    {
      recognize = false;

      // Perform recognition starting from the guess face.
      asiAlgo_RecognizeBlends recognizer( tempAAG,
                                          m_progress,
                                          m_plotter );
      //
      if ( !recognizer.Perform(radius) )
      {
        m_progress.SendLogMessage(LogWarn(Normal) << "Recognition failed.");
        m_progress.SetProgressStatus(ActAPI_ProgressStatus::Progress_Failed);
        return false;
      }
      //
      fids = recognizer.GetResultIndices();
    }

    if ( m_progress.IsCancelling() )
    {
      m_progress.SetProgressStatus(ActAPI_ProgressStatus::Progress_Cancelled);
      return false;
    }

    // Update progress message.
    TCollection_AsciiString msg("Num. faces remaining: ");
    msg += fids.Extent();
    //
    m_progress.SetMessageKey(msg);

    if ( !fids.Extent() )
    {
      m_progress.SendLogMessage(LogInfo(Normal) << "No faces remaining for suppression.");
      stop = true;
      continue;
    }

    // Choose any face for suppression.
    const int fid = fids.GetMaximalMapped();
    //
    if ( nonSuppressibleFaces.Contains( tempAAG->GetFace(fid) ) )
    {
      m_progress.SendLogMessage(LogWarn(Normal) << "Skip non-suppressible face. Keep going...");

      recognize = false; // Try next face.
      fids.Remove( fid );
      continue;
    }
    //
    m_progress.SendLogMessage(LogWarn(Normal) << "Next face to suppress: %1." << fid);

    // Prepare tool.
    asiAlgo_SuppressBlendChain suppressor(tempAAG, m_progress, m_plotter);
    //
    if ( !suppressor.Perform(fid) )
    {
      m_progress.SendLogMessage(LogWarn(Normal) << "Next face suppression failed. Keep going...");

      recognize = false; // Try next face.
      fids.Subtract( suppressor.GetChainIds() );
      fids.Remove( fid );

      // Add non-suppressible faces.
      TopTools_IndexedMapOfShape lastChainFaces = suppressor.GetChainFaces();
      //
      nonSuppressibleFaces.Add( tempAAG->GetFace(fid) );
      for ( int k = 1; k <= lastChainFaces.Extent(); ++k )
        nonSuppressibleFaces.Add( lastChainFaces(k) );

      continue;
    }

    if ( suppressor.GetNumSuppressedChains() == 0 )
    {
      stop = true;
      continue;
    }

    numSuppressedChains += suppressor.GetNumSuppressedChains();

    // Get result.
    const TopoDS_Shape& incRes = suppressor.GetResult();
    //
    result = incRes;

    // Graphical dump.
    if ( !m_plotter.Plotter().IsNull() )
    {
      TCollection_AsciiString name("incRes_fid_"); name += fid;
      m_plotter.REDRAW_SHAPE( name, BRepBuilderAPI_Copy(incRes) );
    }

    // Merge history.
    history->Concatenate( suppressor.GetHistory() );

    // Adjust the collection of remaining faces.
    fids.Subtract( suppressor.GetChainIds() );
    fids.Remove( fid );
    //
    if ( fids.IsEmpty() )
    {
      stop = true;
      continue;
    }

    // Update AAG.
    tempAAG = new asiAlgo_AAG(incRes, false);
    recognize = true;
  }
  while ( !stop );

  m_progress.SetProgressStatus(ActAPI_ProgressStatus::Progress_Succeeded);
  return true;
}
