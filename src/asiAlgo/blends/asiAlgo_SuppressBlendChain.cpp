//-----------------------------------------------------------------------------
// Created on: 17 October 2018
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
#include <asiAlgo_SuppressBlendChain.h>

// asiAlgo includes
#include <asiAlgo_AAGIterator.h>
#include <asiAlgo_AttrBlendCandidate.h>
#include <asiAlgo_BlendTopoConditionFFIsolated.h>
#include <asiAlgo_RebuildEdge.h>
#include <asiAlgo_Timer.h>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

namespace asiAlgo_AAGIterationRule
{
  //! This rule prevents further iteration starting from faces which are
  //! not recognized as blend candidate faces.
  class AllowBlendCandidates : public Standard_Transient
  {
  public:

    // OCCT RTTI
    DEFINE_STANDARD_RTTI_INLINE(AllowBlendCandidates, Standard_Transient)

  public:

    //! Ctor.
    //! \param[in] aag      attributed adjacency graph keeping information on the
    //!                     recognized properties of the model.
    //! \param[in] progress progress notifier.
    //! \param[in] plottter imperative plotter.
    AllowBlendCandidates(const Handle(asiAlgo_AAG)& aag,
                         ActAPI_ProgressEntry       progress,
                         ActAPI_PlotterEntry        plotter)
    : m_aag             (aag),
      m_bAllInitialized (true),
      m_progress        (progress),
      m_plotter         (plotter)
    {}

  public:

    //! For the given face ID, this method decides whether to check its
    //! neighbors or stop.
    //! \param[in] fid 1-based ID of the face in question.
    //! \return true if the face in question is a gatekeeper for further iteration.
    bool IsBlocking(const int fid)
    {
      // If there are some nodal attributes for this face, we check whether
      // it has already been recognized as a blend candidate.
      if ( m_aag->HasNodeAttributes(fid) )
      {
        Handle(asiAlgo_FeatureAttr)
          attr = m_aag->GetNodeAttribute( fid, asiAlgo_AttrBlendCandidate::GUID() );
        //
        if ( !attr.IsNull() )
        {
          Handle(asiAlgo_AttrBlendCandidate)
            bcAttr = Handle(asiAlgo_AttrBlendCandidate)::DownCast(attr);

          // Attempt to identify a topological condition.
          Handle(asiAlgo_BlendTopoConditionFFIsolated)
            cond = new asiAlgo_BlendTopoConditionFFIsolated(m_aag, m_progress, m_plotter);
          //
          if ( cond->Initialize(bcAttr) )
          {
            cond->Dump(m_plotter);
            //
            m_faceInfo.Bind(fid, cond);

            return false; // Allow further iteration.
          }
          else if ( m_bAllInitialized ) m_bAllInitialized = false;
        }
      }

      return true; // Prohibit further iteration.
    }

    //! \return recognized topological conditions.
    const NCollection_DataMap<int, Handle(asiAlgo_BlendTopoCondition)>& GetConditions() const
    {
      return m_faceInfo;
    }

    //! \return true if all topological conditions were initialized
    //!         successfully, false -- otherwise.
    bool AreAllInitialized() const
    {
      return m_bAllInitialized;
    }

  protected:

    //! AAG instance.
    Handle(asiAlgo_AAG) m_aag;

    //! This map stores for each face its topological condition.
    NCollection_DataMap<int, Handle(asiAlgo_BlendTopoCondition)> m_faceInfo;

    //! Indicates whether all topological conditions were identified or not.
    bool m_bAllInitialized;

  protected:

    ActAPI_ProgressEntry m_progress; //!< Progress notifier.
    ActAPI_PlotterEntry  m_plotter;  //!< Imperative plotter.

  };
};

//-----------------------------------------------------------------------------

asiAlgo_SuppressBlendChain::asiAlgo_SuppressBlendChain(const Handle(asiAlgo_AAG)& aag,
                                                       ActAPI_ProgressEntry       progress,
                                                       ActAPI_PlotterEntry        plotter)
//
: ActAPI_IAlgorithm (progress, plotter),
  m_aag             (aag)
{}

//-----------------------------------------------------------------------------

