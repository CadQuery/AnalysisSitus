//-----------------------------------------------------------------------------
// Created on: 21 March 2016
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

#ifndef asiEngine_Part_h
#define asiEngine_Part_h

// asiEngine includes
#include <asiEngine_Model.h>

// asiVisu includes
#include <asiVisu_PrsManager.h>

// OCCT includes
#include <TopTools_IndexedMapOfShape.hxx>

//! Data Model API for geometric part.
class asiEngine_Part
{
public:

  //! ctor.
  //! \param model    [in] Data Model instance.
  //! \param prsMgr   [in] Presentation Manager for visual commands.
  //! \param progress [in] progress notifier.
  //! \param plotter  [in] imperative plotter.
  asiEngine_Part(const Handle(asiEngine_Model)&             model,
                 const vtkSmartPointer<asiVisu_PrsManager>& prsMgr,
                 ActAPI_ProgressEntry                       progress = NULL,
                 ActAPI_PlotterEntry                        plotter  = NULL)
  //
  : m_model(model), m_prsMgr(prsMgr), m_progress(progress), m_plotter(plotter) {}

  //! ctor.
  //! \param model [in] Data Model instance.
  asiEngine_Part(const Handle(asiEngine_Model)& model)
  //
  : m_model(model) {}

public:

  asiEngine_EXPORT Handle(asiData_PartNode)
    CreatePart();

  asiEngine_EXPORT void
    Update(const TopoDS_Shape& model,
           const bool          doResetTessParams = false);

  asiEngine_EXPORT void
    InitializeNaming();

  asiEngine_EXPORT void
    BuildBVH();

  asiEngine_EXPORT void
    Clean(const bool resetNaming);

  asiEngine_EXPORT TopoDS_Shape
    GetShape();

  asiEngine_EXPORT void
    GetSubShapeIndices(const TopTools_IndexedMapOfShape& subShapes,
                       TColStd_PackedMapOfInteger&       indices);

  asiEngine_EXPORT void
    GetSubShapeIndices(const TopTools_IndexedMapOfShape& subShapes,
                       TColStd_PackedMapOfInteger&       faceIndices,
                       TColStd_PackedMapOfInteger&       edgeIndices,
                       TColStd_PackedMapOfInteger&       vertexIndices);

  asiEngine_EXPORT void
    GetSubShapeIndicesByFaceIndices(const TColStd_PackedMapOfInteger& faceIndices,
                                    TColStd_PackedMapOfInteger&       indices);

  asiEngine_EXPORT void
    GetSubShapeIndicesByEdgeIndices(const TColStd_PackedMapOfInteger& edgeIndices,
                                    TColStd_PackedMapOfInteger&       indices);

  asiEngine_EXPORT void
    HighlightFace(const int     faceIndex,
                  const QColor& color);

  asiEngine_EXPORT void
    HighlightFaces(const TColStd_PackedMapOfInteger& faceIndices,
                   const QColor&                     color);

  asiEngine_EXPORT void
    HighlightEdges(const TColStd_PackedMapOfInteger& edgeIndices,
                   const QColor&                     color);

  asiEngine_EXPORT void
    HighlightSubShapes(const TColStd_PackedMapOfInteger& subShapeIndices,
                       const asiVisu_SelectionMode       selMode);

  asiEngine_EXPORT void
    HighlightSubShapes(const TColStd_PackedMapOfInteger& subShapeIndices,
                       const int                         color,
                       const asiVisu_SelectionMode       selMode);

  asiEngine_EXPORT void
    HighlightSubShapes(const TopTools_IndexedMapOfShape& subShapes);

  asiEngine_EXPORT void
    HighlightSubShapes(const TopTools_IndexedMapOfShape& subShapes,
                       const int                         color);

  asiEngine_EXPORT void
    GetHighlightedSubShapes(TopTools_IndexedMapOfShape& subShapes);

  asiEngine_EXPORT void
    GetHighlightedFaces(TColStd_PackedMapOfInteger& faceIndices);

  asiEngine_EXPORT void
    GetHighlightedEdges(TColStd_PackedMapOfInteger& edgeIndices);

protected:

  Handle(asiEngine_Model)             m_model;    //!< Data Model instance.
  vtkSmartPointer<asiVisu_PrsManager> m_prsMgr;   //!< Presentation Manager.
  ActAPI_ProgressEntry                m_progress; //!< Progress notifier.
  ActAPI_PlotterEntry                 m_plotter;  //!< Imperative plotter.

};

#endif
