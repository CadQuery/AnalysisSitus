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

#ifndef asiAlgo_BlendTopoConditionFFOrdinaryVBF_h
#define asiAlgo_BlendTopoConditionFFOrdinaryVBF_h

// asiAlgo includes
#include <asiAlgo_BlendTopoCondition.h>
#include <asiAlgo_Utils.h>

//! Topological condition for ordinary vertex blends in a chain.
struct asiAlgo_BlendTopoConditionFFOrdinaryVBF : public asiAlgo_BlendTopoCondition
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_BlendTopoConditionFFOrdinaryVBF, asiAlgo_BlendTopoCondition)

public:

  //! Ctor.
  //! \param[in] aag      attributed adjacency graph of the CAD-model in question.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_BlendTopoConditionFFOrdinaryVBF(const Handle(asiAlgo_AAG)& aag,
                                          ActAPI_ProgressEntry       progress,
                                          ActAPI_PlotterEntry        plotter)
  //
  : asiAlgo_BlendTopoCondition(aag, progress, plotter)
  {}

public:

  //! Dumps the topological condition to the passed plotter.
  virtual void Dump(ActAPI_PlotterEntry plotter) const
  {
    asiAlgo_BlendTopoCondition::Dump(plotter); // Dump base entities.
  }

  //! Allows to identify a certain topological condition from the passed blend
  //! candidate attribute.
  //! \param[in] bcAttr blend candidate attribute in question.
  //! \return true if the certain topological condition is identified.
  virtual bool Initialize(const Handle(asiAlgo_AttrBlendCandidate)& bcAttr)
  {
    if ( bcAttr->Kind != BlendType_Vertex )
      return false;

    // Identify the spring edges and the support faces.
    if ( !asiAlgo_BlendTopoCondition::Initialize(bcAttr) )
      return false; // Not identified.

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

    // Kill the vertex blend.
    if ( !this->kfmv(output, this->f_b, output, history) )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "KFMV failed on vertex blend.");
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
    this->f_b = TopoDS::Face( history->GetLastModifiedOrArg(this->f_b) );
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
    asiAlgo_NotUsed(edges);
  }

};

#endif
