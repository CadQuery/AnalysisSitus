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

#ifndef asiAlgo_SuppressBlendChain_h
#define asiAlgo_SuppressBlendChain_h

// asiAlgo includes
#include <asiAlgo_BlendTopoCondition.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

//! Utility to suppress the previously recognized blend chain starting from
//! the given seed face.
class asiAlgo_SuppressBlendChain : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_SuppressBlendChain, ActAPI_IAlgorithm)

public:

  //! Ctor.
  //! \param[in] aag      attributed adjacency graph containing the recognition
  //!                     results and hence (implicitly) the blend chain to
  //!                     suppress.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_SuppressBlendChain(const Handle(asiAlgo_AAG)& aag,
                               ActAPI_ProgressEntry       progress = NULL,
                               ActAPI_PlotterEntry        plotter  = NULL);

public:

  //! Performs suppression of a blend chain given a seed face index.
  //! \param[in] faceId 1-based ID of the seed face.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Perform(const int faceId);

public:

  //! Accessor for the result shape.
  //! \return result shape.
  const TopoDS_Shape& GetResult() const
  {
    return m_result;
  }

protected:

  //! Actualizes the current state of topological conditions w.r.t. the
  //! passed history.
  //! \param[in] conditions conditions to update.
  //! \param[in] toSkip     blend condition to skip.
  //! \param[in] history    modification history to apply.
  asiAlgo_EXPORT virtual void
    updateTopoConditions(const NCollection_DataMap<int, Handle(asiAlgo_BlendTopoCondition)>& conditions,
                         const int                                                           toSkip,
                         const Handle(asiAlgo_History)&                                      history) const;

protected:

  //! Working instance of the attributed adjacency graph.
  Handle(asiAlgo_AAG) m_aag;

  //! Result shape.
  TopoDS_Shape m_result;

};

#endif
