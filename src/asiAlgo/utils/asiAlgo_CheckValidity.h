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

#ifndef asiAlgo_CheckValidity_h
#define asiAlgo_CheckValidity_h

// asiAlgo includes
#include <asiAlgo.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Wire.hxx>

//-----------------------------------------------------------------------------

//! Utility to check validity of shapes.
class asiAlgo_CheckValidity : public ActAPI_IAlgorithm
{
public:

  //! Ctor accepting diagnostic tools.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_CheckValidity(ActAPI_ProgressEntry progress,
                          ActAPI_PlotterEntry  plotter);

public:

  //! Checks OCCT validity rules of the given shape.
  //! \param[in] shape shape to check.
  //! \return true if shape is valid, false -- otherwise.
  asiAlgo_EXPORT bool
    CheckBasic(const TopoDS_Shape& shape);

// Class-level API:
public:

  //! Checks whether the passed solid is finite or not. This method performs
  //! point membership classification (PMC) test for an "infinite" point.
  //! \param[in] solid body to check.
  //! \return true if the body is finite.
  asiAlgo_EXPORT static bool
    IsFinite(const TopoDS_Solid& solid);

  //! Checks whether the passed face has all contours (wires)
  //! geometrically closed. The default tolerance is used for coincidence
  //! test.
  //! \param[in] face face to check.
  //! \return true if face is Ok, false -- otherwise.
  asiAlgo_EXPORT static bool
    HasAllClosedWires(const TopoDS_Face& face);

  //! Checks whether the passed face has all contours (wires)
  //! geometrically closed.
  //! \param[in] face             face to check.
  //! \param[in] coincConfusion3d coincidence confusion tolerance. This value
  //!                             is used to recognize points as coincident.
  //! \return true if face is Ok, false -- otherwise.
  asiAlgo_EXPORT static bool
    HasAllClosedWires(const TopoDS_Face& face,
                      const double       coincConfusion3d);

  //! Checks whether the passed wire is closed.
  //! \param[in] wire             wire to check.
  //! \param[in] face             face owning the wire.
  //! \param[in] coincConfusion3d coincidence confusion tolerance. This value
  //!                             is used to recognize points as coincident.
  //! \return true if wire is Ok, false -- otherwise.
  asiAlgo_EXPORT static bool
    IsClosedGeometrically(const TopoDS_Wire& wire,
                          const TopoDS_Face& face,
                          const double       coincConfusion3d);

  //! Checks whether the passed face contains any edges without vertices.
  //! \param[in] face face to check.
  asiAlgo_EXPORT static bool
    HasEdgesWithoutVertices(const TopoDS_Face& face);

  //! Returns maximum tolerance value bound to the passed shape.
  //! \param[in] shape shape to check.
  //! \return maximum tolerance value.
  asiAlgo_EXPORT static double
    MaxTolerance(const TopoDS_Shape& shape);

  //! Checks for the given edge if it has distinct orientations of its
  //! vertices. In a valid edge, there should be one vertex with FORWARD
  //! orientation and one vertex with REVERSED orientation.
  //! \param[in] edge edge to check.
  //! \return true in case of validity, false -- otherwise.
  asiAlgo_EXPORT static bool
    HasDistinctVertexOrientations(const TopoDS_Edge& edge);

};

#endif
