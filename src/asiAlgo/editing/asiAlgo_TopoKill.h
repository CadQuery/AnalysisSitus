//-----------------------------------------------------------------------------
// Created on: 05 September 2017
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

#ifndef asiAlgo_TopoKill_h
#define asiAlgo_TopoKill_h

// asiAlgo includes
#include <asiAlgo_History.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <NCollection_IndexedMap.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_ShapeMapHasher.hxx>

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

  //! \return modification history.
  const Handle(asiAlgo_History)& GetHistory() const
  {
    return m_history;
  }

  //! \brief Sets externally defined history.
  //!
  //! The passed history will not be cleared, so that a pipeline of
  //! modeling tools will grow its history graph as long as the history
  //! object is continuously passed from one tool to another.
  //!
  //! \param[in] history history to set.
  void SetHistory(const Handle(asiAlgo_History)& history)
  {
    m_history = history;
  }

  //! Activates error state.
  void SetErrorStateOn()
  {
    m_bErrorState = true;
  }

  //! Deactivates error state.
  void SetErrorStateOff()
  {
    m_bErrorState = false;
  }

  //! \return error state flag.
  bool IsErrorState() const
  {
    return m_bErrorState;
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
                        TopoDS_Shape&       result);

  //! Internal method which allows to choose properly the topological
  //! orientation for the new edge which is a replacement for the old one.
  //! Since the new edge may have opposite parameterization comparing to
  //! the old edge, simply reusing the old orientation will not work, so
  //! a more sophisticated analysis is necessary.
  //!
  //! \param[in]  oldEdge old edge.
  //! \param[in]  newEdge new edge.
  //! \param[out] ori     chosen orientation.
  //! \return false if this method cannot choose orientation due to faulty
  //!         edge definitions.
  asiAlgo_EXPORT bool
    chooseOri(const TopoDS_Edge&  oldEdge,
              const TopoDS_Edge&  newEdge,
              TopAbs_Orientation& ori) const;

// Input and output:
protected:

  TopoDS_Shape            m_master;  //!< Master CAD model.
  TopoDS_Shape            m_result;  //!< Result CAD model.
  Handle(asiAlgo_History) m_history; //!< History of modification.

  //! Indicates whether some error occurred, so the caller algorithm
  //! (Normalizer) should gracefully halt.
  bool m_bErrorState;

// Requests:
protected:

  //! Hasher which does not take into account neither locations nor
  //! orientations of shapes. Our killer is extremely cruel in this regard...
  class t_partner_hasher
  {
  public:

    static int HashCode(const TopoDS_Shape& S, const int Upper)
    {
      const int I  = (int) ptrdiff_t( S.TShape().operator->() );
      const int HS = ::HashCode(I, Upper);
      //
      return HS;
    }

    static bool IsEqual(const TopoDS_Shape& S1, const TopoDS_Shape& S2)
    {
      return S1.IsPartner(S2);
    }
  };

  // NOTICE: in both maps below, an unoriented/unlocated hasher is used. This is because
  //         the nodes in a formal topology graph do not encode orientations/locations.
  //         In formal topology graph, orientation/location is a property of an arc, i.e.
  //         it is a property of INCLUSION of one shape into another. Since
  //         in the topological killer we work only with nodes, we do not
  //         need to distinguish between differently oriented entities.

  //! Sub-shapes to remove.
  NCollection_IndexedMap<TopoDS_Shape, t_partner_hasher> m_toRemove;

  //! Sub-shapes to replace.
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, t_partner_hasher> m_toReplace;

  //! Newly created sub-shapes.
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, t_partner_hasher> m_newElements;

};

#endif
