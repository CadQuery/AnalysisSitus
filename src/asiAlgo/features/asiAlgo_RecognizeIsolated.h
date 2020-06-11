//-----------------------------------------------------------------------------
// Created on: 11 June 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2016-present, Sergey Slyadnev
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

#ifndef asiAlgo_RecognizeIsolated_h
#define asiAlgo_RecognizeIsolated_h

// asiAlgo includes
#include <asiAlgo_Recognizer.h>

// OCCT includes
#include <Precision.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

//-----------------------------------------------------------------------------

//! Recognizes isolated features, i.e., the features that start from internal
//! contours of the picked faces (one or several).
//!
//! \todo enable storing the detected feature faces in AAG, else -- it is not a "recognizer" but a "finder."
class asiAlgo_RecognizeIsolated : public asiAlgo_Recognizer
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_RecognizeIsolated, asiAlgo_Recognizer)

public:

  //! Ctor.
  //! \param[in] shape    shape in question.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_RecognizeIsolated(const TopoDS_Shape&  shape,
                              ActAPI_ProgressEntry progress,
                              ActAPI_PlotterEntry  plotter);

  //! Ctor.
  //! \param[in] shape    shape in question.
  //! \param[in] aag      attributed adjacency graph in question.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_RecognizeIsolated(const TopoDS_Shape&        shape,
                              const Handle(asiAlgo_AAG)& aag,
                              ActAPI_ProgressEntry       progress,
                              ActAPI_PlotterEntry        plotter);

  //! Ctor.
  //! \param[in] aag      attributed adjacency graph in question.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_RecognizeIsolated(const Handle(asiAlgo_AAG)& aag,
                              ActAPI_ProgressEntry       progress,
                              ActAPI_PlotterEntry        plotter);

public:

  //! Performs recognition of the isolated features.
  //! \param[in] seeds seed faces to start recognition from.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT virtual bool
    Perform(const asiAlgo_Feature& seeds);

protected:

  //! Extracts outer and inner edges (if any) for the given face.
  //! \param[in]  face       face of interest.
  //! \param[out] outerEdges outer edges.
  //! \param[out] innerEdges inner edges, one vector for each inner wire.
  asiAlgo_EXPORT void
    getFaceEdges(const TopoDS_Face&                       face,
                 std::vector<TopoDS_Edge>&                outerEdges,
                 std::vector< std::vector<TopoDS_Edge> >& innerEdges) const;

  //! Checks whether all edges from some inner wire are present in
  //! the passed group of faces.
  //! \param[in] innerEdges   inner edges, one vector for each inner wire.
  //! \param[in] groupToCheck face group of interest.
  //! \return true if `groupToCheck` has all edges from particular group,
  //!         false -- otherwise.
  asiAlgo_EXPORT bool
    checkPresence(const std::vector< std::vector<TopoDS_Edge> >& innerEdges,
                  const asiAlgo_Feature&                         groupToCheck) const;

};

#endif
