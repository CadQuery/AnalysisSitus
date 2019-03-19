//-----------------------------------------------------------------------------
// Created on: 22 June 2017
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <ottoExe_Graph.h>

// ottoExe includes
#include <ottoExe_CommonFacilities.h>
#include <ottoExe_GraphItem.h>

// asiVisu includes
#include <asiVisu_Utils.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// Active Data includes
#include <ActData_DependencyGraph.h>

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
#include <vtkMutableDirectedGraph.h>
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

///
#include <vtkGraphLayoutView.h>
#include <vtkGraphToPolyData.h>
#include <vtkGlyphSource2D.h>
#include <vtkGlyph3D.h>
#include <vtkPolyDataMapper.h>

//-----------------------------------------------------------------------------

#define LEGEND_TITLE "Dependency graph"
#define COLOR_NORMAL 0.0, 1.0, 0.0
#define COLOR_HEAVY 1.0, 0.0, 0.0

//-----------------------------------------------------------------------------

//! Constructor.
//! \param[in] model Data Model instance.
ottoExe_Graph::ottoExe_Graph(const Handle(asiEngine_Model)& model)
: m_model      (model),
  m_textWidget (NULL)
{}

//-----------------------------------------------------------------------------

//! Destructor.
ottoExe_Graph::~ottoExe_Graph()
{}

//-----------------------------------------------------------------------------

