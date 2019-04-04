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

#ifndef asiAlgo_RebuildEdge_h
#define asiAlgo_RebuildEdge_h

// asiAlgo includes
#include <asiAlgo_AAG.h>
#include <asiAlgo_FrozenVertices.h>
#include <asiAlgo_History.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

//-----------------------------------------------------------------------------

//! Geometric operator to rebuild edge from its adjacent surfaces.
class asiAlgo_RebuildEdge : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_RebuildEdge, ActAPI_IAlgorithm)

public:

  //! Constructor.
  //! \param[in] masterCAD full CAD model.
  //! \param[in] progress  Progress Notifier.
  //! \param[in] plotter   Imperative Plotter.
  asiAlgo_EXPORT
    asiAlgo_RebuildEdge(const TopoDS_Shape&  masterCAD,
                        ActAPI_ProgressEntry progress = NULL,
                        ActAPI_PlotterEntry  plotter  = NULL);

  //! Constructor.
  //! \param[in] masterCAD full CAD model.
  //! \param[in] aag       AAG of the full CAD model.
  //! \param[in] progress  Progress Notifier.
  //! \param[in] plotter   Imperative Plotter.
  asiAlgo_EXPORT
    asiAlgo_RebuildEdge(const TopoDS_Shape&        masterCAD,
                        const Handle(asiAlgo_AAG)& aag,
                        ActAPI_ProgressEntry       progress = NULL,
                        ActAPI_PlotterEntry        plotter  = NULL);

public:

  //! Performs operator.
  //! \param[in] edge edge to rebuild.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT virtual bool
    Perform(const TopoDS_Edge& edge);

  //! Allows to set a collection of vertices which should not be
  //! recreated (in 3D) by reintersection. In certain topological
  //! conditions, you may benefit from an a priori knowledge of your
  //! data to make computations more efficient and robust. This is a sort
  //! of knowledge-driven computation enabled here.
  //!
  //! \param[in] vertex vertex to add.
  asiAlgo_EXPORT void
    AddFrozenVertex(const TopoDS_Vertex& vertex);

  //! Sets the collection of intact ("frozen") vertices.
  //! \sa AddFrozenVertex()
  //!
  //! \param[in] vertices collection of vertices to set as frozen.
  asiAlgo_EXPORT void
    SetFrozenVertices(const asiAlgo_FrozenVertices& vertices);

public:

  //! \return modified model.
  const TopoDS_Shape& GetResult() const
  {
    return m_result;
  }

  //! \return modification history.
  const Handle(asiAlgo_History)& GetHistory() const
  {
    return m_history;
  }

  //! \brief Sets externally defined history.
  //!
  //! The passed history will not be cleared, so that a pipeline of
  //! modeling tools will grow its history graph as long as the history
  //! object is continuously passed from one tool to another.
  //!
  //! \param[in] history history to set.
  void SetHistory(const Handle(asiAlgo_History)& history)
  {
    m_history = history;
  }

protected:

  Handle(asiAlgo_History) m_history;        //!< Modification history.
  Handle(asiAlgo_AAG)     m_aag;            //!< AAG of the master CAD model.
  asiAlgo_FrozenVertices  m_frozenVertices; //!< Frozen vertices.
  TopoDS_Shape            m_input;          //!< Input CAD model.
  TopoDS_Shape            m_result;         //!< Result CAD model.

};

#endif
