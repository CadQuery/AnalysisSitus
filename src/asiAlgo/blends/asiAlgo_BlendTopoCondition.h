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

#ifndef asiAlgo_BlendTopoCondition_h
#define asiAlgo_BlendTopoCondition_h

// asiAlgo includes
#include <asiAlgo_AAG.h>
#include <asiAlgo_AttrBlendCandidate.h>
#include <asiAlgo_History.h>

// Active Data includes
#include <ActAPI_IProgressNotifier.h>
#include <ActAPI_IPlotter.h>

// OCCT includes
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

//-----------------------------------------------------------------------------

//! Base class for topological conditions on blends.
class asiAlgo_BlendTopoCondition : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_BlendTopoCondition, Standard_Transient)

public:

  TopoDS_Face f_b; //!< Blend face.

public:

  //! Ctor.
  //! \param[in] aag      attributed adjacency graph of the CAD-model in question.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_BlendTopoCondition(const Handle(asiAlgo_AAG)& aag,
                             ActAPI_ProgressEntry       progress,
                             ActAPI_PlotterEntry        plotter)
  //
  : AAG        ( aag ),
    m_progress ( progress ),
    m_plotter  ( plotter )
  {}

public:

  //! Dumps the topological condition to the passed plotter.
  virtual void Dump(ActAPI_PlotterEntry plotter) const
  {
    plotter.REDRAW_SHAPE("f_b", this->f_b);
  }

  //! Lets to identify a certain topological condition from the passed blend
  //! candidate attribute.
  //! \param[in] bcAttr blend candidate attribute in question.
  //! \return true if the certain topological condition is identified.
  virtual bool Initialize(const Handle(asiAlgo_AttrBlendCandidate)& bcAttr)
  {
    // Get ID of the blend face.
    const int f_b_idx = bcAttr->GetFaceId();

    // Initialize blend face.
    this->f_b = this->AAG->GetFace(f_b_idx);

    return true; // Identified.
  }

  //! Collapses topology locally.
  //! \param[in]  input   input shape to perform feature suppression on.
  //! \param[out] output  suppression result.
  //! \param[out] history modification history.
  //! \return true in case of success, false -- otherwise.
  virtual bool Suppress(const TopoDS_Shape&      input,
                        TopoDS_Shape&            output,
                        Handle(asiAlgo_History)& history) = 0;

  //! Actualizes the current state of topological condition w.r.t. the
  //! passed history.
  //! \param[in] history modification history to apply.
  virtual void Actualize(const Handle(asiAlgo_History)& history) = 0;

  //! Gathers the collection of affected edges to rebuild as a result of
  //! suppression.
  //! \param[out] edges   output collection of edges to rebuild.
  virtual void GatherAffectedEdges(NCollection_IndexedMap<TopoDS_Edge>& edges) const = 0;

protected:

  //! Performs KEV Euler operator on the passed shape.
  //! \param[in]  shape       entire shape.
  //! \param[in]  edge        edge to kill.
  //! \param[in]  vertex2Kill vertex to kill.
  //! \param[out] output      result of Euler operator.
  //! \param[out] history     history of modification.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    kev(const TopoDS_Shape&      shape,
        const TopoDS_Edge&       edge,
        const TopoDS_Vertex&     vertex2Kill,
        TopoDS_Shape&            output,
        Handle(asiAlgo_History)& history);

  //! Performs KEF Euler operator on the passed shape.
  //! \param[in]  shape     entire shape.
  //! \param[in]  face      face to kill.
  //! \param[in]  edge2Kill edge to kill.
  //! \param[in]  edge2Save edge to save.
  //! \param[out] output    result of Euler operator.
  //! \param[out] history   history of modification.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    kef(const TopoDS_Shape&      shape,
        const TopoDS_Face&       face,
        const TopoDS_Edge&       edge2Kill,
        const TopoDS_Edge&       edge2Save,
        TopoDS_Shape&            output,
        Handle(asiAlgo_History)& history);

public:

  Handle(asiAlgo_AAG) AAG; //!< Attributed adjacency graph.

protected:

  ActAPI_ProgressEntry m_progress; //!< Progress notifier.
  ActAPI_PlotterEntry  m_plotter;  //!< Imperative plotter.

};

#endif
