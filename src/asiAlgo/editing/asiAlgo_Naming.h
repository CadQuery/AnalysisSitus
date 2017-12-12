//-----------------------------------------------------------------------------
// Created on: 27 November 2017
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

#ifndef asiAlgo_Naming_h
#define asiAlgo_Naming_h

// asiAlgo includes
#include <asiAlgo_History.h>
#include <asiAlgo_TopoGraph.h>

// Active Data includes
#include <ActAPI_IProgressNotifier.h>

//! \brief Naming service for a B-Rep part.
class asiAlgo_Naming : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_Naming, Standard_Transient)

public:

  //! \brief Constructs naming tool from topology graph.
  //! \param[in] topograph topology graph which allows for a more formal
  //!                      view on a CAD-model together with caching its
  //!                      sub-shapes under serial indices.
  //! \param[in] progress  progress notifier.
  asiAlgo_EXPORT
    asiAlgo_Naming(const Handle(asiAlgo_TopoGraph)& topograph,
                   ActAPI_ProgressEntry             progress);

  //! \brief Constructs naming tool from B-Rep shape.
  //! \param[in] shape    master CAD shape to build topology graph from.
  //! \param[in] progress progress notifier.
  asiAlgo_EXPORT
    asiAlgo_Naming(const TopoDS_Shape&  shape,
                   ActAPI_ProgressEntry progress);

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

  //! \brief Generates a unique name for the passed sub-shape.
  //! \param[in] shape shape to generate a name for.
  //! \return generated name as ASCII string.
  asiAlgo_EXPORT TCollection_AsciiString
    GenerateName(const TopoDS_Shape& shape);

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

protected:

  /** @name Core members
   *  Members constituting core of the naming service.
   */
  //@{
  Handle(asiAlgo_TopoGraph) m_topograph; //!< Formal topology graph.
  Handle(asiAlgo_History)   m_history;   //!< History of modification.
  //@}

  //! Progress notification tool.
  ActAPI_ProgressEntry m_progress;

  //! The used indices distributed by shape type.
  NCollection_DataMap<TopAbs_ShapeEnum, int> m_nameIds;

};

#endif
