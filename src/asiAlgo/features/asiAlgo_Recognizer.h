//-----------------------------------------------------------------------------
// Created on: 01 October 2018
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

#ifndef asiAlgo_Recognizer_h
#define asiAlgo_Recognizer_h

// asiAlgo includes
#include <asiAlgo_FeatureFaces.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OpenCascade includes
#include <TopTools_IndexedMapOfShape.hxx>

class asiAlgo_AAG;

//-----------------------------------------------------------------------------

//! Abstract base class for feature recognizers. It provides a unified
//! usage pattern, data structures and a fixed call procedure for all AAG-based
//! feature identification algorithms.
class asiAlgo_Recognizer : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_Recognizer, ActAPI_IAlgorithm)

public:

  //! Ctor.
  //! \param[in] masterCAD full CAD model.
  //! \param[in] aag       AAG (will be created from CAD if nullptr is passed).
  //! \param[in] progress  progress notifier.
  //! \param[in] plotter   imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_Recognizer(const TopoDS_Shape&        masterCAD,
                       const Handle(asiAlgo_AAG)& aag,
                       ActAPI_ProgressEntry       progress,
                       ActAPI_PlotterEntry        plotter);

  //! Ctor.
  //! \param[in] aag      AAG (should not be nullptr here).
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_Recognizer(const Handle(asiAlgo_AAG)& aag,
                       ActAPI_ProgressEntry       progress,
                       ActAPI_PlotterEntry        plotter);

  //! Ctor.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_Recognizer(ActAPI_ProgressEntry progress,
                       ActAPI_PlotterEntry  plotter);

  //! Destructor.
  asiAlgo_EXPORT
    ~asiAlgo_Recognizer();

public:

  //! Initializes the recognition tool with the part shape in question.
  //! \param[in] masterCAD master CAD model.
  asiAlgo_EXPORT void
    Init(const TopoDS_Shape& masterCAD);

public:

  //! \return result collection of detected faces as transient shapes.
  asiAlgo_EXPORT const TopTools_IndexedMapOfShape&
    GetResultFaces() const;

  //! \return result collection of detected faces as integer ids.
  asiAlgo_EXPORT const asiAlgo_Feature&
    GetResultIndices() const;

  //! \return working AAG instance.
  asiAlgo_EXPORT const Handle(asiAlgo_AAG)&
    GetAAG() const;

  //! Sets AAG to operate on.
  //! \param[in] aag AAG instance to set.
  asiAlgo_EXPORT void
    SetAAG(const Handle(asiAlgo_AAG)& aag);

protected:

  //-------------------------------------------------------------------------//
  // IN
  //-------------------------------------------------------------------------//

  TopoDS_Shape        m_master; //!< Master CAD.
  Handle(asiAlgo_AAG) m_aag;    //!< Master AAG.

  //-------------------------------------------------------------------------//
  // OUT
  //-------------------------------------------------------------------------//

  struct
  {
    TopTools_IndexedMapOfShape faces; //!< Detected feature faces.
    asiAlgo_Feature            ids;   //!< Indices of the detected feature faces.

    void Clear()
    {
      faces.Clear();
      ids.Clear();
    }
  } m_result;

};

#endif
