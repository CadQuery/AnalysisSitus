//-----------------------------------------------------------------------------
// Created on: 31 March 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_ControlsFeature.h>

// exeFeatures includes
#include <asiUI_AAGAdaptor.h>
#include <asiUI_Common.h>
#include <asiUI_DialogSmoothAngleTol.h>
#include <asiUI_TopoGraph.h>

// asiAlgo includes
#include <asiAlgo_AAGIterator.h>
#include <asiAlgo_DetachFaces.h>
#include <asiAlgo_DihedralAngle.h>
#include <asiAlgo_FindNonmanifold.h>
#include <asiAlgo_FindSameHosts.h>
#include <asiAlgo_MeshConvert.h>
#include <asiAlgo_PLY.h>
#include <asiAlgo_RehostFaces.h>
#include <asiAlgo_STEP.h>
#include <asiAlgo_SuppressFaces.h>
#include <asiAlgo_Timer.h>
#include <asiAlgo_Utils.h>

// asiVisu includes
#include <asiVisu_GeomPrs.h>

// asiEngine includes
#include <asiEngine_Part.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <Precision.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

// Qt include
#include <QGroupBox>

// VTK includes
#include <vtkActor.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

#define BTN_MIN_WIDTH 120

//-----------------------------------------------------------------------------

//! Constructor.
//! \param model       [in] Data Model instance.
//! \param pPartViewer [in] Part viewer.
//! \param notifier    [in] progress notifier.
//! \param plotter     [in] imperative plotter.
//! \param parent      [in] parent widget.
asiUI_ControlsFeature::asiUI_ControlsFeature(const Handle(asiEngine_Model)& model,
                                             asiUI_ViewerPart*              pPartViewer,
                                             ActAPI_ProgressEntry           notifier,
                                             ActAPI_PlotterEntry            plotter,
                                             QWidget*                       parent)
: QWidget      (parent),
  m_model      (model),
  m_partViewer (pPartViewer),
  m_notifier   (notifier),
  m_plotter    (plotter)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Buttons
  m_widgets.pShowTOPOGraph       = new QPushButton("Show TOPO graph");
  m_widgets.pShowAAG             = new QPushButton("Show AA graph");
  m_widgets.pElimSelected        = new QPushButton("Show AA graph w/o selected");
  //
  m_widgets.pNonManifoldEdges    = new QPushButton("Non-manifold edges");
  m_widgets.pCheckDihAngles      = new QPushButton("Classify dihedral angles");
  m_widgets.pFindSmoothEdges     = new QPushButton("Find smooth edges");
  m_widgets.pFindConvexOnly      = new QPushButton("Find convex-only");
  //
  m_widgets.pDetachSelected      = new QPushButton("Detach selected faces");
  m_widgets.pDeleteSelected      = new QPushButton("Delete selected faces");
  m_widgets.pDeleteSelectedFull  = new QPushButton("Delete selected faces (FULL)");
  //
  m_widgets.pShowTOPOGraph       -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pShowAAG             -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pElimSelected        -> setMinimumWidth(BTN_MIN_WIDTH);
  //
  m_widgets.pNonManifoldEdges    -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pCheckDihAngles      -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pFindSmoothEdges     -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pFindConvexOnly      -> setMinimumWidth(BTN_MIN_WIDTH);
  //
  m_widgets.pDetachSelected      -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pDeleteSelected      -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pDeleteSelectedFull  -> setMinimumWidth(BTN_MIN_WIDTH);

  // Group for analysis
  QGroupBox*   pAnalysisGroup = new QGroupBox("Analysis");
  QVBoxLayout* pAnalysisLay   = new QVBoxLayout(pAnalysisGroup);
  //
  pAnalysisLay->addWidget(m_widgets.pShowTOPOGraph);
  pAnalysisLay->addWidget(m_widgets.pShowAAG);
  pAnalysisLay->addWidget(m_widgets.pElimSelected);

  // Group for features
  QGroupBox*   pFeaturesGroup = new QGroupBox("Features");
  QVBoxLayout* pFeaturesLay   = new QVBoxLayout(pFeaturesGroup);
  //
  pFeaturesLay->addWidget(m_widgets.pNonManifoldEdges);
  pFeaturesLay->addWidget(m_widgets.pCheckDihAngles);
  pFeaturesLay->addWidget(m_widgets.pFindSmoothEdges);
  pFeaturesLay->addWidget(m_widgets.pFindConvexOnly);

  // Group for features
  QGroupBox*   pDyModelingGroup = new QGroupBox("Dynamic Modeling");
  QVBoxLayout* pDyModelingLay   = new QVBoxLayout(pDyModelingGroup);
  //
  pDyModelingLay->addWidget(m_widgets.pDeleteSelected);
  pDyModelingLay->addWidget(m_widgets.pDetachSelected);
  pDyModelingLay->addWidget(m_widgets.pDeleteSelectedFull);

  // Set layout
  m_pMainLayout->addWidget(pAnalysisGroup);
  m_pMainLayout->addWidget(pFeaturesGroup);
  m_pMainLayout->addWidget(pDyModelingGroup);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  //
  this->setLayout(m_pMainLayout);

  // Connect signals to slots
  connect( m_widgets.pShowTOPOGraph,      SIGNAL( clicked() ), SLOT( onShowTOPOGraph       () ) );
  connect( m_widgets.pShowAAG,            SIGNAL( clicked() ), SLOT( onShowAAG             () ) );
  connect( m_widgets.pElimSelected,       SIGNAL( clicked() ), SLOT( onElimSelected        () ) );
  //
  connect( m_widgets.pNonManifoldEdges,   SIGNAL( clicked() ), SLOT( onNonManifoldEdges    () ) );
  connect( m_widgets.pCheckDihAngles,     SIGNAL( clicked() ), SLOT( onCheckDihedralAngles () ) );
  connect( m_widgets.pFindSmoothEdges,    SIGNAL( clicked() ), SLOT( onFindSmoothEdges     () ) );
  connect( m_widgets.pFindConvexOnly,     SIGNAL( clicked() ), SLOT( onFindConvexOnly      () ) );
  //
  connect( m_widgets.pDetachSelected,     SIGNAL( clicked() ), SLOT( onDetachSelected      () ) );
  connect( m_widgets.pDeleteSelected,     SIGNAL( clicked() ), SLOT( onDeleteSelected      () ) );
  connect( m_widgets.pDeleteSelectedFull, SIGNAL( clicked() ), SLOT( onDeleteSelectedFull  () ) );
}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_ControlsFeature::~asiUI_ControlsFeature()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//-----------------------------------------------------------------------------

