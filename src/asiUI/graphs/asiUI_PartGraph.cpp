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

// Own include
#include <asiUI_PartGraph.h>

// asiUI includes
#include <asiUI_AAGAdaptor.h>
#include <asiUI_TopoGraphAdaptor.h>
#include <asiUI_PartGraphItem.h>

// asiVisu includes
#include <asiVisu_Utils.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// asiEngine includes
#include <asiEngine_Part.h>

// OCCT includes
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>

// VTK includes
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkCamera.h>
#include <vtkContextActor.h>
#include <vtkContextInteractorStyle.h>
#include <vtkContextScene.h>
#include <vtkContextTransform.h>
#include <vtkGraphLayout.h>
#include <vtkNew.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSimple2DLayoutStrategy.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkTextRepresentation.h>
#include <vtkTextWidget.h>

#define LEGEND_TITLE_ACCESSORY "Topology graph"
#define LEGEND_TITLE_ADJACENCY "AAG"

//-----------------------------------------------------------------------------

#define VTK_CREATE(Type, Name) \
  vtkSmartPointer<Type> Name = vtkSmartPointer<Type>::New()

//-----------------------------------------------------------------------------

//! Constructor.
//! \param[in] model       Data Model instance.
//! \param[in] pPartViewer part viewer.
asiUI_PartGraph::asiUI_PartGraph(const Handle(asiEngine_Model)& model,
                                 asiUI_ViewerPart*              pPartViewer)
: m_textWidget    (NULL),
  m_summaryWidget (NULL),
  m_model         (model),
  m_partViewer    (pPartViewer)
{}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_PartGraph::~asiUI_PartGraph()
{}

//-----------------------------------------------------------------------------