//! Renders graph.
void ottoExe_Graph::Render()
{
  vtkSmartPointer<vtkGraph> graph = this->convertToGraph();

  /* ===================================
   *  Prepare structures for attributes
   * =================================== */

  //// Layout strategy
  //vtkNew<vtkSimple2DLayoutStrategy> simple2DStrategy;
  //simple2DStrategy->SetIterationsPerLayout(10);

  //// Layout
  //vtkSmartPointer<vtkGraphLayout> graphLayout = vtkSmartPointer<vtkGraphLayout>::New();
  //graphLayout->SetInputData(graph);
  //graphLayout->SetLayoutStrategy( simple2DStrategy.GetPointer() );
  //graphLayout->Update();

  //// Graph item
  //vtkSmartPointer<ottoExe_GraphItem>
  //  graphItem = vtkSmartPointer<ottoExe_GraphItem>::New();
  ////
  //graphItem->SetGraph( graphLayout->GetOutput() );

  //connect( graphItem, SIGNAL( vertexPicked(const vtkIdType) ),
  //         this,      SLOT( onVertexPicked(const vtkIdType) ) );

  //// Context transform
  //vtkSmartPointer<vtkContextTransform> trans = vtkSmartPointer<vtkContextTransform>::New();
  //trans->SetInteractive(true);
  //trans->AddItem(graphItem);

  //// Context actor
  //vtkSmartPointer<vtkContextActor> actor = vtkSmartPointer<vtkContextActor>::New();
  //actor->GetScene()->AddItem(trans);

  ///* ===============================================
  // *  Prepare and initialize interaction facilities
  // * =============================================== */

  //// Renderer
  //vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
  //renderer->SetGradientBackground(true);
  //renderer->SetBackground(0.0, 0.0, 0.0);
  //renderer->SetBackground(0.1, 0.1, 0.1);

  //// Render window
  //vtkNew<vtkRenderWindow> renderWindow;
  //renderWindow->AddRenderer(renderer);
  //renderer->AddActor(actor);

  //// Context interactor style
  //vtkNew<vtkContextInteractorStyle> interactorStyle;
  //interactorStyle->SetScene( actor->GetScene() );

  //// Interactor
  //vtkNew<QVTKInteractor> interactor;
  //interactor->SetInteractorStyle( interactorStyle.GetPointer() );
  //interactor->SetRenderWindow( renderWindow.GetPointer() );

  //// Create Qt window. NOTICE that interactor should be already given to
  //// the render window at this stage
  //m_pWidget = new asiUI_VtkWindow();
  //m_pWidget->SetRenderWindow( renderWindow.GetPointer() );

  //connect( m_pWidget, SIGNAL( windowClosed() ), this, SLOT( onViewerClosed() ) );

  /* ========================
   *  Add legend and summary
   * ======================== */

  //// Legend
  //m_textWidget = vtkTextWidget::New();
  //vtkTextRepresentation* textRep = vtkTextRepresentation::SafeDownCast( m_textWidget->GetRepresentation() );
  //textRep->GetPositionCoordinate()->SetValue(0.2, 0.01);
  //textRep->GetPosition2Coordinate()->SetValue(0.5, 0.06);
  //m_textWidget->SelectableOff();
  ////
  //vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();
  //textRep->SetTextActor(textActor);
  ////
  //m_textWidget->GetTextActor()->SetInput(LEGEND_TITLE);
  //m_textWidget->SetInteractor( m_pWidget->GetInteractor() );
  //m_textWidget->SetDefaultRenderer( renderer );
  //m_textWidget->SetCurrentRenderer( renderer );


  ///

  // Do layout manually before handing graph to the view.
  // This allows us to know the positions of edge arrows.
  vtkSmartPointer<vtkGraphLayoutView> graphLayoutView = 
    vtkSmartPointer<vtkGraphLayoutView>::New();

  vtkSmartPointer<vtkGraphLayout> layout = 
    vtkSmartPointer<vtkGraphLayout>::New();
  vtkSmartPointer<vtkSimple2DLayoutStrategy> strategy = 
    vtkSmartPointer<vtkSimple2DLayoutStrategy>::New();
  layout->SetInputData(graph);
  layout->SetLayoutStrategy(strategy);

  // Tell the view to use the vertex layout we provide
  graphLayoutView->SetLayoutStrategyToPassThrough();
  // The arrows will be positioned on a straight line between two
  // vertices so tell the view not to draw arcs for parallel edges
  graphLayoutView->SetEdgeLayoutStrategyToPassThrough();
 
  // Add the graph to the view. This will render vertices and edges,
  // but not edge arrows.
  graphLayoutView->AddRepresentationFromInputConnection(layout->GetOutputPort());

  // Manually create an actor containing the glyphed arrows.
  vtkSmartPointer<vtkGraphToPolyData> graphToPoly = 
    vtkSmartPointer<vtkGraphToPolyData>::New();
  graphToPoly->SetInputConnection(layout->GetOutputPort());
  graphToPoly->EdgeGlyphOutputOn();
 
  // Set the position (0: edge start, 1: edge end) where
  // the edge arrows should go.
  graphToPoly->SetEdgeGlyphPosition(0.98);
 
  // Make a simple edge arrow for glyphing.
  vtkSmartPointer<vtkGlyphSource2D> arrowSource = 
    vtkSmartPointer<vtkGlyphSource2D>::New();
  arrowSource->SetGlyphTypeToEdgeArrow();
  arrowSource->SetScale(0.05);
  arrowSource->Update();
 
  // Use Glyph3D to repeat the glyph on all edges.
  vtkSmartPointer<vtkGlyph3D> arrowGlyph = 
    vtkSmartPointer<vtkGlyph3D>::New();
  arrowGlyph->SetInputConnection(0, graphToPoly->GetOutputPort(1));
  arrowGlyph->SetInputConnection(1, arrowSource->GetOutputPort());
 
  // Add the edge arrow actor to the view.
  vtkSmartPointer<vtkPolyDataMapper> arrowMapper = 
    vtkSmartPointer<vtkPolyDataMapper>::New();
  arrowMapper->SetInputConnection(arrowGlyph->GetOutputPort());
  vtkSmartPointer<vtkActor> arrowActor = 
    vtkSmartPointer<vtkActor>::New();
  arrowActor->SetMapper(arrowMapper);
  graphLayoutView->GetRenderer()->AddActor(arrowActor);

  graphLayoutView->SetVertexLabelVisibility(true);
  graphLayoutView->SetVertexLabelArrayName(ARRNAME_LABELS);

  graphLayoutView->ResetCamera();
  graphLayoutView->Render();
  graphLayoutView->GetInteractor()->Start();


  //renderWindow->SetLineSmoothing(true);
  //renderWindow->SetWindowName("Dependency graph");
  ////
  //graphItem->StartLayoutAnimation( m_pWidget->GetInteractor() );
  ////
  //m_pWidget->GetInteractor()->Initialize();
  //m_pWidget->resize(400, 400);
  //m_pWidget->show();

  //// Set callback on rendering
  //m_pWidget->GetRenderWindow()->AddObserver(vtkCommand::RenderEvent, this, &ottoExe_Graph::RenderEventCallback);
}

