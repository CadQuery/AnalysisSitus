//-----------------------------------------------------------------------------
// Created on: 27 November 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017-present, Sergey Slyadnev
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

#ifndef asiAlgo_Naming_h
#define asiAlgo_Naming_h

// asiAlgo includes
#include <asiAlgo_History.h>

// OpenCascade includes
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

class asiAlgo_AAG;

//-----------------------------------------------------------------------------

//! \brief Naming service for a B-Rep part.
//!
//! Naming service is essentially a super-structure over the history of
//! modifications. The function of the naming service to keep track of the
//! evolution of the topological entities as long as some computation scheme
//! unfolds. In the simplest form, the naming service is simply a history graph
//! whose roots are associated with some persistent names. In a more rich form,
//! the naming service may also hold an explicit topology graph whose attributes
//! are the names of the topological entities.
//!
//! The idea of the naming service is throughly discussed at
//!
//! [Kripac, J. 1997. A mechanism for persistently naming topological
//!  entities in history-based parametric solid models. Computer-Aided
//!  Design 29, 113-122.]
//!
//! Keeping the explicit topology graph updated is actually an overkill, so it
//! should be done during the maintenance sessions only. For giving sub-shapes
//! their persistent IDs, it is enough to give names to the root items
//! of a history graph. All images can then be traced back to these roots
//! and names can be propagated.
class asiAlgo_Naming : public asiAlgo_History
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_Naming, asiAlgo_History)

public:

  //! \brief Constructs name for the given shape type and its index.
  //!
  //! E.g., this method will create name 'face_10' for the shape type
  //! TopAbs_FACE and shape index 10.
  //!
  //! \param[in] shapeType  shape type.
  //! \param[in] shapeIndex 1-based shape ID.
  //! \return generated name as ASCII string.
  asiAlgo_EXPORT static TCollection_AsciiString
    PrepareName(const TopAbs_ShapeEnum shapeType,
                const int              shapeIndex);

  //! \brief Analyzes the passed name and returns the corresponding shape type.
  //!
  //! \param[in] name shape name in conventional form, e.g., 'face_10'.
  //! \return shape type.
  asiAlgo_EXPORT static TopAbs_ShapeEnum
    ShapeTypeByName(const TCollection_AsciiString& name);

public:

  //! \brief Constructs naming tool.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_Naming(ActAPI_ProgressEntry progress = NULL,
                   ActAPI_PlotterEntry  plotter  = NULL);

public:

  //! \brief Initializes CAD model with names.
  //!
  //! This method should be used to set names for existing topological
  //! elements. Internally it generates unique ASCII string names for all
  //! topological items and populates the working topology graph with
  //! dedicated name attributes.
  //!
  //! \param[in] aag AAG for a B-Rep shape to generate names for. All sub-shapes
  //!                will get names.
  //!
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    InitNames(const Handle(asiAlgo_AAG)& aag);

  //! Attempts to find the rigid index for the given shape.
  //! \param[in] shape shape in question.
  //! \return 1-based persistent index or null if such a shape cannot be found.
  asiAlgo_EXPORT int
    FindRigidId(const TopoDS_Shape& shape) const;

  //! Attempts to find an alive face given its rigid ID.
  //! \param[in] rigidId ID in question.
  //! \return found alive face.
  asiAlgo_EXPORT TopoDS_Face
    FindAliveFace(const int rigidId) const;

  //! Attempts to find an alive edge given its rigid ID.
  //! \param[in] rigidId ID in question.
  //! \return found alive edge.
  asiAlgo_EXPORT TopoDS_Edge
    FindAliveEdge(const int rigidId) const;

  //! Attempts to find an alive vertex given its rigid ID.
  //! \param[in] rigidId ID in question.
  //! \return found alive vertex.
  asiAlgo_EXPORT TopoDS_Vertex
    FindAliveVertex(const int rigidId) const;

  //! Attempts to find alive shape by its name.
  //! \param[in] name conventional shape name which is composed of lowercase
  //!                 shape type, then underscore, then rigid index.
  //! \return shape.
  asiAlgo_EXPORT TopoDS_Shape
    FindAliveShape(const TCollection_AsciiString& name) const;

  //! Generates a conventional name for the passed shape.
  //! \param[in] shape shape in question.
  //! \return generated name.
  asiAlgo_EXPORT TCollection_AsciiString
    GenerateName(const TopoDS_Shape& shape) const;

};

#endif