//! Shows topology graph.
void asiUI_ControlsFeature::onShowTOPOGraph()
{
  // Access Geometry Node
  Handle(asiData_PartNode) N = m_model->GetPartNode();
  if ( N.IsNull() || !N->IsWellFormed() )
    return;

  // Shape to visualize a graph for
  TopoDS_Shape targetShape;

  // Access active face
  Handle(asiData_FaceNode) FN = N->GetFaceRepresentation();
  if ( FN.IsNull() || !FN->IsWellFormed() || FN->GetSelectedFace() <= 0 )
    targetShape = N->GetShape();
  else
  {
    const int f_idx = FN->GetSelectedFace();

    // Get all sub-shapes
    const TopTools_IndexedMapOfShape&
      M = m_model->GetPartNode()->GetAAG()->GetMapOfSubShapes();

    const TopoDS_Shape& shape = M.FindKey(f_idx);
    //
    if ( shape.ShapeType() == TopAbs_FACE )
    {
      const TopoDS_Face& F = TopoDS::Face(shape);
      //
      targetShape = F;
    }
  }

  // No shape, no graph
  if ( targetShape.IsNull() )
  {
    std::cout << "Error: target shape is null" << std::endl;
    return;
  }

  // Show graph
  asiUI_TopoGraph* pGraphView = new asiUI_TopoGraph(m_model, m_partViewer);
  pGraphView->RenderFull(targetShape, TopAbs_VERTEX);
}

//-----------------------------------------------------------------------------

//! Shows AA graph.
void asiUI_ControlsFeature::onShowAAG()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Access selected faces (if any)
  TopTools_IndexedMapOfShape selected;
  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedSubShapes(selected);

  // Show graph
  asiUI_TopoGraph* pGraphView = new asiUI_TopoGraph(m_model, m_partViewer);
  pGraphView->RenderAdjacency(part, selected);
}

//-----------------------------------------------------------------------------

//! Eliminates selected faces from AAG.
void asiUI_ControlsFeature::onElimSelected()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Get highlighted sub-shapes
  TopTools_IndexedMapOfShape selected;
  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedSubShapes(selected);

  // Build AAG
  Handle(asiAlgo_AAG) aag = new asiAlgo_AAG(part);

  // Remove highlighted sub-shapes
  aag->Remove(selected);

  // Show graph
  asiUI_TopoGraph* pGraphView = new asiUI_TopoGraph(m_model, m_partViewer);
  pGraphView->SetAAG(aag);
  //
  vtkSmartPointer<vtkMutableUndirectedGraph>
    undirected = asiUI_AAGAdaptor::Convert(aag);
  //
  pGraphView->Render(undirected, part, asiUI_TopoGraph::Regime_AAG);
}

