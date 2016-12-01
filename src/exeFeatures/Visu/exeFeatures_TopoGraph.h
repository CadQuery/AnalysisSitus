//-----------------------------------------------------------------------------
// Created on: 05 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeFeatures_TopoGraph_h
#define exeFeatures_TopoGraph_h

// Visualization includes
#include <asiVisu.h>

// GUI includes
#include <asiUI_VtkWindow.h>

// Feature includes
#include <feature_aag.h>

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
class exeFeatures_TopoGraph : public QObject
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

  exeFeatures_TopoGraph();
  ~exeFeatures_TopoGraph();

public:

  void Render              (const vtkSmartPointer<vtkGraph>& graph, const TopoDS_Shape& shape, const Regime regime);
  void Render              (const TopoDS_Shape& shape, const TopTools_IndexedMapOfShape& selectedFaces, const Regime regime, const TopAbs_ShapeEnum leafType);
  void RenderFull          (const TopoDS_Shape& shape, const TopAbs_ShapeEnum leafType);
  void RenderAdjacency     (const TopoDS_Shape& shape, const TopTools_IndexedMapOfShape& selectedFaces);
  void RenderEventCallback ();

public:

  //! Allows to initialize presentable graph with externally constructed AAG.
  void SetAAG(const Handle(feature_aag)& aag) { m_aag = aag; }

protected:

  vtkSmartPointer<vtkGraph>
    convertToGraph(const TopoDS_Shape&               shape,
                   const TopTools_IndexedMapOfShape& selectedFaces,
                   const Regime                      regime,
                   const TopAbs_ShapeEnum            leafType);

  void buildRecursively(const TopoDS_Shape&             rootShape,
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

  asiUI_VtkWindow*    m_pWidget;
  vtkTextWidget*      m_textWidget;
  vtkTextWidget*      m_summaryWidget;
  Handle(feature_aag) m_aag;
  //
  TopTools_IndexedMapOfShape m_faces;    //!< Faces.
  TopTools_IndexedMapOfShape m_edges;    //!< Edges.
  TopTools_IndexedMapOfShape m_vertices; //!< Vertices.

};

#endif
