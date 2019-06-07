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
#include <asiAlgo_SuppressBlendChain.h>

// asiAlgo includes
#include <asiAlgo_AAGIterator.h>
#include <asiAlgo_AttrBlendCandidate.h>
#include <asiAlgo_BlendTopoConditionFFIsolated.h>
#include <asiAlgo_BlendTopoConditionFFOrdinaryEBF.h>
#include <asiAlgo_BlendTopoConditionFFOrdinaryVBF.h>
#include <asiAlgo_CheckValidity.h>
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
    //! \param[in] aag        attributed adjacency graph keeping information on the
    //!                       recognized properties of the model.
    //! \param[in] conditions collection of topological condition. This collection
    //!                       can be null at initialization stage. Alternatively,
    //!                       you may pass an existing collection here, so the
    //!                       rule will not allocate a new one but will add new
    //!                       data to the passed one.
    //! \param[in] progress   progress notifier.
    //! \param[in] plottter   imperative plotter.
    AllowBlendCandidates(const Handle(asiAlgo_AAG)&                    aag,
                         const Handle(asiAlgo_HBlendTopoConditionMap)& conditions,
                         ActAPI_ProgressEntry                          progress,
                         ActAPI_PlotterEntry                           plotter)
    //
    : m_aag             (aag),
      m_faceInfo        (conditions),
      m_bAllInitialized (true),
      m_progress        (progress),
      m_plotter         (plotter)
    {
      if ( m_faceInfo.IsNull() )
        m_faceInfo = new asiAlgo_HBlendTopoConditionMap;
    }

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
        // Skip already visited nodes.
        if ( m_faceInfo->IsBound(fid) )
          return false;

        // Check blend candidates.
        Handle(asiAlgo_FeatureAttr)
          attr = m_aag->GetNodeAttribute( fid, asiAlgo_AttrBlendCandidate::GUID() );
        //
        if ( !attr.IsNull() )
        {
          Handle(asiAlgo_AttrBlendCandidate)
            bcAttr = Handle(asiAlgo_AttrBlendCandidate)::DownCast(attr);
          //
          if ( bcAttr->Kind == BlendType_Uncertain )
          {
            m_problematicFaces.Add(fid);
            m_bAllInitialized = false;
            return true; // Block further iterations.
          }

          // Prepare topological conditions.
          Handle(asiAlgo_BlendTopoConditionFFIsolated)
            cond1 = new asiAlgo_BlendTopoConditionFFIsolated(m_aag, m_progress, m_plotter);
          //
          Handle(asiAlgo_BlendTopoConditionFFOrdinaryEBF)
            cond2 = new asiAlgo_BlendTopoConditionFFOrdinaryEBF(m_aag, m_progress, m_plotter);
          //
          Handle(asiAlgo_BlendTopoConditionFFOrdinaryVBF)
            cond3 = new asiAlgo_BlendTopoConditionFFOrdinaryVBF(m_aag, m_progress, m_plotter);

          // Attempt to identify a topological condition.
          if ( cond1->Initialize(bcAttr) )
          {
            m_progress.SendLogMessage( LogNotice(Normal) << "Confirmed condition: %1."
                                                         << cond1->DynamicType()->Name() );

            cond1->Dump(m_plotter);
            //
            m_faceInfo->Bind(fid, cond1);

            return false; // Allow further iteration.
          }
          else if ( cond2->Initialize(bcAttr) )
          {
            m_progress.SendLogMessage( LogNotice(Normal) << "Confirmed condition: %1."
                                                         << cond2->DynamicType()->Name() );

            cond2->Dump(m_plotter);
            //
            m_faceInfo->Bind(fid, cond2);

            return false; // Allow further iteration.
          }
          else if ( cond3->Initialize(bcAttr) )
          {
            m_progress.SendLogMessage( LogNotice(Normal) << "Confirmed condition: %1."
                                                         << cond3->DynamicType()->Name() );

            cond3->Dump(m_plotter);
            //
            m_faceInfo->Bind(fid, cond3);

            return false; // Allow further iteration.
          }
          //
          else if ( m_bAllInitialized )
          {
            m_problematicFaces.Add(fid);
            m_bAllInitialized = false;
          }
        }
      }

      return true; // Prohibit further iteration.
    }

    //! \return recognized topological conditions.
    const Handle(asiAlgo_HBlendTopoConditionMap)& GetConditions() const
    {
      return m_faceInfo;
    }

    //! \return true if all topological conditions were initialized
    //!         successfully, false -- otherwise.
    bool AreAllInitialized() const
    {
      return m_bAllInitialized;
    }

    //! \return IDs of the problematic faces.
    const TColStd_PackedMapOfInteger& GetProblematicFaces() const
    {
      return m_problematicFaces;
    }

  protected:

    //! AAG instance.
    Handle(asiAlgo_AAG) m_aag;

    //! This map stores for each face its topological condition.
    Handle(asiAlgo_HBlendTopoConditionMap) m_faceInfo;

    //! Indicates whether all topological conditions were identified or not.
    bool m_bAllInitialized;

    //! IDs of the faces whose topological conditions are not initialized.
    TColStd_PackedMapOfInteger m_problematicFaces;

  protected:

    ActAPI_ProgressEntry m_progress; //!< Progress notifier.
    ActAPI_PlotterEntry  m_plotter;  //!< Imperative plotter.

  };
};