//! Renders graph.
//! \param graph  [in] VTK presentable graph.
//! \param shape  [in] master shape.
//! \param regime [in] kind of graph to render.
void asiUI_PartGraph::Render(const vtkSmartPointer<vtkGraph>& graph,
                             const TopoDS_Shape&              shape,
                             const Regime                     regime)
{
  /* ===================================
   *  Prepare structures for attributes
   * =================================== */

  // Layout strategy
  vtkNew<vtkSimple2DLayoutStrategy> simple2DStrategy;
  simple2DStrategy->SetIterationsPerLayout(10);

  // Layout
  vtkSmartPointer<vtkGraphLayout> graphLayout = vtkSmartPointer<vtkGraphLayout>::New();
  graphLayout->SetInputData(graph);
  graphLayout->SetLayoutStrategy( simple2DStrategy.GetPointer() );
  graphLayout->Update();

  // Graph item
  vtkSmartPointer<asiUI_PartGraphItem> graphItem = vtkSmartPointer<asiUI_PartGraphItem>::New();
  graphItem->SetGraph( graphLayout->GetOutput() );

  connect( graphItem, SIGNAL( vertexPicked(const int, const TopAbs_ShapeEnum, const vtkIdType) ),
           this,      SLOT( onVertexPicked(const int, const TopAbs_ShapeEnum, const vtkIdType) ) );

  // Context transform
  vtkSmartPointer<vtkContextTransform> trans = vtkSmartPointer<vtkContextTransform>::New();
  trans->SetInteractive(true);
  trans->AddItem(graphItem);

  // Context actor
  vtkSmartPointer<vtkContextActor> actor = vtkSmartPointer<vtkContextActor>::New();
  actor->GetScene()->AddItem(trans);

  /* ===============================================
   *  Prepare and initialize interaction facilities
   * =============================================== */

  // Renderer
  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
  renderer->SetBackground(0.15, 0.15, 0.15);

  // Render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderer->AddActor(actor);

  // Context interactor style
  vtkNew<vtkContextInteractorStyle> interactorStyle;
  interactorStyle->SetScene( actor->GetScene() );

  // Interactor
  vtkNew<QVTKInteractor> interactor;
  interactor->SetInteractorStyle( interactorStyle.GetPointer() );
  interactor->SetRenderWindow( renderWindow.GetPointer() );

  // Create Qt window. NOTICE that interactor should be already given to
  // the render window at this stage
  m_pWidget = new asiUI_VtkWindow();
  m_pWidget->SetRenderWindow( renderWindow.GetPointer() );

  connect( m_pWidget, SIGNAL( windowClosed() ), this, SLOT( onViewerClosed() ) );

  /* ========================
   *  Add legend and summary
   * ======================== */

  // Legend
  m_textWidget = vtkTextWidget::New();
  vtkTextRepresentation* textRep = vtkTextRepresentation::SafeDownCast( m_textWidget->GetRepresentation() );
  textRep->GetPositionCoordinate()->SetValue(0.2, 0.01);
  textRep->GetPosition2Coordinate()->SetValue(0.5, 0.06);
  m_textWidget->SelectableOff();
  //
  vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();
  textRep->SetTextActor(textActor);
  //
  m_textWidget->GetTextActor()->SetInput( regime == Regime_Topology ? LEGEND_TITLE_ACCESSORY : LEGEND_TITLE_ADJACENCY );
  m_textWidget->SetInteractor( m_pWidget->GetInteractor() );
  m_textWidget->SetDefaultRenderer( renderer );
  m_textWidget->SetCurrentRenderer( renderer );

  // Shape summary
  m_summaryWidget = vtkTextWidget::New();
  asiVisu_Utils::InitTextWidget(m_summaryWidget);
  //
  TCollection_AsciiString shapeInfo;
  asiAlgo_Utils::ShapeSummary(shape, shapeInfo);
  m_summaryWidget->GetTextActor()->SetInput( shapeInfo.ToCString() );
  //
  m_summaryWidget->SetInteractor      ( m_pWidget->GetInteractor() );
  m_summaryWidget->SetDefaultRenderer ( renderer );
  m_summaryWidget->SetCurrentRenderer ( renderer );

  /* ===================
   *  Start interaction
   * =================== */

  renderWindow->SetLineSmoothing(true);
  renderWindow->SetWindowName( regime == Regime_Topology ? "Topology Graph" : "Face Adjacency Graph" );
  //
  graphItem->StartLayoutAnimation( m_pWidget->GetInteractor() );
  //
  m_pWidget->GetInteractor()->Initialize();
  m_pWidget->resize(400, 400);
  m_pWidget->show();

  // Set callback on rendering
  m_pWidget->GetRenderWindow()->AddObserver(vtkCommand::RenderEvent, this, &asiUI_PartGraph::RenderEventCallback);
}

//-----------------------------------------------------------------------------

//! Callback to adjust text widgets.
void asiUI_PartGraph::RenderEventCallback()
{
  if ( !m_textWidget->GetEnabled() )
    m_textWidget->On();

  if ( !m_summaryWidget->GetEnabled() )
    m_summaryWidget->On();
}

//-----------------------------------------------------------------------------

//! Renders part graph in the requested regime.
//! \param shape         [in] target shape.
//! \param selectedFaces [in] selected faces.
//! \param regime        [in] regime of interest.
//! \param leafType      [in] target leaf type for FULL regime.
void asiUI_PartGraph::Render(const TopoDS_Shape&               shape,
                             const TopTools_IndexedMapOfShape& selectedFaces,
                             const Regime                      regime,
                             const TopAbs_ShapeEnum            leafType)
{
  

  // Populate graph data from topology graph
  vtkSmartPointer<vtkGraph> graph = this->convertToGraph(shape, selectedFaces, regime, leafType);

  // Render VTK graph
  this->Render(graph, shape, regime);
}

//-----------------------------------------------------------------------------

//! Renders topology graph.
//! \param shape    [in] target shape.
//! \param leafType [in] target leaf type.
void asiUI_PartGraph::RenderTopology(const TopoDS_Shape&    shape,
                                     const TopAbs_ShapeEnum leafType)
{
  this->Render(shape, TopTools_IndexedMapOfShape(), Regime_Topology, leafType);
}

//-----------------------------------------------------------------------------

