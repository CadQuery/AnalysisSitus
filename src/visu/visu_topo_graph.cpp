//-----------------------------------------------------------------------------
// Created on: 05 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_topo_graph.h>

// Visualization includes
#include <visu_topo_graph_item.h>
#include <visu_utils.h>

// Geometry includes
#include <geom_utils.h>

// Common includes
#include <common_facilities.h>

// OCCT includes
#include <SelectMgr_IndexedMapOfOwner.hxx>
#include <StdSelect_BRepOwner.hxx>
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

#define VTK_CREATE(Type, Name) \
  vtkSmartPointer<Type> Name = vtkSmartPointer<Type>::New()

//! Constructor.
visu_topo_graph::visu_topo_graph()
{}

//! Destructor.
visu_topo_graph::~visu_topo_graph()
{
  if ( !m_prevHighlighted.IsNull() && m_prevHighlighted->IsSelected() )
  {
    Handle(AIS_InteractiveContext) ctx = common_facilities::Instance()->ViewerDMU->GetContext();
    ctx->AddOrRemoveSelected(m_prevHighlighted, 1);
  }
}

//! Renders topology graph in the requested regime.
//! \param shape         [in] target shape.
//! \param selectedFaces [in] selected faces.
//! \param regime        [in] regime of interest.
//! \param leafType      [in] target leaf type for FULL regime.
void visu_topo_graph::Render(const TopoDS_Shape&         shape,
                             const TopTools_ListOfShape& selectedFaces,
                             const Regime                regime,
                             const TopAbs_ShapeEnum      leafType)
{
  // Populate graph data from topology graph
  vtkSmartPointer<vtkGraph> graph = this->convertToGraph(shape, selectedFaces, regime, leafType);

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
  vtkSmartPointer<visu_topo_graph_item> graphItem = vtkSmartPointer<visu_topo_graph_item>::New();
  graphItem->SetGraph( graphLayout->GetOutput() );

  connect( graphItem, SIGNAL( vertexPicked(const vtkIdType) ), this, SLOT( onVertexPicked(const vtkIdType) ) );

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
  renderer->SetGradientBackground(true);
  renderer->SetBackground(0.0, 0.0, 0.0);
  renderer->SetBackground(0.1, 0.1, 0.1);

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
  m_pWidget = new gui_vtk_window();
  m_pWidget->SetRenderWindow( renderWindow.GetPointer() );

  connect(m_pWidget, SIGNAL( windowClosed() ), this, SLOT( onViewerClosed() ) );

  /* ========================
   *  Add legend and summary
   * ======================== */

  // Legend
  m_textWidget = vtkSmartPointer<vtkTextWidget>::New();
  vtkTextRepresentation* textRep = vtkTextRepresentation::SafeDownCast( m_textWidget->GetRepresentation() );
  textRep->GetPositionCoordinate()->SetValue(0.2, 0.01);
  textRep->GetPosition2Coordinate()->SetValue(0.5, 0.06);
  m_textWidget->SelectableOff();
  //
  vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();
  textRep->SetTextActor(textActor);
  //
  m_textWidget->GetTextActor()->SetInput( regime == Regime_Full ? LEGEND_TITLE_ACCESSORY : LEGEND_TITLE_ADJACENCY );
  m_textWidget->SetInteractor( m_pWidget->GetInteractor() );
  m_textWidget->SetDefaultRenderer( renderer );
  m_textWidget->SetCurrentRenderer( renderer );

  // Shape summary
  m_summaryWidget = vtkSmartPointer<vtkTextWidget>::New();
  visu_utils::InitTextWidget(m_summaryWidget);
  //
  TCollection_AsciiString shapeInfo;
  geom_utils::ShapeSummary(shape, shapeInfo);
  m_summaryWidget->GetTextActor()->SetInput( shapeInfo.ToCString() );
  //
  m_summaryWidget->SetInteractor      ( m_pWidget->GetInteractor() );
  m_summaryWidget->SetDefaultRenderer ( renderer );
  m_summaryWidget->SetCurrentRenderer ( renderer );

  /* ===================
   *  Start interaction
   * =================== */

  renderWindow->SetLineSmoothing(true);
  renderWindow->SetWindowName( regime == Regime_Full ? "Accessory Graph" : "Adjacency Graph" );
  //
  graphItem->StartLayoutAnimation( m_pWidget->GetInteractor() );
  //
  m_pWidget->GetInteractor()->Initialize();
  m_pWidget->resize(400, 400);
  m_pWidget->show();

  // Set callback on rendering
  m_pWidget->GetRenderWindow()->AddObserver(vtkCommand::RenderEvent, this, &visu_topo_graph::RenderEventCallback);
}

