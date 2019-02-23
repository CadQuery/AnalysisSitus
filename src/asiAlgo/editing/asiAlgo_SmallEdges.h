//-----------------------------------------------------------------------------
// Created on: 06 August 2018
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

#ifndef asiAlgo_SmallEdges_h
#define asiAlgo_SmallEdges_h

// asiAlgo includes
#include <asiAlgo.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <ShapeBuild_ReShape.hxx>

//-----------------------------------------------------------------------------

//! Fixes small edges.
class asiAlgo_SmallEdges : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_SmallEdges, ActAPI_IAlgorithm)

public:

  //! Constructor.
  //! \param[in] masterCAD CAD part.
  //! \param[in] progress  Progress Notifier.
  //! \param[in] plotter   Imperative Plotter.
  asiAlgo_EXPORT
    asiAlgo_SmallEdges(const TopoDS_Shape&  masterCAD,
                       ActAPI_ProgressEntry progress,
                       ActAPI_PlotterEntry  plotter);

public:

  //! Fixes small edges.
  //! \param[in] len     linear threshold.
  //! \param[in] ang_deg angular threshold.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Perform(const double len,
            const double ang_deg);

public:

  //! \return fixed shape.
  const TopoDS_Shape& GetResult() const
  {
    return m_output;
  }

private:

  Handle(ShapeBuild_ReShape) m_reShape; //!< Re-shape utility.
  TopoDS_Shape               m_input;   //!< Input shape.
  TopoDS_Shape               m_output;  //!< Output shape.

};

#endif