//-----------------------------------------------------------------------------

//! Finds non-manifold edges.
void asiUI_ControlsFeature::onNonManifoldEdges()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Find non-manifold edges
  TopTools_IndexedMapOfShape nmEdges;
  asiAlgo_FindNonmanifold::FindEdges(part, nmEdges, m_notifier);
  //
  std::cout << "Number of non-manifold edges: " << nmEdges.Extent() << std::endl;

  // Prepare compound of edges
  TopoDS_Compound nmEdgesComp;
  BRep_Builder BB;
  BB.MakeCompound(nmEdgesComp);
  //
  for ( int e = 1; e <= nmEdges.Extent(); ++e )
  {
    BB.Add( nmEdgesComp, nmEdges(e) );
  }

  // Save to model
  Handle(asiData_BoundaryEdgesNode)
    BN = part_n->GetBoundaryEdgesRepresentation();
  //
  m_model->OpenCommand();
  {
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Red) )->SetShape(nmEdgesComp);
  }
  m_model->CommitCommand();

  // Update viewer
  Handle(asiVisu_GeomPrs)
    NPrs = Handle(asiVisu_GeomPrs)::DownCast( m_partViewer->PrsMgr()->GetPresentation(part_n) );
  //
  if ( NPrs.IsNull() )
  {
    std::cout << "Error: there is no available presentation for part" << std::endl;
    return;
  }
  //
  NPrs->MainActor()->GetProperty()->SetOpacity(0.5);
  //
  NPrs->GetPipeline(asiVisu_GeomPrs::Pipeline_Contour)->Actor()->SetVisibility(0);
  //
 m_partViewer->PrsMgr()->Actualize( BN.get() );
}

//-----------------------------------------------------------------------------

