//-----------------------------------------------------------------------------
// Created on: 05 September 2017
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

#ifndef asiAlgo_TopoKill_h
#define asiAlgo_TopoKill_h

// asiAlgo includes
#include <asiAlgo.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <NCollection_IndexedMap.hxx>

//! Utility for killing topological entities and reduction of topological
//! graph. Such reduction is possible in two ways:
//!
//! - Killing a sub-shape with all its occurrences in the parent shapes.
//! - Replacement of a sub-shape of a certain type with another (existing)
//!   sub-shape of the same type.
//!
//! To use this tool, the client code first invokes AskRemove() or
//! AskReplace() methods which formulate the intention of the client code.
//! Each request may return true of false depending on whether the request
//! is accepted or rejected by the tool. E.g. a request can be rejected if
//! it does not lead to topological reduction. Once the intention is
//! formulated, the client code invokes Apply() method to perform the
//! recorded modification.
class asiAlgo_TopoKill : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_TopoKill, ActAPI_IAlgorithm)

public:

  //! Constructor.
  //! \param[in] masterCAD full CAD model.
  //! \param[in] progress  Progress Notifier.
  //! \param[in] plotter   Imperative Plotter.
  asiAlgo_EXPORT
    asiAlgo_TopoKill(const TopoDS_Shape&  masterCAD,
                     ActAPI_ProgressEntry progress,
                     ActAPI_PlotterEntry  plotter);

public:

  //! Asks for removal of the given sub-shape.
  //! \param[in] subshape sub-shape to remove.
  //! \return true if the request is accepted, false -- otherwise.
  asiAlgo_EXPORT bool
    AskRemove(const TopoDS_Shape& subshape);

  //! Asks for replacement of the given sub-shape {what} with the given
  //! sub-shape {with}. Orientations in the parent shapes are preserved.
  //! \param[in] what sub-shape to replace.
  //! \param[in] with replacement sub-shape.
  //! \return true if the request is accepted, false -- otherwise.
  asiAlgo_EXPORT bool
    AskReplace(const TopoDS_Shape& what,
               const TopoDS_Shape& with);

  //! Checks whether the passed sub-shape is asked either for removal or
  //! for replacement. If so, one of the output Boolean flags is switched on
  //! to precise what actually is asked.
  //!
  //! \param[in]  subshape         sub-shape to check.
  //! \param[in]  checkRecursively indicates whether to check recursively
  //!                              the sub-shapes of the given shape.
  //! \param[out] forRemoval       indicates whether removal is asked.
  //! \param[out] forReplacement   indicates whether replacement is asked.
  //! \return true if anything is asked for the given sub-shape (and optionally
  //!         its children), false -- otherwise.
  asiAlgo_EXPORT bool
    IsAsked(const TopoDS_Shape& subshape,
            const bool          checkRecursively,
            bool&               forRemoval,
            bool&               forReplacement) const;

  //! Checks whether the passed sub-shape is asked either for removal or
  //! for replacement. If so, one of the output Boolean flags is switched on
  //! to precise what actually is asked.
  //!
  //! \param[in]  subshape       sub-shape to check.
  //! \param[out] forRemoval     indicates whether removal is asked.
  //! \param[out] forReplacement indicates whether replacement is asked.
  //! \return true if anything is asked for the given sub-shape,
  //!         false -- otherwise.
  asiAlgo_EXPORT bool
    IsAsked(const TopoDS_Shape& subshape,
            bool&               forRemoval,
            bool&               forReplacement) const;

public:

  //! Performs topological change.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Apply();

public:

  //! \return converted model.
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
  //! \param[in]  root   root topological element to iterate recursively.
  //! \param[out] result newly created topological element.
  asiAlgo_EXPORT void
    buildTopoGraphLevel(const TopoDS_Shape& root,
                        TopoDS_Shape&       result) const;

  //! Constructs a new empty shape of the given type. Shapes are lightweight
  //! containers of data which, according to OCCT rules, are not modified
  //! but re-created whenever necessary. This method is here because
  //! OpenCascade does.not.allow.constructing.shapes.in.one.line.of.code!
  //!
  //! \param[in] type type of shape to construct.
  //! \return new empty shape.
  asiAlgo_EXPORT TopoDS_Shape
    makeShape(const TopAbs_ShapeEnum type) const;

// Input and output:
protected:

  TopoDS_Shape m_master; //!< Master CAD model.
  TopoDS_Shape m_result; //!< Result CAD model.

// Requests:
protected:

  //! Sub-shapes to remove.
  NCollection_IndexedMap<TopoDS_Shape> m_toRemove;

  //! Sub-shapes to replace.
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape> m_toReplace;

};

#endif
