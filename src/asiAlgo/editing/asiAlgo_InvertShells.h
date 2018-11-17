//-----------------------------------------------------------------------------
// Created on: 03 August 2018
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

#ifndef asiAlgo_InvertShells_h
#define asiAlgo_InvertShells_h

// asiAlgo includes
#include <asiAlgo.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

//-----------------------------------------------------------------------------

//! Utility to invert orientations of all shells in the given shape.
class asiAlgo_InvertShells : public ActAPI_IAlgorithm
{
public:

  //! Ctor accepting the input shape and diagnostic tools.
  //! \param[in] aag      input shape.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_InvertShells(const TopoDS_Shape&  shape,
                         ActAPI_ProgressEntry progress = NULL,
                         ActAPI_PlotterEntry  plotter  = NULL);

public:

  //! Performs shell flipping.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Perform();

public:

  //! \return result shape.
  const TopoDS_Shape& GetResult() const
  {
    return m_result;
  }

protected:

  //! Recursive routine which builds a new topological graph in bottom-up
  //! fashion. The bottom-up construction avoids modification of topological
  //! entities (as they are created and then just added to their direct
  //! parents) which is prohibited in OCCT. The routine recreates all
  //! top-level structural topological elements (compounds, solids).
  //!
  //! \param[in]  root   root topological element to iterate recursively.
  //! \param[our] result newly created topological element.
  asiAlgo_EXPORT void
    buildTopoGraphLevel(const TopoDS_Shape& root,
                        TopoDS_Shape&       result) const;

protected:

  TopoDS_Shape m_shape;  //!< Input shape.
  TopoDS_Shape m_result; //!< Result model.

};

#endif
