//-----------------------------------------------------------------------------
// Created on: 21 March 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiEngine_Part_h
#define asiEngine_Part_h

// A-Situs includes
#include <asiEngine_Model.h>

// A-Situs (visualization) includes
#include <asiVisu_PrsManager.h>

// OCCT includes
#include <TopTools_IndexedMapOfShape.hxx>

//! Data Model API for geometric part.
class asiEngine_Part
{
public:

  //! ctor.
  //! \param model [in] Data Model instance.
  asiEngine_Part(const Handle(asiEngine_Model)&             model,
                 const vtkSmartPointer<asiVisu_PrsManager>& prsMgr)
  //
  : m_model(model), m_prsMgr(prsMgr) {}

public:

  asiEngine_EXPORT Handle(asiData_PartNode)
    Create();

  asiEngine_EXPORT void
    Update(const TopoDS_Shape& model);

  asiEngine_EXPORT void
    Clean();

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

  Handle(asiEngine_Model)             m_model;  //!< Data Model instance.
  vtkSmartPointer<asiVisu_PrsManager> m_prsMgr; //!< Presentation Manager.

};

#endif
