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

#ifndef asiAlgo_BlendTopoConditionIsolated_h
#define asiAlgo_BlendTopoConditionIsolated_h

// asiAlgo includes
#include <asiAlgo_BlendTopoConditionFF.h>
#include <asiAlgo_Utils.h>

//! Topological condition for isolated face-face blends.
struct asiAlgo_BlendTopoConditionFFIsolated : public asiAlgo_BlendTopoConditionFF
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_BlendTopoConditionFFIsolated, asiAlgo_BlendTopoConditionFF)

public:

  TopoDS_Face   f_t1;      //!< Terminating face 1.
  TopoDS_Face   f_t2;      //!< Terminating face 2.
  TopoDS_Edge   e_b_t1;    //!< Terminating edge 1 between faces `b` and `t1`.
  TopoDS_Edge   e_b_t2;    //!< Terminating edge 2 between faces `b` and `t2`.
  TopoDS_Edge   e_s1_t1;   //!< Edge between `s1` and `t1`.
  TopoDS_Edge   e_s1_t2;   //!< Edge between `s1` and `t2`.
  TopoDS_Edge   e_s2_t1;   //!< Edge between `s2` and `t1`.
  TopoDS_Edge   e_s2_t2;   //!< Edge between `s2` and `t2`.
  TopoDS_Vertex v_b_s1_t1; //!< Vertex common for `b`, `s1` and `t1`.
  TopoDS_Vertex v_b_s2_t1; //!< Vertex common for `b`, `s2` and `t1`.
  TopoDS_Vertex v_b_s1_t2; //!< Vertex common for `b`, `s1` and `t2`.
  TopoDS_Vertex v_b_s2_t2; //!< Vertex common for `b`, `s2` and `t2`.

  //! Modified geometry.
  struct ModifGeom : public asiAlgo_BlendTopoConditionFF::ModifGeom
  {
    Handle(Geom_Surface) s_t1;
    Handle(Geom_Surface) s_t2;
    Handle(Geom_Curve)   c_s1_t1;
    Handle(Geom_Curve)   c_s1_t2;
    Handle(Geom_Curve)   c_s2_t1;
    Handle(Geom_Curve)   c_s2_t2;
  };

