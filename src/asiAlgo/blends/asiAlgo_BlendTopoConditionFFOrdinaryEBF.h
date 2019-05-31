//-----------------------------------------------------------------------------
// Created on: 12 April (*) 2019
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

#ifndef asiAlgo_BlendTopoConditionFFOrdinaryEBF_h
#define asiAlgo_BlendTopoConditionFFOrdinaryEBF_h

// asiAlgo includes
#include <asiAlgo_BlendTopoConditionFF.h>
#include <asiAlgo_Utils.h>

//! Topological condition for ordinary edge blends in a chain.
struct asiAlgo_BlendTopoConditionFFOrdinaryEBF : public asiAlgo_BlendTopoConditionFF
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_BlendTopoConditionFFOrdinaryEBF, asiAlgo_BlendTopoConditionFF)

public:

  TopoDS_Face   f_c1;      //!< Next face 1 for the blend face `b`.
  TopoDS_Face   f_c2;      //!< Next face 2 for the blend face `b`.
  TopoDS_Edge   e_b_c1;    //!< Cross edge between `b` and the first neighbor `c1`.
  TopoDS_Edge   e_b_c2;    //!< Cross edge between `b` and the second neighbor `c2`.
  TopoDS_Vertex v_b_s1_c1; //!< Vertex common for `b`, `s1` and `c1`.
  TopoDS_Vertex v_b_s2_c1; //!< Vertex common for `b`, `s2` and `c1`.
  TopoDS_Vertex v_b_s1_c2; //!< Vertex common for `b`, `s1` and `c2`.
  TopoDS_Vertex v_b_s2_c2; //!< Vertex common for `b`, `s2` and `c2`.

  //! Additional edges to recompute (the ones touchin the vertices `b_s1_c1`, etc.).
  TopTools_IndexedMapOfShape e_extra;

public:

  //! Ctor.
  //! \param[in] aag      attributed adjacency graph of the CAD-model in question.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_BlendTopoConditionFFOrdinaryEBF(const Handle(asiAlgo_AAG)& aag,
                                          ActAPI_ProgressEntry       progress,
                                          ActAPI_PlotterEntry        plotter)
  //
  : asiAlgo_BlendTopoConditionFF(aag, progress, plotter)
  {}