//! Renders topology graph.
//! \param shape    [in] target shape.
//! \param leafType [in] target leaf type.
void visu_topo_graph::RenderFull(const TopoDS_Shape& shape, const TopAbs_ShapeEnum leafType)
{
  this->Render(shape, TopTools_ListOfShape(), Regime_Full, leafType);
}

//! Renders AA graph.
//! \param shape         [in] target shape.
//! \param selectedFaces [in] selected faces.
void visu_topo_graph::RenderAdjacency(const TopoDS_Shape&         shape,
                                      const TopTools_ListOfShape& selectedFaces)
{
  this->Render(shape, selectedFaces, Regime_AAG, TopAbs_SHAPE);
}

//! Builds one or another graph (depending on the desired regime).
//! \param shape         [in] master model.
//! \param selectedFaces [in] optional selected faces.
//! \param regime        [in] desired regime.
//! \param leafType      [in] leaf type for FULL regime.
//! \return graph instance.
vtkSmartPointer<vtkGraph>
  visu_topo_graph::convertToGraph(const TopoDS_Shape&         shape,
                                  const TopTools_ListOfShape& selectedFaces,
                                  const Regime                regime,
                                  const TopAbs_ShapeEnum      leafType)
{
  vtkSmartPointer<vtkGraph> result;
  //
  if ( regime == Regime_Full )
  {
    result = vtkSmartPointer<vtkMutableDirectedGraph>::New();
    vtkMutableDirectedGraph* directed_result = dynamic_cast<vtkMutableDirectedGraph*>( result.GetPointer() );

    // Array for groups
    vtkNew<vtkStringArray> groupArr;
    groupArr->SetNumberOfComponents(1);
    groupArr->SetName(ARRNAME_GROUP);

    // Array for vertex labels
    vtkNew<vtkStringArray> labelArr;
    labelArr->SetNumberOfComponents(1);
    labelArr->SetName(ARRNAME_LABELS);

    // Create VTK data set for graph data
    const vtkIdType root_vid = directed_result->AddVertex();
    TopTools_DataMapOfShapeInteger M;
    M.Bind(shape, root_vid);
    //
    labelArr->InsertNextValue( geom_utils::ShapeAddrWithPrefix(shape).c_str() );
    groupArr->InsertNextValue( shape.ShapeType() == TopAbs_COMPOUND ? ARRNAME_GROUP_COMPOUND : ARRNAME_GROUP_ORDINARY );
    //
    this->buildRecursively(shape, root_vid, leafType, directed_result, labelArr.GetPointer(), groupArr.GetPointer(), M);

    // Set arrays
    result->GetVertexData()->AddArray( labelArr.GetPointer() );
    result->GetVertexData()->AddArray( groupArr.GetPointer() );
  }
  else if ( regime == Regime_AAG )
  {
    m_aag = new geom_aag(shape, selectedFaces);
    vtkSmartPointer<vtkMutableUndirectedGraph> undirected_result = m_aag->ToVTK();
    result = undirected_result;
  }
  else
    Standard_ProgramError::Raise("Unexpected regime for graph visualization");

  return result;
}

