//-----------------------------------------------------------------------------
// Created on: 28 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
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

#ifndef asiAlgo_STEP_h
#define asiAlgo_STEP_h

// asiAlgo includes
#include <asiAlgo_InteropVars.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

//-----------------------------------------------------------------------------

//! STEP interoperability tool.
class asiAlgo_STEP : public ActAPI_IAlgorithm
{
public:

  //! Ctor accepting progress notifier and imperative plotter.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_STEP(ActAPI_ProgressEntry progress,
               ActAPI_PlotterEntry  plotter = NULL)
  //
  : ActAPI_IAlgorithm(progress, plotter) {}

public:

  //! Performs STEP import.
  //! \param[in] filename  file to read.
  //! \param[in] doHealing indicates whether to run shape healing after import.
  //! \param[in] result    retrieved shape.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Read(const TCollection_AsciiString& filename,
         const bool                     doHealing,
         TopoDS_Shape&                  result);

  //! Saves the passed CAD model to STEP file.
  //! \param[in] shape    shape to store.
  //! \param[in] filename file to save.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Write(const TopoDS_Shape&            shape,
          const TCollection_AsciiString& filename);
};

#endif
