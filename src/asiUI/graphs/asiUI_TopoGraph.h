//-----------------------------------------------------------------------------
// Created on: 05 December 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiUI_TopoGraph_h
#define asiUI_TopoGraph_h

// asiUI includes
#include <asiUI_ViewerPart.h>
#include <asiUI_VtkWindow.h>

// asiAlgo includes
#include <asiAlgo_AAG.h>

// OCCT includes
#include <SelectMgr_EntityOwner.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

// VTK includes
#include <vtkCommand.h>
#include <vtkIntArray.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTextWidget.h>

//! Visualizes topology graph.
class asiUI_TopoGraph : public QObject
{
  Q_OBJECT

public:

  //! Type of graph to visualize (e.g. topology, AAG, etc.).
  enum Regime
  {
    Regime_Full,
    Regime_AAG
  };

public:

  asiUI_EXPORT
    asiUI_TopoGraph(const Handle(asiEngine_Model)& model,
                    asiUI_ViewerPart*              pPartViewer);

  asiUI_EXPORT
    ~asiUI_TopoGraph();

public:

  asiUI_EXPORT void
    Render(const vtkSmartPointer<vtkGraph>& graph,
           const TopoDS_Shape&              shape,
           const Regime                     regime);

  asiUI_EXPORT void
    Render(const TopoDS_Shape&               shape,
           const TopTools_IndexedMapOfShape& selectedFaces,
           const Regime                      regime,
           const TopAbs_ShapeEnum            leafType);

  asiUI_EXPORT void
    RenderFull(const TopoDS_Shape&    shape,
               const TopAbs_ShapeEnum leafType);

  asiUI_EXPORT void
    RenderAdjacency(const TopoDS_Shape&               shape,
                    const TopTools_IndexedMapOfShape& selectedFaces);

  asiUI_EXPORT void
    RenderEventCallback();

public:

  //! Allows to initialize presentable graph with externally constructed AAG.
  void SetAAG(const Handle(asiAlgo_AAG)& aag) { m_aag = aag; }

protected:

  asiUI_EXPORT vtkSmartPointer<vtkGraph>
    convertToGraph(const TopoDS_Shape&               shape,
                   const TopTools_IndexedMapOfShape& selectedFaces,
                   const Regime                      regime,
                   const TopAbs_ShapeEnum            leafType);

  asiUI_EXPORT void
    buildRecursively(const TopoDS_Shape&             rootShape,
                     const vtkIdType                 rootId,
                     const TopAbs_ShapeEnum          leafType,
                     vtkMutableDirectedGraph*        pDS,
                     vtkStringArray*                 pLabelArr,
                     vtkStringArray*                 pGroupArr,
                     vtkIntArray*                    pIdsArr,
                     TopTools_DataMapOfShapeInteger& shapeVertices);

protected slots:

  void onViewerClosed();
  void onVertexPicked(const int, const TopAbs_ShapeEnum, const vtkIdType);

protected:

  asiUI_VtkWindow*        m_pWidget;       //!< Widget.
  vtkTextWidget*          m_textWidget;    //!< Text.
  vtkTextWidget*          m_summaryWidget; //!< Summary.
  Handle(asiAlgo_AAG)     m_aag;           //!< AAG.
  Handle(asiEngine_Model) m_model;         //!< Data Model instance.
  asiUI_ViewerPart*       m_partViewer;    //!< Part viewer.
  //
  TopTools_IndexedMapOfShape m_faces;     //!< Faces.
  TopTools_IndexedMapOfShape m_edges;     //!< Edges.
  TopTools_IndexedMapOfShape m_vertices;  //!< Vertices.
  TopTools_IndexedMapOfShape m_subShapes; //!< All sub-shapes.

};

#endif