//! Renders face adjacency graph.
//! \param shape         [in] target shape.
//! \param selectedFaces [in] selected faces.
void asiUI_PartGraph::RenderAdjacency(const TopoDS_Shape&               shape,
                                      const TopTools_IndexedMapOfShape& selectedFaces)
{
  this->Render(shape, selectedFaces, Regime_AAG, TopAbs_SHAPE);
}

//-----------------------------------------------------------------------------

//! Builds one or another graph (depending on the desired regime).
//! \param shape         [in] master model.
//! \param selectedFaces [in] optional selected faces.
//! \param regime        [in] desired regime.
//! \param leafType      [in] leaf type for FULL regime.
//! \return graph instance.
vtkSmartPointer<vtkGraph>
  asiUI_PartGraph::convertToGraph(const TopoDS_Shape&               shape,
                                  const TopTools_IndexedMapOfShape& selectedFaces,
                                  const Regime                      regime,
                                  const TopAbs_ShapeEnum            leafType)
{
  vtkSmartPointer<vtkGraph> result;
  //
  if ( regime == Regime_Topology )
  {
    m_topoGraph = new asiAlgo_TopoGraph(shape);
    vtkSmartPointer<vtkMutableDirectedGraph>
      directed = asiUI_TopoGraphAdaptor::Convert(m_topoGraph, leafType);
    //
    result = directed;
  }
  else if ( regime == Regime_AAG )
  {
    m_aag = new asiAlgo_AAG(shape, selectedFaces);
    vtkSmartPointer<vtkMutableUndirectedGraph>
      undirected = asiUI_AAGAdaptor::Convert(m_aag);
    //
    result = undirected;
  }
  else
    Standard_ProgramError::Raise("Unexpected regime for graph visualization");

  return result;
}

//-----------------------------------------------------------------------------

//! Reaction on closing the viewer.
void asiUI_PartGraph::onViewerClosed()
{
  // NOTE: the important point is to remove widget after all items which
  //       may listen to it
  m_textWidget->Delete();
  m_summaryWidget->Delete();

  delete m_pWidget;
  delete this;
}

//-----------------------------------------------------------------------------

//! Reaction on vertex picking.
//! \param subShapeId [in] sub-shape ID.
//! \param shapeType  [in] sub-shape type.
//! \param vid        [in] graph vertex ID.
void asiUI_PartGraph::onVertexPicked(const int              subShapeId,
                                     const TopAbs_ShapeEnum shapeType,
                                     const vtkIdType        vid)
{
  asiVisu_NotUsed(vid);

  // Prepare maps of sub-shapes
  const TopTools_IndexedMapOfShape&
    mapOfFaces = m_aag.IsNull() ? m_topoGraph->GetMapOfFaces() : m_aag->GetMapOfFaces();
  //
  const TopTools_IndexedMapOfShape&
    mapOfEdges = m_aag.IsNull() ? m_topoGraph->GetMapOfEdges() : m_aag->GetMapOfEdges();
  //
  const TopTools_IndexedMapOfShape&
    mapOfVertices = m_aag.IsNull() ? m_topoGraph->GetMapOfVertices() : m_aag->GetMapOfVertices();
  //
  const TopTools_IndexedMapOfShape&
    mapOfSubShapes = m_aag.IsNull() ? m_topoGraph->GetMapOfSubShapes() : m_aag->GetMapOfSubShapes();

  if ( subShapeId > 0 && m_partViewer )
  {
    // Get sub-shape by index
    TopoDS_Shape subShape;
    if ( shapeType == TopAbs_FACE )
      subShape = mapOfFaces.FindKey(subShapeId);
    else if ( shapeType == TopAbs_EDGE )
      subShape = mapOfEdges.FindKey(subShapeId);
    else if ( shapeType == TopAbs_VERTEX )
      subShape = mapOfVertices.FindKey(subShapeId);
    else
      subShape = mapOfSubShapes(subShapeId);

    // Highlight in the main viewer
    TopTools_IndexedMapOfShape selected;
    selected.Add(subShape);
    //
    asiEngine_Part( m_model,
                    m_partViewer->PrsMgr() ).HighlightSubShapes(selected);
  }
}
