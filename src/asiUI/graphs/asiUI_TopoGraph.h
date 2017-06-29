//-----------------------------------------------------------------------------
// Created on: 05 December 2015
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

#ifndef asiUI_TopoGraph_h
#define asiUI_TopoGraph_h

// asiUI includes
#include <asiUI_ViewerPart.h>
#include <asiUI_VtkWindow.h>

// asiAlgo includes
#include <asiAlgo_AAG.h>

// OCCT includes
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
