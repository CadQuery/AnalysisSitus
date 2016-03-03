//-----------------------------------------------------------------------------
// Created on: 05 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_topo_graph_h
#define visu_topo_graph_h

// Visualization includes
#include <visu_common.h>

// GUI includes
#include <gui_vtk_window.h>

// Geometry includes
#include <geom_aag.h>

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
class visu_topo_graph : public QObject
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

  visu_topo_graph();
  ~visu_topo_graph();

public:

  void Render              (const TopoDS_Shape& shape, const TopTools_ListOfShape& selectedFaces, const Regime regime, const TopAbs_ShapeEnum leafType);
  void RenderFull          (const TopoDS_Shape& shape, const TopAbs_ShapeEnum leafType);
  void RenderAdjacency     (const TopoDS_Shape& shape, const TopTools_ListOfShape& selectedFaces);
  void RenderEventCallback ();

protected:

  vtkSmartPointer<vtkGraph>
    convertToGraph(const TopoDS_Shape&         shape,
                   const TopTools_ListOfShape& selectedFaces,
                   const Regime                regime,
                   const TopAbs_ShapeEnum      leafType);

  void buildRecursively(const TopoDS_Shape&             rootShape,
                        const vtkIdType                 rootId,
                        const TopAbs_ShapeEnum          leafType,
                        vtkMutableDirectedGraph*        pDS,
                        vtkStringArray*                 pLabelArr,
                        vtkStringArray*                 pGroupArr,
                        TopTools_DataMapOfShapeInteger& shapeVertices);

protected slots:

  void onViewerClosed();
  void onVertexPicked(const vtkIdType);

protected:

  gui_vtk_window*                m_pWidget;
  vtkSmartPointer<vtkTextWidget> m_textWidget;
  vtkSmartPointer<vtkTextWidget> m_summaryWidget;
  Handle(geom_aag)               m_aag;
  Handle(SelectMgr_EntityOwner)  m_prevHighlighted;

};

#endif
