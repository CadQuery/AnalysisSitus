//-----------------------------------------------------------------------------
// Created on: 10 April 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
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