//! Classifies solid angles as concave / convex.
void asiUI_ControlsFeature::onCheckDihedralAngles()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Get highlighted sub-shapes
  TopTools_IndexedMapOfShape selected;
  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedSubShapes(selected);

  // Holders for geometries
  TopTools_IndexedMapOfShape convexEdges,     concaveEdges,     undefinedEdges,     smoothEdges;
  TopoDS_Compound            convexEdgesComp, concaveEdgesComp, undefinedEdgesComp, smoothEdgesComp;
  //
  BRep_Builder BB;
  BB.MakeCompound(convexEdgesComp);
  BB.MakeCompound(concaveEdgesComp);
  BB.MakeCompound(undefinedEdgesComp);
  BB.MakeCompound(smoothEdgesComp);

  // Perform either global or local analysis
  if ( selected.IsEmpty() )
  {
    // Build AAG
    Handle(asiAlgo_AAG) aag = new asiAlgo_AAG(part);

    // Loop over the faces
    //
    Handle(asiAlgo_AAGRandomIterator) it = new asiAlgo_AAGRandomIterator(aag);
    for ( ; it->More(); it->Next() )
    {
      const int current_face_idx = it->GetFaceId();
      TColStd_PackedMapOfInteger neighbor_ids;
      it->GetNeighbors(neighbor_ids);
      const TopoDS_Face& current_face = aag->GetFace(current_face_idx);

      // Loop over the neighbors
      for ( TColStd_MapIteratorOfPackedMapOfInteger nit(neighbor_ids); nit.More(); nit.Next() )
      {
        const int          neighbor_face_idx = nit.Key();
        const TopoDS_Face& neighbor_face     = aag->GetFace(neighbor_face_idx);

        // Check angle between the two faces
        this->classifyDihAngle(current_face,
                               neighbor_face,
                               convexEdges,
                               concaveEdges,
                               undefinedEdges,
                               smoothEdges,
                               convexEdgesComp,
                               concaveEdgesComp,
                               undefinedEdgesComp,
                               smoothEdgesComp,
                               false,
                               false,
                               0.0);
      }
    }
  }
  else
  {
    std::cout << "Info: local analysis of edges" << std::endl;

    TopoDS_Face twin;
    if ( selected.Extent() < 2 )
    {
      std::cout << "Warning: at least 2 faces are expected" << std::endl;

      // Analyze the same face twice
      twin = TopoDS::Face( selected.FindKey(1) );
    }

    for ( int f = 1; f <= selected.Extent(); ++f )
    {
      const TopoDS_Face& F = TopoDS::Face( selected(f) );
      //
      if ( twin.IsNull() )
      {
        for ( int g = f + 1; g <= selected.Extent(); ++g )
        {
          const TopoDS_Face& G = TopoDS::Face( selected(g) );

          // Check angle between the two faces
          this->classifyDihAngle(F,
                                 G,
                                 convexEdges,
                                 concaveEdges,
                                 undefinedEdges,
                                 smoothEdges,
                                 convexEdgesComp,
                                 concaveEdgesComp,
                                 undefinedEdgesComp,
                                 smoothEdgesComp,
                                 true,
                                 false,
                                 0.0);
        }
      }
      else
      {
        // Check angle between the two faces
        this->classifyDihAngle(F,
                               twin,
                               convexEdges,
                               concaveEdges,
                               undefinedEdges,
                               smoothEdges,
                               convexEdgesComp,
                               concaveEdgesComp,
                               undefinedEdgesComp,
                               smoothEdgesComp,
                               true,
                               false,
                               0.0);
      }
    }
  }

  // Dump
  std::cout << "\tNum. convex  edges: "   << convexEdges.Extent()    << std::endl;
  std::cout << "\tNum. concave edges: "   << concaveEdges.Extent()   << std::endl;
  std::cout << "\tNum. undefined edges: " << undefinedEdges.Extent() << std::endl;
  std::cout << "\tNum. smooth edges: "    << smoothEdges.Extent()    << std::endl;

  // Save to model
  Handle(asiData_BoundaryEdgesNode)
    BN = part_n->GetBoundaryEdgesRepresentation();
  //
  m_model->OpenCommand();
  {
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Green)    ) ->SetShape(convexEdgesComp);
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Red)   )    ->SetShape(concaveEdgesComp);
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Ordinary) ) ->SetShape(undefinedEdgesComp);
  }
  m_model->CommitCommand();

  // Update viewer
  Handle(asiVisu_GeomPrs)
    NPrs = Handle(asiVisu_GeomPrs)::DownCast( m_partViewer->PrsMgr()->GetPresentation(part_n) );
  //
  if ( NPrs.IsNull() )
  {
    std::cout << "Error: there is no available presentation for part" << std::endl;
    return;
  }
  //
  NPrs->MainActor()->GetProperty()->SetOpacity(0.5);
  //
  if ( !NPrs->GetPipeline(asiVisu_GeomPrs::Pipeline_Contour).IsNull() )
    NPrs->GetPipeline(asiVisu_GeomPrs::Pipeline_Contour)->Actor()->SetVisibility(0);
  //
  m_partViewer->PrsMgr()->Actualize( BN.get() );
}

//-----------------------------------------------------------------------------