//-----------------------------------------------------------------------------

asiAlgo_SuppressBlendChain::asiAlgo_SuppressBlendChain(const Handle(asiAlgo_AAG)&     aag,
                                                       const Handle(asiAlgo_History)& history,
                                                       ActAPI_ProgressEntry           progress,
                                                       ActAPI_PlotterEntry            plotter)
//
: ActAPI_IAlgorithm   (progress, plotter),
  m_aag               (aag),
  m_history           (history),
  m_iSuppressedChains (0)
{}

//-----------------------------------------------------------------------------

asiAlgo_SuppressBlendChain::asiAlgo_SuppressBlendChain(const Handle(asiAlgo_AAG)& aag,
                                                       ActAPI_ProgressEntry       progress,
                                                       ActAPI_PlotterEntry        plotter)
//
: ActAPI_IAlgorithm   (progress, plotter),
  m_aag               (aag),
  m_iSuppressedChains (0)
{
  m_history = new asiAlgo_History;
}

//-----------------------------------------------------------------------------

bool asiAlgo_SuppressBlendChain::Perform(const int faceId)
{
  m_iSuppressedChains = 0;

  /* ===================================
   *  Initialize topological conditions
   * =================================== */

  TIMER_NEW
  TIMER_GO

  // Identify all topological conditions.
  if ( !this->initTopoConditions(faceId) )
    return false;

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(m_progress, "Initialize topo conditions")

  /* =======================================================
   *  Check if the chain in question is suppressible or not
   * ======================================================= */

  TIMER_RESET
  TIMER_GO

   if ( !this->checkChainSuppressible() )
     return false;

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(m_progress, "Check if chain is suppressible")

  /* ================================
   *  Perform topological operations
   * ================================ */

  TIMER_RESET
  TIMER_GO

  TopoDS_Shape targetShape;

  if ( !this->performTopoOperations(targetShape) )
    return false;

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(m_progress, "Topological reduction")

  /* ==============================
   *  Perform geometric operations
   * ============================== */

  TIMER_RESET
  TIMER_GO

  if ( !this->performGeomOperations(targetShape) )
    return false;

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(m_progress, "Rebuild edges")

  /* =========================================================
   *  Check if the affected faces are valid after suppression
   * ========================================================= */

  TIMER_RESET
  TIMER_GO

  std::vector<asiAlgo_History::t_item*> leafItems;
  m_history->GetLeafs(leafItems, TopAbs_FACE);

  // Check faces.
  for ( size_t k = 0; k < leafItems.size(); ++k )
  {
    // Skip deleted blends.
    if ( leafItems[k]->IsDeleted )
      continue;

    // Check alive faces.
    const TopoDS_Face& affectedFace = TopoDS::Face(leafItems[k]->TransientPtr);
    //
    if ( !this->isValidFace(affectedFace) )
    {
      m_plotter.REDRAW_SHAPE("affectedFace_INVALID", affectedFace, Color_Red);
      m_progress.SendLogMessage(LogWarn(Normal) << "One of the affected faces becomes invalid "
                                                   "after processing. Abort is required.");
      return false;
    }
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(m_progress, "Check validity of faces")

  /* ==========
   *  Finalize
   * ========== */

  // Set output.
  m_result            = targetShape;
  m_iSuppressedChains = 1;

  // Set history.
  m_history->AddModified(m_aag->GetMasterCAD(), m_result);

  return true;
}

//-----------------------------------------------------------------------------

TColStd_PackedMapOfInteger asiAlgo_SuppressBlendChain::GetChainIds() const
{
  if ( m_workflow.topoCondition.IsNull() )
    return TColStd_PackedMapOfInteger();

  TColStd_PackedMapOfInteger result;
  //
  for ( asiAlgo_HBlendTopoConditionMap::Iterator it(*m_workflow.topoCondition); it.More(); it.Next() )
  {
    const int fid = it.Key();
    result.Add(fid);
  }

  return result;
}

//-----------------------------------------------------------------------------

TopTools_IndexedMapOfShape asiAlgo_SuppressBlendChain::GetChainFaces() const
{
  TColStd_PackedMapOfInteger faceIds = this->GetChainIds();

  TopTools_IndexedMapOfShape result;
  //
  for ( TColStd_MapIteratorOfPackedMapOfInteger fit(faceIds); fit.More(); fit.Next() )
  {
    const int fid = fit.Key();
    result.Add( m_aag->GetFace(fid) );
  }

  return result;
}

//-----------------------------------------------------------------------------

bool
  asiAlgo_SuppressBlendChain::initTopoConditions(const int faceId)
{
  // Propagation rule.
  Handle(asiAlgo_AAGIterationRule::AllowBlendCandidates)
    itRule = new asiAlgo_AAGIterationRule::AllowBlendCandidates(m_aag,
                                                                m_workflow.topoCondition,
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
    int kk = 1;
    TCollection_AsciiString problemFacesStr;
    const TColStd_PackedMapOfInteger& problemFaces = itRule->GetProblematicFaces();
    //
    for ( TColStd_MapIteratorOfPackedMapOfInteger fit(problemFaces); fit.More(); fit.Next(), ++kk )
    {
      problemFacesStr += fit.Key();

      if ( kk < problemFaces.Extent() )
        problemFacesStr += " ";
    }

    m_progress.SendLogMessage(LogWarn(Normal) << "Some non-identified topological conditions "
                                                 "remain for faces with the following IDs: %1."
                                              << problemFacesStr);
    return false;
  }

  // Get identified conditions.
  m_workflow.topoCondition = itRule->GetConditions();
  return true;
}

//-----------------------------------------------------------------------------

void
  asiAlgo_SuppressBlendChain::updateTopoConditions(const int                      toSkip,
                                                   const Handle(asiAlgo_History)& history) const
{
  for ( asiAlgo_HBlendTopoConditionMap::Iterator cit(*m_workflow.topoCondition);
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

//-----------------------------------------------------------------------------

bool
  asiAlgo_SuppressBlendChain::performTopoOperations(TopoDS_Shape& resultShape)
{
  // Input shape.
  resultShape = m_aag->GetMasterCAD();

  // Process the initialized topological conditions.
  for ( asiAlgo_HBlendTopoConditionMap::Iterator cit(*m_workflow.topoCondition);
        cit.More(); cit.Next() )
  {
    const int                                 blendFaceId = cit.Key();
    const Handle(asiAlgo_BlendTopoCondition)& cond        = cit.Value();

    if ( !cond->Suppress(resultShape, resultShape, m_history) )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "Cannot suppress topological condition for blend face %1."
                                               << blendFaceId);
      return false; // No sense to go further if we already fail here.
    }

    // Update all topological conditions.
    this->updateTopoConditions(blendFaceId, m_history);

    // Add edges to rebuild.
    cond->GatherAffectedEdges(m_workflow.edges2Rebuild);

    // Update all edges to rebuild.
    this->updateEdges2Rebuild(m_history);
  }

  return true;
}

//-----------------------------------------------------------------------------

bool
  asiAlgo_SuppressBlendChain::performGeomOperations(TopoDS_Shape& targetShape)
{
  const int numEdges = m_workflow.edges2Rebuild.edges.Extent();

  m_progress.SendLogMessage(LogInfo(Normal) << "There are %1 edges to rebuild"
                                            << numEdges);

  // Normalize geometry.
  for ( int k = 1; k <= m_workflow.edges2Rebuild.edges.Extent(); ++k )
  {
    // Update from history as the edge could have been modified by rebuilder
    // on previous iterations.
    if ( k > 1 )
      this->updateEdges2Rebuild(m_history);

    const TopoDS_Edge& edge2Rebuild = m_workflow.edges2Rebuild.edges(k).edge;
    //
    if ( edge2Rebuild.IsNull() )
      continue; // Null image of the next edge to rebuild.
    //
    m_plotter.DRAW_SHAPE(edge2Rebuild, Color_Magenta, 1.0, true, "edge2Rebuild");

    // Prepare algorithm.
    asiAlgo_RebuildEdge rebuildEdge(targetShape, m_progress, m_plotter);
    //
    rebuildEdge.SetHistory(m_history);
    rebuildEdge.SetFrozenVertices(m_workflow.edges2Rebuild.edges(k).frozenVertices);

    // Apply geometric operator.
    if ( !rebuildEdge.Perform(edge2Rebuild) )
    {
      m_plotter.DRAW_SHAPE(edge2Rebuild, Color_Red, 1.0, true, "edge2Rebuild_failed");
      m_progress.SendLogMessage(LogWarn(Normal) << "Cannot rebuild edge.");
      return false;
    }

    // Update working part.
    targetShape = rebuildEdge.GetResult();
  }

  return true;
}

//-----------------------------------------------------------------------------

void asiAlgo_SuppressBlendChain::updateEdges2Rebuild(const Handle(asiAlgo_History)& history)
{
  asiAlgo_Edges2Rebuild updated;

  for ( int k = 1; k <= m_workflow.edges2Rebuild.edges.Extent(); ++k )
  {
    asiAlgo_Edge2Rebuild edgeInfo = m_workflow.edges2Rebuild.edges(k);

    // Update from history.
    edgeInfo.Actualize(history);
    //
    updated.Add(edgeInfo);
  }

  // Set the result.
  m_workflow.edges2Rebuild = updated;
}

//-----------------------------------------------------------------------------

bool asiAlgo_SuppressBlendChain::checkChainSuppressible() const
{
  /* Check if all cross-edges are referenced twice by the blend faces
     in a chain. */

  // Gather all cross edges of a chain into a single collection.
  TColStd_PackedMapOfInteger allCrossEdgeIndices;
  TColStd_PackedMapOfInteger allBlendFaceIndices;
  //
  for ( asiAlgo_HBlendTopoConditionMap::Iterator cit(*m_workflow.topoCondition);
        cit.More(); cit.Next() )
  {
    const int fid = cit.Key();

    // Get attribute.
    Handle(asiAlgo_FeatureAttr)
      attr = m_aag->GetNodeAttribute( fid, asiAlgo_AttrBlendCandidate::GUID() );
    //
    if ( !attr.IsNull() )
    {
      Handle(asiAlgo_AttrBlendCandidate)
        bcAttr = Handle(asiAlgo_AttrBlendCandidate)::DownCast(attr);

      // Collect indices of cross edges
      allCrossEdgeIndices.Unite(bcAttr->CrossEdgeIndices);
      allBlendFaceIndices.Add(fid);
    }
  }

  // Check every cross-edge.
  for ( TColStd_MapIteratorOfPackedMapOfInteger eit(allCrossEdgeIndices);
        eit.More(); eit.Next() )
  {
    const int          eid = eit.Key();
    const TopoDS_Edge& E   = TopoDS::Edge( m_aag->RequestMapOfEdges()(eid) );

    int numOccurrences = 0;
    const TopTools_ListOfShape&
      E_faces = m_aag->RequestMapOfEdgesFaces().FindFromKey(E);
    //
    for ( TopTools_ListIteratorOfListOfShape fit(E_faces); fit.More(); fit.Next() )
    {
      const TopoDS_Face& F   = TopoDS::Face( fit.Value() );
      const int          fid = m_aag->GetFaceId(F);

      if ( allBlendFaceIndices.Contains(fid) )
        ++numOccurrences;
    }

    if ( numOccurrences != 2 )
    {
      m_progress.SendLogMessage(LogWarn(Normal) << "Cross-edge %1 has %2 occurrence(s) while 2 is expected."
                                                << eid << numOccurrences);
      return false;
    }
  }

  m_progress.SendLogMessage(LogInfo(Normal) << "Blend chain seems to be suppressible.");
  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_SuppressBlendChain::isValidFace(const TopoDS_Face& face) const
{
  // This calibrated value is used to compensate weird tolerances which
  // happen to be insufficient to cover tiny contour gaps.
  const double tol = asiAlgo_CheckValidity::MaxTolerance(face)*5.0;

  return asiAlgo_CheckValidity::HasAllClosedWires(face, tol) &&
        !asiAlgo_CheckValidity::HasEdgesWithoutVertices(face);
}
