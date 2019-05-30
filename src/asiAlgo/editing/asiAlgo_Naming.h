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
#include <asiAlgo_TopoGraph.h>

// Active Data includes
#include <ActAPI_IProgressNotifier.h>

//-----------------------------------------------------------------------------

//! \brief Naming service for a B-Rep part.
class asiAlgo_Naming : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_Naming, Standard_Transient)

public:

  //! \brief Constructs naming tool which holds history only.
  //! \param[in] history  history to store.
  //! \param[in] progress progress notifier.
  asiAlgo_EXPORT
    asiAlgo_Naming(const Handle(asiAlgo_History)& history,
                   ActAPI_ProgressEntry           progress = NULL);

  //! \brief Constructs naming tool from topology graph.
  //! \param[in] topograph topology graph which allows for a more formal
  //!                      view on a CAD-model together with caching its
  //!                      sub-shapes under serial indices.
  //! \param[in] progress  progress notifier.
  asiAlgo_EXPORT
    asiAlgo_Naming(const Handle(asiAlgo_TopoGraph)& topograph,
                   ActAPI_ProgressEntry             progress = NULL);

  //! \brief Constructs naming tool from B-Rep shape.
  //! \param[in] shape    master CAD shape to build topology graph from.
  //! \param[in] progress progress notifier.
  asiAlgo_EXPORT
    asiAlgo_Naming(const TopoDS_Shape&  shape,
                   ActAPI_ProgressEntry progress = NULL);

public:

  //! \brief Initializes CAD model with names.
  //!
  //! This method should be used to set names for existing topological
  //! elements. Internally it generates unique ASCII string names for all
  //! topological items and populates the working topology graph with
  //! dedicated name attributes.
  //!
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    InitNames();

  //! \brief Constructs name for the given shape type and its index.
  //!
  //! E.g., this method will create name 'face_10' for the shape type
  //! TopAbs_FACE and shape index 10. This method will not populate the
  //! naming service internally but only generate a name for further
  //! referencing.
  //!
  //! \param[in] shapeType  shape type.
  //! \param[in] shapeIndex 1-based shape ID.
  //! \return generated name as ASCII string.
  //!
  //! \sa GenerateName() method for indexing the working model with names
  //!     and populating the service internally.
  asiAlgo_EXPORT static TCollection_AsciiString
    PrepareName(const TopAbs_ShapeEnum shapeType,
                const int              shapeIndex);

  //! \brief Generates a unique name for the passed sub-shape.
  //! \param[in] shape shape to generate a name for.
  //! \return generated name as ASCII string.
  asiAlgo_EXPORT TCollection_AsciiString
    GenerateName(const TopoDS_Shape& shape);

  //! Actualizes the naming attributes stored in the topology graph according
  //! to the current state of history. This method should be called in a
  //! modeling algorithm which takes care of persistent naming.
  //! \param[in] newShape modified shape to actualize naming on.
  //! \return false if actualization is impossible (e.g. non-unique names are used).
  asiAlgo_EXPORT bool
    Actualize(const TopoDS_Shape& newShape);

public:

  //! \return topology graph.
  const Handle(asiAlgo_TopoGraph)& GetTopoGraph() const
  {
    return m_topograph;
  }

  //! \return history.
  const Handle(asiAlgo_History)& GetHistory() const
  {
    return m_history;
  }

  //! Accessor for shape by its name.
  //! \param[in] name name of the shape to find.
  //! \return transient pointer by persistent name.
  TopoDS_Shape GetShape(const TCollection_AsciiString& name) const
  {
    if ( !m_namedAliveShapes.IsBound(name) )
      return TopoDS_Shape();

    return m_namedAliveShapes(name);
  }

  //! Attempts to find a name registered for the passed shape.
  //! \param[in]  shape shape in question.
  //! \param[out] name  found name.
  //! \return false if name cannot be found.
  bool FindName(const TopoDS_Shape&      shape,
                TCollection_AsciiString& name) const
  {
    for ( NCollection_DataMap<TCollection_AsciiString, TopoDS_Shape>::Iterator
          it(m_namedAliveShapes); it.More(); it.Next() )
    {
      if ( it.Value().IsPartner(shape) )
      {
        name = it.Key();
        return true;
      }
    }

    return false;
  }

protected:

  //! Helper (internal) method to transfer naming attribute from old shape
  //! to its images.
  //! \param[in] images       images of the old shape to set naming attribute for.
  //! \param[in] newTopograph topology graph where images can be found.
  //! \param[in] attr2Pass    attribute to pass.
  //! \param[in] isGenerated  whether "generated" evolution is processed.
  asiAlgo_EXPORT void
    actualizeImages(const std::vector<TopoDS_Shape>& images,
                    const Handle(asiAlgo_TopoGraph)& newTopograph,
                    const Handle(asiAlgo_TopoAttr)&  attr2Pass,
                    const bool                       isGenerated);

  //! Helper (internal) method to pass naming attribute to unmodified
  //! topological entity.
  //! \param[in] shape        (sub-)shape in question.
  //! \param[in] newTopograph topology graph where images can be found.
  //! \param[in] attr2Pass    attribute to pass.
  asiAlgo_EXPORT void
    passIntact(const TopoDS_Shape&              shape,
               const Handle(asiAlgo_TopoGraph)& newTopograph,
               const Handle(asiAlgo_TopoAttr)&  attr2Pass);

  //! Adds named shape to the internal map for fast access.
  //! \param[in] name  string to use as a name. This name should be unique.
  //! \param[in] shape shape to register.
  //! \return false if the passed name is not unique and cannot be added so.
  asiAlgo_EXPORT bool
    registerNamedShape(const TCollection_AsciiString& name,
                       const TopoDS_Shape&            shape);

protected:

  /** @name Core members
   *  Members constituting core of the naming service.
   */
  //@{

  //! Formal topology graph.
  Handle(asiAlgo_TopoGraph) m_topograph;

  //! History of modification.
  Handle(asiAlgo_History) m_history;

  //! The used indices distributed by shape type.
  NCollection_DataMap<TopAbs_ShapeEnum, int> m_nameIds;

  //! Map of unique names and their corresponding shapes. This is actually
  //! a collection of alive shapes.
  NCollection_DataMap<TCollection_AsciiString, TopoDS_Shape> m_namedAliveShapes;

  //! Progress notification tool.
  ActAPI_ProgressEntry m_progress;

  //@}

};

#endif