//! Finds smooth edges.
void asiUI_ControlsFeature::onFindSmoothEdges()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Ask user to provide a tolerance for detection of smooth angles
  asiUI_DialogSmoothAngleTol* wDlg = new asiUI_DialogSmoothAngleTol(this);
  //
  wDlg->exec();
  //
  const double angTol = wDlg->Tolerance;

  // Get highlighted sub-shapes
  TopTools_IndexedMapOfShape selected;
  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedSubShapes(selected);

  // Holders for geometries
  TopTools_IndexedMapOfShape convexEdges,     concaveEdges,     undefinedEdges,     smoothEdges;
  TopoDS_Compound            convexEdgesComp, concaveEdgesComp, undefinedEdgesComp, smoothEdgesComp;
  //
  BRep_Builder BB;
  BB.MakeCompound(convexEdgesComp);
  BB.MakeCompound(concaveEdgesComp);
  BB.MakeCompound(undefinedEdgesComp);
  BB.MakeCompound(smoothEdgesComp);

  // Perform either global or local analysis
  if ( selected.IsEmpty() )
  {
    // Build AAG
    Handle(asiAlgo_AAG) aag = new asiAlgo_AAG(part);

    // Loop over the faces
    Handle(asiAlgo_AAGRandomIterator) it = new asiAlgo_AAGRandomIterator(aag);
    //
    for ( ; it->More(); it->Next() )
    {
      const int current_face_idx = it->GetFaceId();
      TColStd_PackedMapOfInteger neighbor_ids;
      it->GetNeighbors(neighbor_ids);
      const TopoDS_Face& current_face = aag->GetFace(current_face_idx);

      // Loop over the neighbors
      for ( TColStd_MapIteratorOfPackedMapOfInteger nit(neighbor_ids); nit.More(); nit.Next() )
      {
        const int          neighbor_face_idx = nit.Key();
        const TopoDS_Face& neighbor_face     = aag->GetFace(neighbor_face_idx);

        // Check angle between the two faces
        this->classifyDihAngle(current_face,
                               neighbor_face,
                               convexEdges,
                               concaveEdges,
                               undefinedEdges,
                               smoothEdges,
                               convexEdgesComp,
                               concaveEdgesComp,
                               undefinedEdgesComp,
                               smoothEdgesComp,
                               false,
                               true,
                               angTol);
      }
    }
  }
  else
  {
    std::cout << "Info: local analysis of edges" << std::endl;
    if ( selected.Extent() < 2 )
    {
      std::cout << "Error: at least 2 faces are expected" << std::endl;
      return;
    }

    for ( int f = 1; f <= selected.Extent(); ++f )
    {
      const TopoDS_Face& F = TopoDS::Face( selected(f) );
      //
      for ( int g = f + 1; g <= selected.Extent(); ++g )
      {
        const TopoDS_Face& G = TopoDS::Face( selected(g) );

        // Check angle between the two faces
        this->classifyDihAngle(F,
                               G,
                               convexEdges,
                               concaveEdges,
                               undefinedEdges,
                               smoothEdges,
                               convexEdgesComp,
                               concaveEdgesComp,
                               undefinedEdgesComp,
                               smoothEdgesComp,
                               true,
                               true,
                               angTol);
      }
    }
  }

  // Dump
  std::cout << "\tNum. convex  edges: "   << convexEdges.Extent()    << std::endl;
  std::cout << "\tNum. concave edges: "   << concaveEdges.Extent()   << std::endl;
  std::cout << "\tNum. undefined edges: " << undefinedEdges.Extent() << std::endl;
  std::cout << "\tNum. smooth edges: "    << smoothEdges.Extent()    << std::endl;

  // Save to model
  Handle(asiData_BoundaryEdgesNode)
    BN = part_n->GetBoundaryEdgesRepresentation();
  //
  m_model->OpenCommand();
  {
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Ordinary) ) ->SetShape( TopoDS_Shape() );
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Red) )      ->SetShape( TopoDS_Shape() );
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Green) )    ->SetShape(smoothEdgesComp);
  }
  m_model->CommitCommand();

  // Update viewer
  Handle(asiVisu_GeomPrs)
    NPrs = Handle(asiVisu_GeomPrs)::DownCast( m_partViewer->PrsMgr()->GetPresentation(part_n) );
  //
  if ( NPrs.IsNull() )
  {
    std::cout << "Error: there is no available presentation for part" << std::endl;
    return;
  }
  //
  NPrs->MainActor()->GetProperty()->SetOpacity(0.5);
  //
  NPrs->GetPipeline(asiVisu_GeomPrs::Pipeline_Contour)->Actor()->SetVisibility(0);
  //
  m_partViewer->PrsMgr()->Actualize( BN.get() );
}

//-----------------------------------------------------------------------------

//! Identifies convex-only faces.
void asiUI_ControlsFeature::onFindConvexOnly()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Build AAG
  Handle(asiAlgo_AAG) aag = new asiAlgo_AAG(part);

  // Find convex-only faces
  TopTools_IndexedMapOfShape convex;
  if ( !aag->FindConvexOnly(convex) )
  {
    std::cout << "There are no convex-only faces in AAG" << std::endl;
    return;
  }

  // Highlight sub-shape
  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).HighlightSubShapes(convex);
}

//-----------------------------------------------------------------------------

//! Detaches selected faces.
void asiUI_ControlsFeature::onDetachSelected()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Get highlighted faces
  TopTools_IndexedMapOfShape selected;
  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedSubShapes(selected);

  TIMER_NEW
  TIMER_GO

  // Delete selected faces
  asiAlgo_DetachFaces detacher(part);
  if ( !detacher.Perform(selected) )
  {
    std::cout << "Error: cannot detach faces" << std::endl;
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Detach faces")

  const TopoDS_Shape& result = detacher.Result();

  // Save to model
  m_model->Clear();
  //
  // Update part
  m_model->OpenCommand(); // tx start
  {
    asiEngine_Part( m_model, m_partViewer->PrsMgr() ).Update(result);
  }
  m_model->CommitCommand(); // tx commit

  // Update viewer
  m_partViewer->PrsMgr()->DeleteAllPresentations();
  m_partViewer->PrsMgr()->InitializePickers();
  m_partViewer->PrsMgr()->Actualize( part_n.get() );
}

