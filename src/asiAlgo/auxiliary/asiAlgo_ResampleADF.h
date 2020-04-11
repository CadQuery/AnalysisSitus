//-----------------------------------------------------------------------------
// Created on: 11 April 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Sergey Slyadnev
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

#ifndef asiAlgo_ResampleADF_h
#define asiAlgo_ResampleADF_h

// asiAlgo includes
#include <asiAlgo_UniformGrid.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// Mobius includes
#include <mobius/poly_SVO.h>

// Standard includes
#include <string>
#include <ostream>

using namespace mobius;

//! Resamples an Adaptive Distance Field to generate a uniform
//! voxelization.
class asiAlgo_ResampleADF : public ActAPI_IAlgorithm
{
public:

  //! Ctor.
  //! \param[in] pSVO     root SVO node representing the ADF.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_ResampleADF(poly_SVO*            pSVO,
                        ActAPI_ProgressEntry progress = nullptr,
                        ActAPI_PlotterEntry  plotter  = nullptr);

public:

  //! Performs ADF resampling.
  //! \param[in] step discretization step.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Perform(const float step);

  //! \return resulting grid.
  asiAlgo_EXPORT const Handle(asiAlgo_UniformGrid<float>)&
    GetResult() const;

  //! Sets multithreading mode (parallel or sequential).
  //! \param[in] on Boolean value to set.
  asiAlgo_EXPORT void
    SetParallelMode(const bool on);

protected:

  //! Root SVO node.
  poly_SVO* m_pSVO;

  //! Uniform grid which is the result of the uniform sampling.
  Handle(asiAlgo_UniformGrid<float>) m_grid;

  //! Indicates whether the multithreading is on/off.
  bool m_bIsParallel;

};

#endif
