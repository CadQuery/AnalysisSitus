//-----------------------------------------------------------------------------
// Created on: 05 December 2015
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
#pragma warning(push, 0)
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
#pragma warning(pop)

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
: 
  m_pWidget       (NULL),
  m_textWidget    (NULL),
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
//! \param[in] graph             VTK presentable graph.
//! \param[in] shape             master shape.
//! \param[in] regime            kind of graph to render.
//! \param[in] colorizeLocations indicates whether to colorize graph nodes
//!                              in accordance with the locations of their
//!                              corresponding sub-shapes.
void asiUI_PartGraph::Render(const vtkSmartPointer<vtkGraph>& graph,
                             const TopoDS_Shape&              shape,
                             const Regime                     regime,
                             const bool                       colorizeLocations)
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
  vtkSmartPointer<asiUI_PartGraphItem>
    graphItem = vtkSmartPointer<asiUI_PartGraphItem>::New();
  //
  graphItem->SetGraph               ( graphLayout->GetOutput() );
  graphItem->SetColorizeByLocations ( colorizeLocations );

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
//! \param[in] shape             target shape.
//! \param[in] selectedFaces     selected faces.
//! \param[in] regime            regime of interest.
//! \param[in] leafType          target leaf type for FULL regime.
//! \param[in] colorizeLocations indicates whether to colorize graph nodes
//!                              in accordance with the locations of their
//!                              corresponding sub-shapes.
void asiUI_PartGraph::Render(const TopoDS_Shape&               shape,
                             const TopTools_IndexedMapOfShape& selectedFaces,
                             const Regime                      regime,
                             const TopAbs_ShapeEnum            leafType,
                             const bool                        colorizeLocations)
{
  // Populate graph data from topology graph
  vtkSmartPointer<vtkGraph>
    graph = this->convertToGraph(shape,
                                 NULL,
                                 selectedFaces,
                                 regime,
                                 leafType);

  // Render VTK graph
  this->Render(graph, shape, regime, colorizeLocations);
}

//-----------------------------------------------------------------------------

//! Renders topology graph.
//! \param[in] shape             target shape.
//! \param[in] leafType          target leaf type.
//! \param[in] colorizeLocations indicates whether to colorize graph nodes
//!                              in accordance with the locations of their
//!                              corresponding sub-shapes.
void asiUI_PartGraph::RenderTopology(const TopoDS_Shape&    shape,
                                     const TopAbs_ShapeEnum leafType,
                                     const bool             colorizeLocations)
{
  this->Render(shape,
               TopTools_IndexedMapOfShape(),
               Regime_Topology,
               leafType,
               colorizeLocations);
}

//-----------------------------------------------------------------------------

//! Renders face adjacency graph.
//! \param[in] shape         target shape.
//! \param[in] selectedFaces selected faces.
void asiUI_PartGraph::RenderAdjacency(const TopoDS_Shape&               shape,
                                      const TopTools_IndexedMapOfShape& selectedFaces)
{
  this->Render(shape, selectedFaces, Regime_AAG, TopAbs_SHAPE, false);
}

//-----------------------------------------------------------------------------

//! Renders face adjacency graph.
//! \param[in] aag           target AAG.
//! \param[in] selectedFaces selected faces.
void asiUI_PartGraph::RenderAdjacency(const Handle(asiAlgo_AAG)&        aag,
                                      const TopTools_IndexedMapOfShape& selectedFaces)
{
  // Populate graph data from topology graph
  vtkSmartPointer<vtkGraph>
    graph = this->convertToGraph(aag->GetMasterCAD(),
                                 aag,
                                 selectedFaces,
                                 Regime_AAG,
                                 TopAbs_SHAPE);

  // Render VTK graph
  this->Render(graph, aag->GetMasterCAD(), Regime_AAG, false);
}

//-----------------------------------------------------------------------------

//! Builds one or another graph (depending on the desired regime).
//! \param[in] shape         master model.
//! \param[in] aag           master AAG (optional).
//! \param[in] selectedFaces optional selected faces.
//! \param[in] regime        desired regime.
//! \param[in] leafType      leaf type for FULL regime.
//! \return graph instance.
vtkSmartPointer<vtkGraph>
  asiUI_PartGraph::convertToGraph(const TopoDS_Shape&               shape,
                                  const Handle(asiAlgo_AAG)&        aag,
                                  const TopTools_IndexedMapOfShape& selectedFaces,
                                  const Regime                      regime,
                                  const TopAbs_ShapeEnum            leafType)
{
  vtkSmartPointer<vtkGraph> result;
  //
  if ( regime == Regime_Topology )
  {
    // Create new topology graph or take the existing one from Naming
    /*if ( m_naming.IsNull() )*/
      m_topoGraph = new asiAlgo_TopoGraph(shape);
    //else
    //  m_topoGraph = m_naming->GetTopoGraph()->SubGraph(shape);

    // Convert
    vtkSmartPointer<vtkMutableDirectedGraph>
      directed = asiUI_TopoGraphAdaptor::Convert(m_topoGraph, m_naming, leafType);
    //
    result = directed;
  }
  else if ( regime == Regime_AAG )
  {
    m_aag = (aag.IsNull() ? new asiAlgo_AAG(shape) : aag);

    // Set selected faces
    m_aag->SetSelectedFaces(selectedFaces);

    // Convert
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
//! \param[in] subShapeId sub-shape ID.
//! \param[in] shapeType  sub-shape type.
//! \param[in] vid        graph vertex ID.
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
    mapOfEdges = m_aag.IsNull() ? m_topoGraph->GetMapOfEdges() : m_aag->RequestMapOfEdges();
  //
  const TopTools_IndexedMapOfShape&
    mapOfVertices = m_aag.IsNull() ? m_topoGraph->GetMapOfVertices() : m_aag->RequestMapOfVertices();
  //
  const TopTools_IndexedMapOfShape&
    mapOfSubShapes = m_aag.IsNull() ? m_topoGraph->GetMapOfSubShapes() : m_aag->RequestMapOfSubShapes();

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