//! Builds data structures for visualization recursively.
//! \param rootShape     [in]     root shape.
//! \param rootId        [in]     ID of the root vertex.
//! \param leafType      [in]     topological type for leafs.
//! \param pDS           [in/out] data structure being filled.
//! \param pLabelArr     [in/out] array for labels associated with vertices.
//! \param pGroupArr     [in/out] array for vertex groups.
//! \param shapeVertices [in/out] map of shapes against their registered graph vertices.
void visu_topo_graph::buildRecursively(const TopoDS_Shape&             rootShape,
                                       const vtkIdType                 rootId,
                                       const TopAbs_ShapeEnum          leafType,
                                       vtkMutableDirectedGraph*        pDS,
                                       vtkStringArray*                 pLabelArr,
                                       vtkStringArray*                 pGroupArr,
                                       TopTools_DataMapOfShapeInteger& shapeVertices)
{
  // Check if it is time to stop
  if ( rootShape.ShapeType() == leafType )
    return;

  // Iterate over the sub-shape
  for ( TopoDS_Iterator it(rootShape, 0, 0); it.More(); it.Next() )
  {
    const TopoDS_Shape& subShape = it.Value();

    vtkIdType childId;
    if ( shapeVertices.IsBound(subShape) )
    {
      childId = shapeVertices.Find(subShape);
    }
    else
    {
      childId = pDS->AddVertex();
      shapeVertices.Bind(subShape, childId);

      if ( pLabelArr )
        pLabelArr->InsertNextValue( geom_utils::ShapeAddrWithPrefix(subShape).c_str() );

      if ( pGroupArr )
        pGroupArr->InsertNextValue( subShape.ShapeType() == TopAbs_COMPOUND ? ARRNAME_GROUP_COMPOUND : ARRNAME_GROUP_ORDINARY );
    }
    //
    pDS->AddEdge(rootId, childId);
    //
    this->buildRecursively(subShape, childId, leafType, pDS, pLabelArr, pGroupArr, shapeVertices);
  }
}

//! Callback to adjust text widgets.
void visu_topo_graph::RenderEventCallback()
{
  if ( !m_textWidget->GetEnabled() )
    m_textWidget->On();

  if ( !m_summaryWidget->GetEnabled() )
    m_summaryWidget->On();
}

//! Reaction on closing the viewer.
void visu_topo_graph::onViewerClosed()
{
  delete m_pWidget;
  delete this;
}

//! Reaction on vertex picking.
//! \param vid [in] vertex ID.
void visu_topo_graph::onVertexPicked(const vtkIdType vid)
{
  if ( m_aag.IsNull() )
    return;

  if ( !common_facilities::Instance()->ViewerDMU )
    return;

  // Let's use the fact that vertex ID is equal to face ID minus 1.
  // The latter is by construction
  const int face_id = vid + 1;

  // Get face from graph vertex
  TopoDS_Face F = m_aag->GetFace(face_id);

  // Access presentation
  Handle(AIS_InteractiveContext) ctx = common_facilities::Instance()->ViewerDMU->GetContext();
  //
  Handle(SelectMgr_IndexedMapOfOwner) prsOwners;
  ctx->EntityOwners(prsOwners, common_facilities::Instance()->aisDMU);
  //
  if ( prsOwners.IsNull() || !prsOwners->Extent() )
  {
    std::cout << "Error: no entity owners" << std::endl;
    return;
  }
  else
    std::cout << "Got " << prsOwners->Extent() << " entity owner(s)" << std::endl;

  if ( !m_prevHighlighted.IsNull() && m_prevHighlighted->IsSelected() )
    ctx->AddOrRemoveSelected(m_prevHighlighted, 1);

  // Highlight
  for ( int i = 1; i <= prsOwners->Extent(); ++i )
  {
    const Handle(SelectMgr_EntityOwner)& owner = prsOwners->FindKey(i);
    //
    if ( owner->IsKind( STANDARD_TYPE(StdSelect_BRepOwner) ) )
    {
      Handle(StdSelect_BRepOwner) brepOwner = Handle(StdSelect_BRepOwner)::DownCast(owner);
      const TopoDS_Shape& ownedShape = brepOwner->Shape();

      // Try to find the hole shape among sensitivities
      if ( ownedShape.IsSame(F) )
      {
        if ( !owner->IsSelected() )
        {
          ctx->AddOrRemoveSelected(owner, 1);
          m_prevHighlighted = owner;
        }
        break;
      }
    }
  }
}
