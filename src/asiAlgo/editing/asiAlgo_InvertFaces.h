//-----------------------------------------------------------------------------
// Created on: 10 April 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef asiAlgo_InvertFaces_h
#define asiAlgo_InvertFaces_h

// asiAlgo includes
#include <asiAlgo_AAG.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

//-----------------------------------------------------------------------------

//! Utility to invert orientations of faces.
class asiAlgo_InvertFaces : public ActAPI_IAlgorithm
{
public:

  //! Ctor accepting AAG and diagnostic tools.
  //! \param aag      [in] input AAG for the working B-Rep geometry.
  //! \param progress [in] progress notifier.
  //! \param plotter  [in] imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_InvertFaces(const Handle(asiAlgo_AAG)& aag,
                        ActAPI_ProgressEntry       progress = NULL,
                        ActAPI_PlotterEntry        plotter = NULL);

public:

  //! Performs face flipping.
  //! \param faceIds [in] IDs of the faces to invert. These IDs should
  //!                     correspond to the faces in the working AAG.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Perform(const TColStd_PackedMapOfInteger& faceIds);

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
  //! top-level structural topological elements (compounds, solids, shells)
  //! and reuses all boundary elements starting from faces. The faces whose
  //! indices are enumerated in the passed collection are inverted.
  //!
  //! \param root         [in]  root topological element to iterate recursively.
  //! \param faces2Invert [in]  faces to invert.
  //! \param result       [out] newly created topological element.
  asiAlgo_EXPORT void
    buildTopoGraphLevel(const TopoDS_Shape&               root,
                        const TColStd_PackedMapOfInteger& faces2Invert,
                        TopoDS_Shape&                     result) const;

protected:

  Handle(asiAlgo_AAG)  m_aag;      //!< Master AAG.
  TopoDS_Shape         m_result;   //!< Result model.
  ActAPI_ProgressEntry m_progress; //!< Progress entry.
  ActAPI_PlotterEntry  m_plotter;  //!< Plotter entry.

};

#endif