//-----------------------------------------------------------------------------

//! Deletes selected faces.
void asiUI_ControlsFeature::onDeleteSelected()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Get highlighted faces
  TColStd_PackedMapOfInteger selected;
  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedFaces(selected);

  TIMER_NEW
  TIMER_GO

  // Delete selected faces
  asiAlgo_SuppressFaces eraser(part);
  if ( !eraser.Perform(selected, true) )
  {
    std::cout << "Error: cannot delete faces" << std::endl;
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Faces ONLY deletion")

  const TopoDS_Shape& result = eraser.GetResult();

  // Save to model
  m_model->Clear();
  //
  // Update part
  m_model->OpenCommand(); // tx start
  {
    asiEngine_Part( m_model, m_partViewer->PrsMgr() ).Update(result);
  }
  m_model->CommitCommand(); // tx commit

  // Update viewer
  m_partViewer->PrsMgr()->DeleteAllPresentations();
  m_partViewer->PrsMgr()->InitializePickers();
  m_partViewer->PrsMgr()->Actualize( part_n.get() );
}

//-----------------------------------------------------------------------------

//! Deletes selected faces with all underlying topology.
void asiUI_ControlsFeature::onDeleteSelectedFull()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Get highlighted faces
  TColStd_PackedMapOfInteger selected;
  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedFaces(selected);

  TIMER_NEW
  TIMER_GO

  // Delete selected faces
  asiAlgo_SuppressFaces eraser(part);
  if ( !eraser.Perform(selected, false) )
  {
    std::cout << "Error: cannot delete faces" << std::endl;
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("FULL faces deletion")

  const TopoDS_Shape& result = eraser.GetResult();

  // Save to model
  m_model->Clear();
  //
  // Update part
  m_model->OpenCommand(); // tx start
  {
    asiEngine_Part( m_model, m_partViewer->PrsMgr() ).Update(result);
  }
  m_model->CommitCommand(); // tx commit

  // Update viewer
  m_partViewer->PrsMgr()->DeleteAllPresentations();
  m_partViewer->PrsMgr()->InitializePickers();
  m_partViewer->PrsMgr()->Actualize( part_n.get() );
}

//-----------------------------------------------------------------------------

void asiUI_ControlsFeature::classifyDihAngle(const TopoDS_Face&          F,
                                             const TopoDS_Face&          G,
                                             TopTools_IndexedMapOfShape& convexEdges,
                                             TopTools_IndexedMapOfShape& concaveEdges,
                                             TopTools_IndexedMapOfShape& undefinedEdges,
                                             TopTools_IndexedMapOfShape& smoothEdges,
                                             TopoDS_Compound&            convexEdgesComp,
                                             TopoDS_Compound&            concaveEdgesComp,
                                             TopoDS_Compound&            undefinedEdgesComp,
                                             TopoDS_Compound&            smoothEdgesComp,
                                             const bool                  usePlotter,
                                             const bool                  allowSmooth,
                                             const double                smoothAngularTol) const
{
  // Check angle between the two faces
  TopTools_IndexedMapOfShape commonEdges;
  asiAlgo_DihedralAngle dihAngle(m_notifier,
                                 usePlotter ? m_plotter : NULL);
  //
  asiAlgo_FeatureAngle angle = dihAngle.AngleBetweenFaces(F,
                                                          G,
                                                          allowSmooth,
                                                          smoothAngularTol,
                                                          commonEdges);
  //
  TopTools_IndexedMapOfShape* pTargetMap;
  TopoDS_Compound*            pTargetComp;
  //
  if ( angle == Angle_Convex )
  {
    pTargetMap  = &convexEdges;
    pTargetComp = &convexEdgesComp;
  }
  else if ( angle == Angle_Concave )
  {
    pTargetMap  = &concaveEdges;
    pTargetComp = &concaveEdgesComp;
  }
  else if ( angle == Angle_Smooth )
  {
    pTargetMap  = &smoothEdges;
    pTargetComp = &smoothEdgesComp;
  }
  else
  {
    pTargetMap  = &undefinedEdges;
    pTargetComp = &undefinedEdgesComp;
  }
  //
  for ( int i = 1; i <= commonEdges.Extent(); ++i )
  {
    pTargetMap->Add( commonEdges(i) );
    BRep_Builder().Add( *pTargetComp, commonEdges(i) );
  }
}