public:

  //! Ctor.
  //! \param[in] aag      attributed adjacency graph of the CAD-model in question.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_BlendTopoConditionFFIsolated(const Handle(asiAlgo_AAG)& aag,
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

    plotter.REDRAW_SHAPE("f_t1",      this->f_t1);
    plotter.REDRAW_SHAPE("f_t2",      this->f_t2);
    plotter.REDRAW_SHAPE("e_b_t1",    this->e_b_t1,    Color_Red,   1.0, true);
    plotter.REDRAW_SHAPE("e_b_t2",    this->e_b_t2,    Color_Red,   1.0, true);
    plotter.REDRAW_SHAPE("e_s1_t1",   this->e_s1_t1,   Color_White, 1.0, true);
    plotter.REDRAW_SHAPE("e_s1_t2",   this->e_s1_t2,   Color_White, 1.0, true);
    plotter.REDRAW_SHAPE("e_s2_t1",   this->e_s2_t1,   Color_White, 1.0, true);
    plotter.REDRAW_SHAPE("e_s2_t2",   this->e_s2_t2,   Color_White, 1.0, true);
    plotter.REDRAW_SHAPE("v_b_s1_t1", this->v_b_s1_t1, Color_White, 1.0, true);
    plotter.REDRAW_SHAPE("v_b_s1_t2", this->v_b_s1_t2, Color_White, 1.0, true);
    plotter.REDRAW_SHAPE("v_b_s2_t1", this->v_b_s2_t1, Color_White, 1.0, true);
    plotter.REDRAW_SHAPE("v_b_s2_t2", this->v_b_s2_t2, Color_White, 1.0, true);
  }

  //! Lets to identify a certain topological condition from the passed blend
  //! candidate attribute.
  //! \param[in] bcAttr blend candidate attribute in question.
  //! \return true if the certain topological condition is identified.
  virtual bool Initialize(const Handle(asiAlgo_AttrBlendCandidate)& bcAttr)
  {
    // Identify the spring edges and the support faces.
    if ( !asiAlgo_BlendTopoConditionFF::Initialize(bcAttr) )
      return false; // Not identified.

    // Isolated blend contains two terminating edges.
    const int numCrossEdges  = bcAttr->CrossEdgeIndices.Extent();
    const int numTermEdges   = bcAttr->TerminatingEdgeIndices.Extent();
    const int numSpringEdges = bcAttr->SpringEdgeIndices.Extent();
    //
    if ( numCrossEdges != 0 || numTermEdges != 2 || numSpringEdges != 2 )
      return false;

    // Get ID of the blend face.
    const int f_b_idx = bcAttr->GetFaceId();

    // Get terminating edges.
    const int   e_b_t1_idx = bcAttr->TerminatingEdgeIndices.GetMinimalMapped();
    const int   e_b_t2_idx = bcAttr->TerminatingEdgeIndices.GetMaximalMapped();
    TopoDS_Edge e_b_t1_loc = TopoDS::Edge( AAG->GetMapOfEdges()(e_b_t1_idx) );
    TopoDS_Edge e_b_t2_loc = TopoDS::Edge( AAG->GetMapOfEdges()(e_b_t2_idx) );

    // Get terminating (`t`) faces as neighbors to the blend face through
    // the terminating edges.
    TColStd_PackedMapOfInteger f_b_t1_indices = AAG->GetNeighborsThru(f_b_idx, e_b_t1_loc);
    TColStd_PackedMapOfInteger f_b_t2_indices = AAG->GetNeighborsThru(f_b_idx, e_b_t2_loc);
    //
    if ( f_b_t1_indices.Extent() != 1 || f_b_t2_indices.Extent() != 1 )
      return false;
    //
    const int f_b_t1_idx = f_b_t1_indices.GetMinimalMapped();
    const int f_b_t2_idx = f_b_t2_indices.GetMinimalMapped();

    // Initialize topological primitives.
    this->f_t1   = this->AAG->GetFace(f_b_t1_idx);
    this->f_t2   = this->AAG->GetFace(f_b_t2_idx);
    this->e_b_t1 = e_b_t1_loc;
    this->e_b_t2 = e_b_t2_loc;

    // Get common vertices.
    this->v_b_s1_t1 = asiAlgo_Utils::GetCommonVertex(this->f_b, this->f_s1, this->f_t1);
    this->v_b_s2_t1 = asiAlgo_Utils::GetCommonVertex(this->f_b, this->f_s2, this->f_t1);
    this->v_b_s1_t2 = asiAlgo_Utils::GetCommonVertex(this->f_b, this->f_s1, this->f_t2);
    this->v_b_s2_t2 = asiAlgo_Utils::GetCommonVertex(this->f_b, this->f_s2, this->f_t2);

    // Get other common edges.
    this->e_s1_t1 = asiAlgo_Utils::GetCommonEdge(this->f_s1, this->f_t1, this->v_b_s1_t1);
    this->e_s1_t2 = asiAlgo_Utils::GetCommonEdge(this->f_s1, this->f_t2, this->v_b_s1_t2);
    this->e_s2_t1 = asiAlgo_Utils::GetCommonEdge(this->f_s2, this->f_t1, this->v_b_s2_t1);
    this->e_s2_t2 = asiAlgo_Utils::GetCommonEdge(this->f_s2, this->f_t2, this->v_b_s2_t2);

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

    history = new asiAlgo_History;

    // Kill the first terminating edge.
    if ( !this->kev(output, this->e_b_t1, output, history) )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "KEV failed on terminating edge.");
      return false;
    }
    //
    this->Actualize(history);

    // Kill the second terminating edge.
    if ( !this->kev(output, this->e_b_t2, output, history) )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "KEV failed on terminating edge.");
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
    this->f_b       = TopoDS::Face   ( history->GetLastModifiedOrArg(this->f_b) );
    this->f_s1      = TopoDS::Face   ( history->GetLastModifiedOrArg(this->f_s1) );
    this->f_s2      = TopoDS::Face   ( history->GetLastModifiedOrArg(this->f_s2) );
    this->f_t1      = TopoDS::Face   ( history->GetLastModifiedOrArg(this->f_t1) );
    this->f_t2      = TopoDS::Face   ( history->GetLastModifiedOrArg(this->f_t2) );
    this->e_b_t1    = TopoDS::Edge   ( history->GetLastModifiedOrArg(this->e_b_t1) );
    this->e_b_t2    = TopoDS::Edge   ( history->GetLastModifiedOrArg(this->e_b_t2) );
    this->e_b_s1    = TopoDS::Edge   ( history->GetLastModifiedOrArg(this->e_b_s1) );
    this->e_b_s2    = TopoDS::Edge   ( history->GetLastModifiedOrArg(this->e_b_s2) );
    this->e_s1_t1   = TopoDS::Edge   ( history->GetLastModifiedOrArg(this->e_s1_t1) );
    this->e_s1_t2   = TopoDS::Edge   ( history->GetLastModifiedOrArg(this->e_s1_t2) );
    this->e_s2_t1   = TopoDS::Edge   ( history->GetLastModifiedOrArg(this->e_s2_t1) );
    this->e_s2_t2   = TopoDS::Edge   ( history->GetLastModifiedOrArg(this->e_s2_t2) );
    this->v_b_s1_t1 = TopoDS::Vertex ( history->GetLastModifiedOrArg(this->v_b_s1_t1) );
    this->v_b_s1_t2 = TopoDS::Vertex ( history->GetLastModifiedOrArg(this->v_b_s1_t2) );
    this->v_b_s2_t1 = TopoDS::Vertex ( history->GetLastModifiedOrArg(this->v_b_s2_t1) );
    this->v_b_s2_t2 = TopoDS::Vertex ( history->GetLastModifiedOrArg(this->v_b_s2_t2) );
  }

  //! Gathers the collection of affected edges to rebuild as a result of
  //! suppression.
  //! \param[out] edges   output collection of edges to rebuild.
  virtual void GatherAffectedEdges(NCollection_IndexedMap<TopoDS_Edge>& edges) const
  {
    edges.Add(this->e_s1_t1);
    edges.Add(this->e_s1_t2);
    edges.Add(this->e_b_s1);
    edges.Add(this->e_s2_t1);
    edges.Add(this->e_s2_t2);
  }

};

#endif