bool asiAlgo_SuppressBlendChain::Perform(const int faceId)
{
  /* ===================================
   *  Initialize topological conditions
   * =================================== */

  TIMER_NEW
  TIMER_GO

  // Propagation rule.
  Handle(asiAlgo_AAGIterationRule::AllowBlendCandidates)
    itRule = new asiAlgo_AAGIterationRule::AllowBlendCandidates(m_aag,
                                                                m_progress,
                                                                m_plotter);

  // Prepare neighborhood iterator with customized propagation rule. The idea
  // of the following loop is to initialize all topological conditions before
  // suppression.
  int numIteratedFaces = 0;
  //
  asiAlgo_AAGNeighborsIterator<asiAlgo_AAGIterationRule::AllowBlendCandidates>
    nit(m_aag, faceId, itRule);
  //
  while ( nit.More() )
  {
    nit.Next();
    ++numIteratedFaces;
  }

  // Check if suppression is possible.
  if ( !itRule->AreAllInitialized() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Some non-identified topological conditions remain.");
    return false;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(m_progress, "Initialize topo conditions")

  /* ================================
   *  Perform topological operations
   * ================================ */

  TIMER_RESET
  TIMER_GO

  // Input shape.
  TopoDS_Shape targetShape = m_aag->GetMasterCAD();

  // Affected edges to rebuild after topological reduction.
  asiAlgo_Edges2Rebuild edges2Rebuild;

  // Process the initialized topological conditions.
  const NCollection_DataMap<int, Handle(asiAlgo_BlendTopoCondition)>&
    conditions = itRule->GetConditions();
  //
  for ( NCollection_DataMap<int, Handle(asiAlgo_BlendTopoCondition)>::Iterator cit(conditions);
        cit.More(); cit.Next() )
  {
    const int                                 blendFaceId = cit.Key();
    const Handle(asiAlgo_BlendTopoCondition)& cond        = cit.Value();

    Handle(asiAlgo_History) history;
    //
    if ( !cond->Suppress(targetShape, targetShape, history) )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "Cannot suppress topological condition for blend face %1."
                                               << blendFaceId);
      return false; // No sense to go further if we already fail here.
    }

    // Update all topological conditions.
    this->updateTopoConditions(conditions, blendFaceId, history);

    // Add edges to rebuild.
    cond->GatherAffectedEdges(edges2Rebuild);
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(m_progress, "Topological reduction")

  /* ==============================
   *  Perform geometric operations
   * ============================== */

  TIMER_RESET
  TIMER_GO

  Handle(asiAlgo_History) rebuildHistory = new asiAlgo_History;

  // Normalize geometry.
  for ( int k = 1; k <= edges2Rebuild.Extent(); ++k )
  {
    const TopoDS_Edge& originalEdge = edges2Rebuild(k).edge;

    // Update from history as the edge could have been modified by rebuilder
    // on previous iterations.
    TopoDS_Edge
      edge2Rebuild = TopoDS::Edge( rebuildHistory->GetLastModifiedOrArg(originalEdge).Oriented(TopAbs_EXTERNAL) );

    m_plotter.DRAW_SHAPE(edge2Rebuild, Color_Red, 1.0, true, "edge2Rebuild");

    // Prepare algorithm.
    asiAlgo_RebuildEdge rebuildEdge(targetShape, m_progress, m_plotter);
    //
    rebuildEdge.SetHistory(rebuildHistory);
    rebuildEdge.SetFrozenVertices(edges2Rebuild(k).frozenVertices);

    // Apply geometric operator.
    if ( !rebuildEdge.Perform(edge2Rebuild) )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "Cannot rebuild edge.");
      return false;
    }

    // Update working part.
    targetShape = rebuildEdge.GetResult();
  }

  // Set output.
  m_result = targetShape;

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(m_progress, "Rebuild edges")

  return true;
}

//-----------------------------------------------------------------------------

void
  asiAlgo_SuppressBlendChain::updateTopoConditions(const NCollection_DataMap<int, Handle(asiAlgo_BlendTopoCondition)>& conditions,
                                                   const int                                                           toSkip,
                                                   const Handle(asiAlgo_History)&                                      history) const
{
  for ( NCollection_DataMap<int, Handle(asiAlgo_BlendTopoCondition)>::Iterator cit(conditions);
        cit.More(); cit.Next() )
  {
    const int fid = cit.Key();
    //
    if ( fid == toSkip )
      continue;

    // Actualize condition.
    const Handle(asiAlgo_BlendTopoCondition)& cond = cit.Value();
    //
    cond->Actualize(history);
  }
}
