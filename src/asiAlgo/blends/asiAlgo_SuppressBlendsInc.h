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

#ifndef asiAlgo_SuppressBlendsInc_h
#define asiAlgo_SuppressBlendsInc_h

// asiAlgo includes
#include <asiAlgo.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

class asiAlgo_AAG;
class asiAlgo_History;

//-----------------------------------------------------------------------------

//! Utility to suppress blends incrementally.
class asiAlgo_SuppressBlendsInc : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_SuppressBlendsInc, ActAPI_IAlgorithm)

public:

  //! Ctor.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_SuppressBlendsInc(ActAPI_ProgressEntry progress = NULL,
                              ActAPI_PlotterEntry  plotter  = NULL);

public:

  //! Performs incremental blend suppression for the shape defined by AAG for
  //! its initial state.
  //! \param[in]  aag                 attributed adjacency graph of input.
  //! \param[in]  radius              max radius of blends for recognition.
  //! \param[out] result              result shape.
  //! \param[out] history             concatenated history of modification.
  //! \param[out] numSuppressedChains number of suppressed chains.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Perform(const Handle(asiAlgo_AAG)& aag,
            const double               radius,
            TopoDS_Shape&              result,
            Handle(asiAlgo_History)&   history,
            int&                       numSuppressedChains) const;

};

#endif