public:

  //! Dumps the topological condition to the passed plotter.
  virtual void Dump(ActAPI_PlotterEntry plotter) const
  {
    asiAlgo_BlendTopoConditionFF::Dump(plotter); // Dump base entities.

    plotter.REDRAW_SHAPE("f_c1",      this->f_c1);
    plotter.REDRAW_SHAPE("f_c2",      this->f_c2);
    plotter.REDRAW_SHAPE("e_b_c1",    this->e_b_c1,    Color_Red,   1.0, true);
    plotter.REDRAW_SHAPE("e_b_c2",    this->e_b_c2,    Color_Red,   1.0, true);
    plotter.REDRAW_SHAPE("v_b_s1_c1", this->v_b_s1_c1, Color_White, 1.0, true);
    plotter.REDRAW_SHAPE("v_b_s1_c2", this->v_b_s1_c2, Color_White, 1.0, true);
    plotter.REDRAW_SHAPE("v_b_s2_c1", this->v_b_s2_c1, Color_White, 1.0, true);
    plotter.REDRAW_SHAPE("v_b_s2_c2", this->v_b_s2_c2, Color_White, 1.0, true);
  }

  //! Allows to identify a certain topological condition from the passed blend
  //! candidate attribute.
  //! \param[in] bcAttr blend candidate attribute in question.
  //! \return true if the certain topological condition is identified.
  virtual bool Initialize(const Handle(asiAlgo_AttrBlendCandidate)& bcAttr)
  {
    // Identify the spring edges and the support faces.
    if ( !asiAlgo_BlendTopoConditionFF::Initialize(bcAttr) )
      return false; // Not identified.

    // Ordinary blend contains either two cross edges or one cross and one
    // terminating edge. It should not contain two terminating edges as,
    // otherwise, it is an isolated blend (which is another topo condition).
    int e_b_c1_idx = 0, e_b_c2_idx = 0;
    //
    const int numCrossEdges  = bcAttr->CrossEdgeIndices.Extent();
    const int numTermEdges   = bcAttr->TerminatingEdgeIndices.Extent();
    const int numSpringEdges = bcAttr->SpringEdgeIndices.Extent();
    //
    if ( numSpringEdges != 2 )
      return false;
    //
    if ( numTermEdges == 2 )
      return false;
    //
    if ( numCrossEdges == 2 && numTermEdges == 0 )
    {
      e_b_c1_idx = bcAttr->CrossEdgeIndices.GetMinimalMapped();
      e_b_c2_idx = bcAttr->CrossEdgeIndices.GetMaximalMapped();
    }
    else if ( numCrossEdges == 1 && numTermEdges == 1 )
    {
      e_b_c1_idx = bcAttr->CrossEdgeIndices.GetMinimalMapped();
      e_b_c2_idx = bcAttr->TerminatingEdgeIndices.GetMinimalMapped();
    }
    else
      return false;

    // Get ID of the blend face.
    const int f_b_idx = bcAttr->GetFaceId();

    // Get cross edges.
    TopoDS_Edge e_b_c1_loc = TopoDS::Edge( AAG->RequestMapOfEdges()(e_b_c1_idx) );
    TopoDS_Edge e_b_c2_loc = TopoDS::Edge( AAG->RequestMapOfEdges()(e_b_c2_idx) );

    // Get `c` faces as neighbors to the blend face through the cross edges.
    TColStd_PackedMapOfInteger f_b_c1_indices = AAG->GetNeighborsThru(f_b_idx, e_b_c1_loc);
    TColStd_PackedMapOfInteger f_b_c2_indices = AAG->GetNeighborsThru(f_b_idx, e_b_c2_loc);
    //
    if ( f_b_c1_indices.Extent() != 1 || f_b_c2_indices.Extent() != 1 )
      return false;
    //
    const int f_b_c1_idx = f_b_c1_indices.GetMinimalMapped();
    const int f_b_c2_idx = f_b_c2_indices.GetMinimalMapped();

    // Initialize topological primitives.
    this->f_c1   = this->AAG->GetFace(f_b_c1_idx);
    this->f_c2   = this->AAG->GetFace(f_b_c2_idx);
    this->e_b_c1 = e_b_c1_loc;
    this->e_b_c2 = e_b_c2_loc;

    // Get common vertices.
    this->v_b_s1_c1 = asiAlgo_Utils::GetCommonVertex(this->f_b, this->f_s1, this->f_c1);
    this->v_b_s2_c1 = asiAlgo_Utils::GetCommonVertex(this->f_b, this->f_s2, this->f_c1);
    this->v_b_s1_c2 = asiAlgo_Utils::GetCommonVertex(this->f_b, this->f_s1, this->f_c2);
    this->v_b_s2_c2 = asiAlgo_Utils::GetCommonVertex(this->f_b, this->f_s2, this->f_c2);

    // Add additional edges to rebuild (if any).
    asiAlgo_Utils::GetCommonEdges(this->f_s1, this->v_b_s1_c1, e_extra);
    asiAlgo_Utils::GetCommonEdges(this->f_s1, this->v_b_s1_c2, e_extra);
    asiAlgo_Utils::GetCommonEdges(this->f_s2, this->v_b_s2_c1, e_extra);
    asiAlgo_Utils::GetCommonEdges(this->f_s2, this->v_b_s2_c2, e_extra);

    return true; // Identified.
  }

  //! Collapses topology locally.
  //! \param[in]  input   input shape to perform feature suppression on.
  //! \param[out] output  suppression result.
  //! \param[out] history modification history.
  //! \return true in case of success, false -- otherwise.
  virtual bool Suppress(const TopoDS_Shape&      input,
                        TopoDS_Shape&            output,
                        Handle(asiAlgo_History)& history)
  {
    output = input;

    if ( history.IsNull() )
      history = new asiAlgo_History;

    // Kill the first cross edge. We also specify a vertex to kill to
    // be as much deterministic as possible.
    if ( !this->e_b_c1.IsNull() && !this->kev(output, this->e_b_c1, this->v_b_s1_c1, output, history) )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "KEV failed on the first cross edge.");
      return false;
    }
    //
    this->Actualize(history);

    // Kill the second cross edge.
    if ( !this->e_b_c2.IsNull() && !this->kev(output, this->e_b_c2, this->v_b_s1_c2, output, history) )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "KEV failed on the second cross edge.");
      return false;
    }
    //
    this->Actualize(history);

    // Kill the blend face.
    if ( !this->kef(output, this->f_b, this->e_b_s2, this->e_b_s1, output, history) )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "KEF failed for the blend face and spring edge.");
      return false;
    }
    //
    this->Actualize(history);

    return true;
  }

  //! Actualizes the current state of topological condition w.r.t. the
  //! passed history.
  //! \param[in] history modification history to apply.
  virtual void Actualize(const Handle(asiAlgo_History)& history)
  {
    this->f_b       = TopoDS::Face   ( history->GetLastImageOrArg(this->f_b) );
    this->f_s1      = TopoDS::Face   ( history->GetLastImageOrArg(this->f_s1) );
    this->f_s2      = TopoDS::Face   ( history->GetLastImageOrArg(this->f_s2) );
    this->f_c1      = TopoDS::Face   ( history->GetLastImageOrArg(this->f_c1) );
    this->f_c2      = TopoDS::Face   ( history->GetLastImageOrArg(this->f_c2) );
    this->e_b_c1    = TopoDS::Edge   ( history->GetLastImageOrArg(this->e_b_c1) );
    this->e_b_c2    = TopoDS::Edge   ( history->GetLastImageOrArg(this->e_b_c2) );
    this->e_b_s1    = TopoDS::Edge   ( history->GetLastImageOrArg(this->e_b_s1) );
    this->e_b_s2    = TopoDS::Edge   ( history->GetLastImageOrArg(this->e_b_s2) );
    this->v_b_s1_c1 = TopoDS::Vertex ( history->GetLastImageOrArg(this->v_b_s1_c1) );
    this->v_b_s1_c2 = TopoDS::Vertex ( history->GetLastImageOrArg(this->v_b_s1_c2) );
    this->v_b_s2_c1 = TopoDS::Vertex ( history->GetLastImageOrArg(this->v_b_s2_c1) );
    this->v_b_s2_c2 = TopoDS::Vertex ( history->GetLastImageOrArg(this->v_b_s2_c2) );

    // Update extra edges.
    TopTools_IndexedMapOfShape e_extra_updated;
    for ( int k = 1; k <= e_extra.Extent(); ++k )
      e_extra_updated.Add( TopoDS::Edge( history->GetLastImageOrArg( this->e_extra(k) ) ) );
  }

  //! Gathers the collection of affected edges to rebuild as a result of
  //! suppression. Every edge may declare some vertices as "frozen", i.e.,
  //! these vertices need no recomputation at geometry normalization stage.
  //! The decision whether to freeze or not a specific vertex will be taken
  //! by the computational scheme as it possesses knowledge not only of
  //! one topological condition, but of many of them. That is to say that
  //! if one vertex is frozen in one topological condition, it may be
  //! non-frozen in another. In such situation, for the normalization stage
  //! to succeed, such vertices should be unfreezed.
  //!
  //! \param[out] edges output collection of edges to rebuild.
  virtual void GatherAffectedEdges(asiAlgo_Edges2Rebuild& edges) const
  {
    edges.Add( asiAlgo_Edge2Rebuild(this->e_b_s1) );

    // TODO: for those extra edges, frozen vertices can be determined.
    for ( int k = 1; k <= e_extra.Extent(); ++k )
      edges.Add( asiAlgo_Edge2Rebuild( TopoDS::Edge( this->e_extra(k) ) ) );
  }

};

#endif