//-----------------------------------------------------------------------------

//! Callback to adjust text widgets.
void ottoExe_Graph::RenderEventCallback()
{
  if ( !m_textWidget->GetEnabled() )
    m_textWidget->On();
}

//-----------------------------------------------------------------------------

vtkSmartPointer<vtkGraph> ottoExe_Graph::convertToGraph()
{
  vtkSmartPointer<vtkMutableDirectedGraph>
    result = vtkSmartPointer<vtkMutableDirectedGraph>::New();

  /* ===================================
   *  Prepare structures for attributes
   * =================================== */

  // Array for groups
  vtkSmartPointer<vtkStringArray> groupArr = vtkSmartPointer<vtkStringArray>::New();
  groupArr->SetNumberOfComponents(1);
  groupArr->SetName(ARRNAME_GROUP);

  // Array for vertex labels
  vtkSmartPointer<vtkStringArray> labelArr = vtkSmartPointer<vtkStringArray>::New();
  labelArr->SetNumberOfComponents(1);
  labelArr->SetName(ARRNAME_LABELS);

  /* ================================================
   *  Populate graph data from Model Execution Graph
   * ================================================ */

  Handle(ActData_DependencyGraph) depGraph = new ActData_DependencyGraph(m_model);

  // Get graph data
  const ActData_DependencyGraph::VertexDataMap& vertices = depGraph->Vertices();
  const ActData_DependencyGraph::EdgeMap&       arcs     = depGraph->Edges();

  // Populate VTK graph data with vertices
  NCollection_DataMap<int, vtkIdType> VTKVertMap;
  for ( ActData_DependencyGraph::VertexDataMap::Iterator vIt(vertices); vIt.More(); vIt.Next() )
  {
    // Prepare domain-VTK correspondence pair
    int       V    = vIt.Key();
    vtkIdType vtkV = result->AddVertex();
    //
    VTKVertMap.Bind(V, vtkV);

    // Generate label for vertex
    const ActData_DependencyGraph::VertexData& VData = vIt.Value();
    ActAPI_DataObjectId nodeId = VData.Parameter->GetNode()->GetId();
    TCollection_AsciiString
      name = TCollection_AsciiString( VData.TreeFunction->GetName() ).Cat(" [").Cat(nodeId).Cat("]");
    //
    labelArr->InsertNextValue( name.ToCString() );

    // Set color scalar for vertex
    if ( VData.TreeFunction->IsHeavy() )
      groupArr->InsertNextValue(ARRNAME_GROUP_TF_HEAVY);
    else
      groupArr->InsertNextValue(ARRNAME_GROUP_TF_LIGHT);
  }

  // Populate VTK graph data with edges
  for ( ActData_DependencyGraph::EdgeMap::Iterator arcIt(arcs); arcIt.More(); arcIt.Next() )
  {
    const ActData_DependencyGraph::OriEdge& arc = arcIt.Value();
    result->AddEdge( VTKVertMap.Find(arc.V1), VTKVertMap(arc.V2) );
  }

  result->GetVertexData()->AddArray(labelArr);
  result->GetVertexData()->AddArray(groupArr);

  return result;
}

//-----------------------------------------------------------------------------

//! Reaction on closing the viewer.
void ottoExe_Graph::onViewerClosed()
{
  // NOTE: the important point is to remove widget after all items which
  //       may listen to it
  m_textWidget->Delete();

  delete m_pWidget;
  delete this;
}

//-----------------------------------------------------------------------------

void ottoExe_Graph::onVertexPicked(const vtkIdType vid)
{
  // NYI
}
