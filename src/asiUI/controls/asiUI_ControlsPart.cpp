//-----------------------------------------------------------------------------
// Created on: 25 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_ControlsPart.h>

// Algo includes
#include <asiAlgo_Timer.h>

// GUI includes
#include <asiUI_Common.h>
#include <asiUI_DialogCloudify.h>
#include <asiUI_DialogSewing.h>
#include <asiUI_DialogSTEP.h>
#include <asiUI_TopoGraph.h>

// Engine includes
#include <asiEngine_Part.h>

// Geometry includes
#include <asiAlgo_DetachFaces.h>
#include <asiAlgo_FindNonmanifold.h>
#include <asiAlgo_STEP.h>
#include <asiAlgo_Utils.h>

// Mesh includes
#include <asiAlgo_MeshConvert.h>
#include <asiAlgo_PLY.h>

// Visualization includes
#include <asiVisu_GeomPrs.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <AppDef_Array1OfMultiPointConstraint.hxx>
#include <AppDef_BSplineCompute.hxx>
#include <AppDef_MultiLine.hxx>
#include <AppDef_Variational.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepTools.hxx>
#include <GCPnts_QuasiUniformAbscissa.hxx>
#include <GCPnts_UniformAbscissa.hxx>
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
//! \param parent [in] parent widget.
asiUI_ControlsPart::asiUI_ControlsPart(QWidget* parent) : QWidget(parent)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Buttons
  m_widgets.pLoadBRep           = new QPushButton("Load b-rep");
  m_widgets.pLoadSTEP           = new QPushButton("Load STEP");
  m_widgets.pLoadSTL            = new QPushButton("Load STL");
  m_widgets.pSaveSTEP           = new QPushButton("Save STEP");
  m_widgets.pSavePly            = new QPushButton("Save mesh (ply)");
  m_widgets.pSaveBRep           = new QPushButton("Save b-rep");
  //
  m_widgets.pShowTOPOGraph      = new QPushButton("Show TOPO graph");
  m_widgets.pShowAAG            = new QPushButton("Show AA graph");
  m_widgets.pElimSelected       = new QPushButton("Show AA graph w/o selected");
  m_widgets.pCheckShape         = new QPushButton("Check shape");
  m_widgets.pTolerance          = new QPushButton("Tolerance");
  m_widgets.pNonManifoldEdges   = new QPushButton("Non-manifold edges");
  //
  m_widgets.pSewing             = new QPushButton("Sewing");
  m_widgets.pMaximizeFaces      = new QPushButton("Maximize faces");
  m_widgets.pOBB                = new QPushButton("OBB");
  m_widgets.pCR                 = new QPushButton("Canonical recognition");
  m_widgets.pCloudify           = new QPushButton("Cloudify");
  m_widgets.pMultiLine          = new QPushButton("Multi-line re-approx");
  //
  m_widgets.pShowVertices       = new QPushButton("Show vertices");
  m_widgets.pShowNormals        = new QPushButton("Show normals");
  m_widgets.pSelectFaces        = new QPushButton("Select faces");
  m_widgets.pSelectEdges        = new QPushButton("Select edges");
  //
  m_widgets.pLoadBRep           -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pLoadSTEP           -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pLoadSTL            -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pSaveSTEP           -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pSavePly            -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pSaveBRep           -> setMinimumWidth(BTN_MIN_WIDTH);
  //
  m_widgets.pShowTOPOGraph      -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pShowAAG            -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pElimSelected       -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pCheckShape         -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pTolerance          -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pNonManifoldEdges   -> setMinimumWidth(BTN_MIN_WIDTH);
  //
  m_widgets.pSewing             -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pMaximizeFaces      -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pOBB                -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pCR                 -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pCloudify           -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pMultiLine          -> setMinimumWidth(BTN_MIN_WIDTH);
  //
  m_widgets.pShowVertices       -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pShowNormals        -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pSelectFaces        -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pSelectEdges        -> setMinimumWidth(BTN_MIN_WIDTH);

  // Other configurations
  m_widgets.pShowVertices->setCheckable(true);

  // Group box for data interoperability
  QGroupBox*   pExchangeGroup = new QGroupBox("Data Exchange");
  QVBoxLayout* pExchangeLay   = new QVBoxLayout(pExchangeGroup);
  //
  pExchangeLay->addWidget(m_widgets.pLoadBRep);
  pExchangeLay->addWidget(m_widgets.pLoadSTEP);
  pExchangeLay->addWidget(m_widgets.pLoadSTL);
  pExchangeLay->addWidget(m_widgets.pSaveSTEP);
  pExchangeLay->addWidget(m_widgets.pSavePly);
  pExchangeLay->addWidget(m_widgets.pSaveBRep);

  // Group for analysis
  QGroupBox*   pAnalysisGroup = new QGroupBox("Analysis");
  QVBoxLayout* pAnalysisLay   = new QVBoxLayout(pAnalysisGroup);
  //
  pAnalysisLay->addWidget(m_widgets.pShowTOPOGraph);
  pAnalysisLay->addWidget(m_widgets.pShowAAG);
  pAnalysisLay->addWidget(m_widgets.pElimSelected);
  pAnalysisLay->addWidget(m_widgets.pCheckShape);
  pAnalysisLay->addWidget(m_widgets.pTolerance);
  pAnalysisLay->addWidget(m_widgets.pNonManifoldEdges);

  // Group for processing
  QGroupBox*   pProcessingGroup = new QGroupBox("Processing");
  QVBoxLayout* pProcessingLay   = new QVBoxLayout(pProcessingGroup);
  //
  pProcessingLay->addWidget(m_widgets.pSewing);
  pProcessingLay->addWidget(m_widgets.pMaximizeFaces);
  pProcessingLay->addWidget(m_widgets.pOBB);
  pProcessingLay->addWidget(m_widgets.pCR);
  pProcessingLay->addWidget(m_widgets.pCloudify);
  pProcessingLay->addWidget(m_widgets.pMultiLine);

  // Group for visualization
  QGroupBox*   pVisuGroup = new QGroupBox("Visualization");
  QVBoxLayout* pVisuLay   = new QVBoxLayout(pVisuGroup);
  //
  pVisuLay->addWidget(m_widgets.pShowVertices);
  pVisuLay->addWidget(m_widgets.pShowNormals);
  pVisuLay->addWidget(m_widgets.pSelectFaces);
  pVisuLay->addWidget(m_widgets.pSelectEdges);

  // Set layout
  m_pMainLayout->addWidget(pExchangeGroup);
  m_pMainLayout->addWidget(pAnalysisGroup);
  m_pMainLayout->addWidget(pProcessingGroup);
  m_pMainLayout->addWidget(pVisuGroup);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  //
  this->setLayout(m_pMainLayout);

  // Connect signals to slots
  connect( m_widgets.pLoadBRep,         SIGNAL( clicked() ), SLOT( onLoadBRep         () ) );
  connect( m_widgets.pLoadSTEP,         SIGNAL( clicked() ), SLOT( onLoadSTEP         () ) );
  connect( m_widgets.pLoadSTL,          SIGNAL( clicked() ), SLOT( onLoadSTL          () ) );
  connect( m_widgets.pSaveSTEP,         SIGNAL( clicked() ), SLOT( onSaveSTEP         () ) );
  connect( m_widgets.pSavePly,          SIGNAL( clicked() ), SLOT( onSavePly          () ) );
  connect( m_widgets.pSaveBRep,         SIGNAL( clicked() ), SLOT( onSaveBRep         () ) );
  //
  connect( m_widgets.pShowTOPOGraph,    SIGNAL( clicked() ), SLOT( onShowTOPOGraph    () ) );
  connect( m_widgets.pShowAAG,          SIGNAL( clicked() ), SLOT( onShowAAG          () ) );
  connect( m_widgets.pElimSelected,     SIGNAL( clicked() ), SLOT( onElimSelected     () ) );
  connect( m_widgets.pCheckShape,       SIGNAL( clicked() ), SLOT( onCheckShape       () ) );
  connect( m_widgets.pTolerance,        SIGNAL( clicked() ), SLOT( onTolerance        () ) );
  connect( m_widgets.pNonManifoldEdges, SIGNAL( clicked() ), SLOT( onNonManifoldEdges () ) );
  //
  connect( m_widgets.pSewing,           SIGNAL( clicked() ), SLOT( onSewing           () ) );
  connect( m_widgets.pMaximizeFaces,    SIGNAL( clicked() ), SLOT( onMaximizeFaces    () ) );
  connect( m_widgets.pOBB,              SIGNAL( clicked() ), SLOT( onOBB              () ) );
  connect( m_widgets.pCR,               SIGNAL( clicked() ), SLOT( onCR               () ) );
  connect( m_widgets.pCloudify,         SIGNAL( clicked() ), SLOT( onCloudify         () ) );
  connect( m_widgets.pMultiLine,        SIGNAL( clicked() ), SLOT( onMultiLine        () ) );
  //
  connect( m_widgets.pShowVertices,     SIGNAL( clicked() ), SLOT( onShowVertices     () ) );
  connect( m_widgets.pShowNormals,      SIGNAL( clicked() ), SLOT( onShowNormals      () ) );
  connect( m_widgets.pSelectFaces,      SIGNAL( clicked() ), SLOT( onSelectFaces      () ) );
  connect( m_widgets.pSelectEdges,      SIGNAL( clicked() ), SLOT( onSelectEdges      () ) );
}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_ControlsPart::~asiUI_ControlsPart()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//-----------------------------------------------------------------------------

//! On BREP loading.
void asiUI_ControlsPart::onLoadBRep()
{
  QString filename = asiUI_Common::selectBRepFile(asiUI_Common::OpenSaveAction_Open);

  // Read BREP
  TopoDS_Shape shape;
  if ( !asiAlgo_Utils::ReadBRep(QStr2AsciiStr(filename), shape) )
  {
    std::cout << "Error: cannot read b-rep file" << std::endl;
    return;
  }

  // Clean up the Model
  common_facilities::Instance()->Model->Clear();

  // Set part geometry
  Handle(asiData_PartNode) geom_n = common_facilities::Instance()->Model->GetPartNode();
  //
  common_facilities::Instance()->Model->OpenCommand(); // tx start
  {
    geom_n->SetShape(shape);
  }
  common_facilities::Instance()->Model->CommitCommand(); // tx commit

  // Update UI
  common_facilities::Instance()->Prs.DeleteAll();
  common_facilities::Instance()->Prs.Part->InitializePickers();
  common_facilities::Instance()->Prs.Part->Actualize(geom_n.get(), false, true);
  //
  if ( common_facilities::Instance()->ObjectBrowser )
    common_facilities::Instance()->ObjectBrowser->Populate();
}

//-----------------------------------------------------------------------------

//! Loads model from STEP file.
void asiUI_ControlsPart::onLoadSTEP()
{
  // Dialog for reading STEP
  asiUI_DialogSTEP* pDlg = new asiUI_DialogSTEP(asiUI_DialogSTEP::Mode_Read, this);
  pDlg->show();
}

//-----------------------------------------------------------------------------

//! On STL loading.
void asiUI_ControlsPart::onLoadSTL()
{
  QString filename = asiUI_Common::selectSTLFile(asiUI_Common::OpenSaveAction_Open);

  // Read STL
  TopoDS_Shape shape;
  if ( !asiAlgo_Utils::ReadSTL(QStr2AsciiStr(filename), shape) )
  {
    std::cout << "Error: cannot read STL file" << std::endl;
    return;
  }

  // Clean up the Model
  common_facilities::Instance()->Model->Clear();

  // Set part geometry
  Handle(asiData_PartNode) geom_n = common_facilities::Instance()->Model->GetPartNode();
  //
  common_facilities::Instance()->Model->OpenCommand(); // tx start
  {
    geom_n->SetShape(shape);
  }
  common_facilities::Instance()->Model->CommitCommand(); // tx commit

  // Update UI
  common_facilities::Instance()->Prs.DeleteAll();
  common_facilities::Instance()->Prs.Part->InitializePickers();
  common_facilities::Instance()->Prs.Part->Actualize(geom_n.get(), false, true);
  //
  if ( common_facilities::Instance()->ObjectBrowser )
    common_facilities::Instance()->ObjectBrowser->Populate();
}

//-----------------------------------------------------------------------------

//! Saves model to STEP file.
void asiUI_ControlsPart::onSaveSTEP()
{
  TopoDS_Shape part;
  if ( !asiUI_Common::PartShape(part) ) return;

  // Dialog for STEP save
  asiUI_DialogSTEP* pDlg = new asiUI_DialogSTEP(asiUI_DialogSTEP::Mode_Write, this);
  pDlg->show();
}

//-----------------------------------------------------------------------------

//! Saves mesh to PLY file.
void asiUI_ControlsPart::onSavePly()
{
  TopoDS_Shape part;
  if ( !asiUI_Common::PartShape(part) ) return;
  //
  QString filename = asiUI_Common::selectPlyFile(asiUI_Common::OpenSaveAction_Save);

  // Convert shape's inherent mesh to a storable mesh
  Handle(Mesh) storedMesh;
  if ( !asiAlgo_MeshConvert::ToPersistent(part, storedMesh) )
  {
    std::cout << "Error: cannot convert mesh to persistent form" << std::endl;
    return;
  }

  // Save mesh to ply files
  if ( !asiAlgo_PLY::Write( storedMesh, QStr2AsciiStr(filename) ) )
  {
    std::cout << "Error: cannot save mesh" << std::endl;
    return;
  }
}

//-----------------------------------------------------------------------------

//! Saves model to BREP file.
void asiUI_ControlsPart::onSaveBRep()
{
  TopoDS_Shape part;
  if ( !asiUI_Common::PartShape(part) ) return;
  //
  QString filename = asiUI_Common::selectBRepFile(asiUI_Common::OpenSaveAction_Save);

  // Save shape
  if ( !asiAlgo_Utils::WriteBRep( part, QStr2AsciiStr(filename) ) )
  {
    std::cout << "Error: cannot save shape" << std::endl;
    return;
  }
}

//-----------------------------------------------------------------------------

//! Shows topology graph.
void asiUI_ControlsPart::onShowTOPOGraph()
{
  // Access Geometry Node
  Handle(asiData_PartNode) N = common_facilities::Instance()->Model->GetPartNode();
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

    TopTools_IndexedMapOfShape M;
    TopExp::MapShapes(N->GetShape(), M);
    const TopoDS_Shape& shape = M.FindKey(f_idx);

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
  asiVisu_TopoGraph* pGraphView = new asiVisu_TopoGraph;
  pGraphView->RenderFull(targetShape, TopAbs_VERTEX);
}

//-----------------------------------------------------------------------------

//! Shows AA graph.
void asiUI_ControlsPart::onShowAAG()
{
  TopoDS_Shape part;
  if ( !asiUI_Common::PartShape(part) ) return;

  // Access selected faces (if any)
  TopTools_IndexedMapOfShape selected;
  asiEngine_Part::GetHighlightedSubShapes(selected);

  // Show graph
  asiVisu_TopoGraph* pGraphView = new asiVisu_TopoGraph;
  pGraphView->RenderAdjacency(part, selected);
}

//-----------------------------------------------------------------------------

//! Eliminates selected faces from AAG.
void asiUI_ControlsPart::onElimSelected()
{
  TopoDS_Shape part;
  if ( !asiUI_Common::PartShape(part) ) return;

  // Get highlighted sub-shapes
  TopTools_IndexedMapOfShape selected;
  asiEngine_Part::GetHighlightedSubShapes(selected);

  // Build AAG
  Handle(feature_aag) aag = new feature_aag(part);

  // Remove highlighted sub-shapes
  aag->Remove(selected);

  // Show graph
  asiVisu_TopoGraph* pGraphView = new asiVisu_TopoGraph;
  pGraphView->SetAAG(aag);
  //
  vtkSmartPointer<vtkMutableUndirectedGraph> undirected = feature_aag_vtk::Convert(aag);
  pGraphView->Render(undirected, part, asiVisu_TopoGraph::Regime_AAG);
}

//-----------------------------------------------------------------------------

//! Runs shape checker.
void asiUI_ControlsPart::onCheckShape()
{
  TopoDS_Shape part;
  if ( !asiUI_Common::PartShape(part) ) return;

  // Get highlighted faces
  TopTools_IndexedMapOfShape selected;
  asiEngine_Part::GetHighlightedSubShapes(selected);

  if ( selected.IsEmpty() )
  {
    if ( !asiAlgo_Utils::CheckShape(part, common_facilities::Instance()->Notifier) )
    {
      std::cout << "Error: shape is invalid" << std::endl;
      return;
    }
    std::cout << "Shape is correct" << std::endl;
  }
  else
  {
    for ( int i = 1; i <= selected.Extent(); ++i )
    {
      if ( !asiAlgo_Utils::CheckShape(selected(i), common_facilities::Instance()->Notifier) )
      {
        std::cout << "Error: sub-shape is invalid" << std::endl;
        return;
      }
      std::cout << "Sub-shape is correct" << std::endl;
    }
  }
}

//-----------------------------------------------------------------------------

//! Consults tolerance.
void asiUI_ControlsPart::onTolerance()
{
  TopoDS_Shape part;
  if ( !asiUI_Common::PartShape(part) ) return;

  // Get highlighted faces
  TopTools_IndexedMapOfShape selected;
  asiEngine_Part::GetHighlightedSubShapes(selected);

  if ( selected.IsEmpty() )
  {
    const double tol = asiAlgo_Utils::MaxTolerance(part);
    std::cout << "Tolerance: " << tol << std::endl;
  }
  else
  {
    double maxTol = 0.0;
    for ( int i = 1; i <= selected.Extent(); ++i )
    {
      const double tol = asiAlgo_Utils::MaxTolerance( selected(i) );
      maxTol = std::max(tol, maxTol);
    }
    std::cout << "Tolerance (sub-shapes): " << maxTol << std::endl;
  }
}

//-----------------------------------------------------------------------------

//! Finds non-manifold edges.
void asiUI_ControlsPart::onNonManifoldEdges()
{
  TopoDS_Shape part;
  if ( !asiUI_Common::PartShape(part) ) return;

  // Find non-manifold edges
  TopTools_IndexedMapOfShape nmEdges;
  asiAlgo_FindNonmanifold::FindEdges(part, nmEdges, common_facilities::Instance()->Notifier);
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
    BN = common_facilities::Instance()->Model->GetPartNode()->GetBoundaryEdgesRepresentation();
  //
  common_facilities::Instance()->Model->OpenCommand();
  {
    ActParamTool::AsShape( BN->Parameter(asiData_BoundaryEdgesNode::PID_Red) )->SetShape(nmEdgesComp);
  }
  common_facilities::Instance()->Model->CommitCommand();

  // Update viewer
  Handle(asiVisu_GeomPrs)
    NPrs = Handle(asiVisu_GeomPrs)::DownCast(
      common_facilities::Instance()->Prs.Part->GetPresentation( common_facilities::Instance()->Model->GetPartNode() )
    );
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
  common_facilities::Instance()->Prs.Part->Actualize( BN.get() );
}

//-----------------------------------------------------------------------------

//! Runs sewing.
void asiUI_ControlsPart::onSewing()
{
  TopoDS_Shape part;
  if ( !asiUI_Common::PartShape(part) ) return;

  // Run dialog for sewing properties
  asiUI_DialogSewing* wSewing = new asiUI_DialogSewing(this);
  wSewing->show();
}

//-----------------------------------------------------------------------------

//! Runs face maximization.
void asiUI_ControlsPart::onMaximizeFaces()
{
  // Access Geometry Node
  Handle(asiData_PartNode) N = common_facilities::Instance()->Model->GetPartNode();
  if ( N.IsNull() || !N->IsWellFormed() )
    return;

  // Shape to visualize a graph for
  TopoDS_Shape part = N->GetShape();
  //
  if ( part.IsNull() )
  {
    std::cout << "Error: part shape is null" << std::endl;
    return;
  }

  common_facilities::Instance()->Model->OpenCommand();
  {
    TIMER_NEW
    TIMER_GO

    // Perform merge
    if ( !asiAlgo_Utils::MaximizeFaces(part) )
    {
      std::cout << "Error: face merging failed" << std::endl;
      common_facilities::Instance()->Model->AbortCommand();
      return;
    }

    TIMER_FINISH
    TIMER_COUT_RESULT_MSG("Face Maximization")

    //
    std::cout << "Face merging done. Visualizing..." << std::endl;
    //
    N->SetShape(part);
  }
  common_facilities::Instance()->Model->CommitCommand();

  // Update viewer
  common_facilities::Instance()->Model->Clear();
  //
  common_facilities::Instance()->Prs.DeleteAll();
  common_facilities::Instance()->Prs.Part->InitializePickers();
  common_facilities::Instance()->Prs.Part->Actualize( N.get() );
}

//-----------------------------------------------------------------------------

//! Builds Oriented Bounding Box.
void asiUI_ControlsPart::onOBB()
{
  TopoDS_Shape part;
  if ( !asiUI_Common::PartShape(part) ) return;

  TIMER_NEW
  TIMER_GO

  // OBB builder
  feature_build_obb obb_builder(part, feature_build_obb::Mode_Inertia,
                                common_facilities::Instance()->Notifier,
                                common_facilities::Instance()->Plotter);
  //
  if ( !obb_builder.Perform() )
  {
    std::cout << "Error: cannot build OBB" << std::endl;
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Build OBB")

  // Draw result
  ActAPI_PlotterEntry IV(common_facilities::Instance()->Plotter);
  IV.DRAW_SHAPE(obb_builder.GetResult()->BuildSolid(), Color_White, 0.3, true);
}

//-----------------------------------------------------------------------------

//! Runs canonical recognition.
void asiUI_ControlsPart::onCR()
{
  // Access Geometry Node
  Handle(asiData_PartNode) N = common_facilities::Instance()->Model->GetPartNode();
  if ( N.IsNull() || !N->IsWellFormed() )
    return;

  // Get part
  TopoDS_Shape part = N->GetShape();
  //
  if ( part.IsNull() )
  {
    std::cout << "Error: part shape is null" << std::endl;
    return;
  }

  TColStd_PackedMapOfInteger modifiedFaces, selected;

  common_facilities::Instance()->Model->OpenCommand();
  {
    TIMER_NEW
    TIMER_GO

    // CR tool
    feature_cr recognizer(part,
                          common_facilities::Instance()->Notifier,
                          common_facilities::Instance()->Plotter);
    //
    if ( !recognizer.Perform(1e-6) )
    {
      std::cout << "Error: canonical recognition failed" << std::endl;
      common_facilities::Instance()->Model->AbortCommand();
      return;
    }

    TIMER_FINISH
    TIMER_COUT_RESULT_MSG("Canonical Recognition")

    const TopoDS_Shape& result = recognizer.GetResult();
    //
    std::cout << "Recognition done. Visualizing..." << std::endl;
    //
    N->SetShape(result);

    // Get converted faces
    recognizer.GetHistory()->GetModified(modifiedFaces);
  }
  common_facilities::Instance()->Model->CommitCommand();

  // Update viewer
  common_facilities::Instance()->Model->Clear();
  //
  common_facilities::Instance()->Prs.DeleteAll();
  common_facilities::Instance()->Prs.Part->InitializePickers();
  common_facilities::Instance()->Prs.Part->Actualize( N.get() );

  // Highlight the modified faces
  asiEngine_Part::GetSubShapeIndicesByFaceIndices(modifiedFaces, selected);
  asiEngine_Part::HighlightSubShapes(selected);
}

//-----------------------------------------------------------------------------

//! Builds point cloud from the model.
void asiUI_ControlsPart::onCloudify()
{
  TopoDS_Shape part;
  if ( !asiUI_Common::PartShape(part) ) return;

  // Run dialog for cloudification
  asiUI_DialogCloudify* wCloudify = new asiUI_DialogCloudify(this);
  wCloudify->show();
}

//-----------------------------------------------------------------------------

//! Conducts multi-line experiment.
void asiUI_ControlsPart::onMultiLine()
{
  TopoDS_Shape part;
  if ( !asiUI_Common::PartShape(part) ) return;

  // Imperative plotter
  ActAPI_PlotterEntry IV(common_facilities::Instance()->Plotter);

  const int NPTS = 1000;

  // Get all highlighted sub-shapes
  TopTools_IndexedMapOfShape subShapes;
  asiEngine_Part::GetHighlightedSubShapes(subShapes);

  // Perform discretization
  int nEdges = 0;
  //
  std::vector<Handle(asiAlgo_PointCloud)> pclouds;
  //
  for ( int k = 1; k <= subShapes.Extent(); ++k )
  {
    const TopoDS_Shape& subShape = subShapes(k);
    //
    if ( subShape.ShapeType() != TopAbs_EDGE )
      continue;
    //
    const TopoDS_Edge& edge = TopoDS::Edge(subShape);

    // Use quasi-uniform discretizer as we need exactly the same number of points
    BRepAdaptor_Curve bac(edge);
    GCPnts_QuasiUniformAbscissa absc(bac, NPTS);
    //
    if ( !absc.IsDone() )
    {
      std::cout << "Error: cannot build discretization on an edge" << std::endl;
      continue;
    }

    // Loop over the discretization points
    Handle(HRealArray) pts = new HRealArray(0, absc.NbPoints()*3 - 1);
    //
    for ( int p = 1, pp = 0; p <= absc.NbPoints(); ++p )
    {
      const double param = absc.Parameter(p);
      gp_Pnt         pnt = bac.Value(param);
      //
      pts->ChangeValue(pp++) = pnt.X();
      pts->ChangeValue(pp++) = pnt.Y();
      pts->ChangeValue(pp++) = pnt.Z();
    }
    //
    IV.DRAW_POINTS(pts, Color_Green);

    pclouds.push_back( new asiAlgo_PointCloud(pts) );
    ++nEdges;
  }

  if ( !nEdges )
    return;

  // Initialize multiline constraints
  AppDef_Array1OfMultiPointConstraint arr(1, NPTS);
  //
  for ( int idx = 0; idx < NPTS; ++idx )
  {
    // Fill MPC
    Handle(TColgp_HArray1OfPnt) mpc_data = new TColgp_HArray1OfPnt(1, nEdges);
    //
    for ( int eidx = 0; eidx < nEdges; ++eidx )
    {
      gp_Pnt pnt = pclouds[eidx]->GetPoint(idx);
      mpc_data->ChangeValue(eidx + 1) = pnt;
    }

    AppDef_MultiPointConstraint MPC( mpc_data->Array1() );
    arr.SetValue(idx + 1, MPC);
  }

  // Multi-line approximation
  AppDef_MultiLine multiline(arr);
  //
  Handle(AppParCurves_HArray1OfConstraintCouple)
    TABofCC = new AppParCurves_HArray1OfConstraintCouple( 1, NPTS );
  //
  for ( int i = 1; i <= NPTS; ++i )
  {
    AppParCurves_ConstraintCouple ACC(i, AppParCurves_PassPoint);
    TABofCC->SetValue(i, ACC);
  }

  // Run approximation
  AppDef_Variational bcompute(multiline, 1, NPTS, TABofCC);
  bcompute.Approximate();

  // Build the resulting geometry
  const AppParCurves_MultiBSpCurve& TheCurve = bcompute.Value();

  // All curves are compatible at this stage: they have the same number
  // of knots, poles, and the same degree.
  int CurveDegree = TheCurve.Degree();
  const TColStd_Array1OfReal& CurveKnots = TheCurve.Knots();
  const TColStd_Array1OfInteger& CurveMults = TheCurve.Multiplicities();
  int nbPoles = TheCurve.NbPoles();
  //
  TopoDS_Compound comp;
  BRep_Builder().MakeCompound(comp);
  //
  for ( int k = 0; k < TheCurve.NbCurves(); ++k )
  {
    TColgp_Array1OfPnt Poles(1, nbPoles);
    TheCurve.Curve(k + 1, Poles);
    //
    Handle(Geom_BSplineCurve)
      bcurve = new Geom_BSplineCurve(Poles, CurveKnots, CurveMults, CurveDegree);

    TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(bcurve);
    BRep_Builder().Add(comp, edge);

    IV.DRAW_CURVE(bcurve, Color_Blue);
  }

  /*TCollection_AsciiString name("D:/bentley.brep");
  BRepTools::Write( comp, name.ToCString() );*/
}

//-----------------------------------------------------------------------------

//! Switches visualization of vertices.
void asiUI_ControlsPart::onShowVertices()
{
  // Access Geometry Node
  Handle(asiData_PartNode) N = common_facilities::Instance()->Model->GetPartNode();
  if ( N.IsNull() || !N->IsWellFormed() )
    return;

  const bool isOn = m_widgets.pShowVertices->isChecked();

  // Modify data
  common_facilities::Instance()->Model->OpenCommand();
  {
    N->SetHasVertices(isOn);
  }
  common_facilities::Instance()->Model->CommitCommand();

  // Actualize
  common_facilities::Instance()->Prs.DeleteAll();
  common_facilities::Instance()->Prs.Part->InitializePickers();
  common_facilities::Instance()->Prs.Part->Actualize( N.get() );
}

//-----------------------------------------------------------------------------

//! Switches visualization of normals.
void asiUI_ControlsPart::onShowNormals()
{
  TopoDS_Shape part;
  if ( !asiUI_Common::PartShape(part) ) return;

  // Calculate boundary box to choose a nice modulus for normal
  Bnd_Box B;
  BRepBndLib::Add(part, B);
  //
  const double modulus = B.CornerMin().Distance( B.CornerMax() ) / 50;

  // Imperative plotter
  ActAPI_PlotterEntry IV(common_facilities::Instance()->Plotter);

  const bool isOn = m_widgets.pShowNormals->isChecked();

  // Iterate over the face and sample the vector field defined by their
  // orientations
  for ( TopExp_Explorer fexp(part, TopAbs_FACE); fexp.More(); fexp.Next() )
  {
    const TopoDS_Face& face = TopoDS::Face( fexp.Current() );

    double uMin, uMax, vMin, vMax;
    BRepTools::UVBounds(face, uMin, uMax, vMin, vMax);

    // Get middle point
    const double um = (uMin + uMax)*0.5;
    const double vm = (vMin + vMax)*0.5;

    Handle(Geom_Surface) surf = BRep_Tool::Surface(face);
    gp_Pnt P;
    gp_Vec D1U, D1V;
    //
    surf->D1(um, vm, P, D1U, D1V);

    // Calculate normal
    gp_Vec norm = D1U ^ D1V;
    if ( norm.Magnitude() < RealEpsilon() )
    {
      std::cout << "Error: degenerated normal" << std::endl;
      IV.DRAW_POINT(P, Color_Black, "Degenerated Face Norm");
      continue;
    }

    norm.Normalize();
    norm *= modulus;

    if ( face.Orientation() == TopAbs_REVERSED )
    {
      IV.DRAW_VECTOR_AT(P, norm, Color_Blue, "Face Norm");
    }
    else
    {
      IV.DRAW_VECTOR_AT(P, norm, Color_Red, "Face Norm");
    }
  }
}

//-----------------------------------------------------------------------------

//! Switches to selection by faces.
void asiUI_ControlsPart::onSelectFaces()
{
  // Access Geometry Node
  Handle(asiData_PartNode) N = common_facilities::Instance()->Model->GetPartNode();
  if ( N.IsNull() || !N->IsWellFormed() )
    return;

  // WHY? ALL? THE FOLLOWING? IS? NECESSARY? WTF?!
  common_facilities::Instance()->Prs.Part->SetSelectionMode(SelectionMode_Face);
  common_facilities::Instance()->Prs.DeleteAll();
  common_facilities::Instance()->Prs.Part->InitializePickers();
  common_facilities::Instance()->Prs.Part->Actualize( N.get() );
}

//-----------------------------------------------------------------------------

//! Switches to selection by edges.
void asiUI_ControlsPart::onSelectEdges()
{
  // Access Geometry Node
  Handle(asiData_PartNode) N = common_facilities::Instance()->Model->GetPartNode();
  if ( N.IsNull() || !N->IsWellFormed() )
    return;

  // WHY? ALL? THE FOLLOWING? IS? NECESSARY? WTF?!
  common_facilities::Instance()->Prs.Part->SetSelectionMode(SelectionMode_Edge);
  common_facilities::Instance()->Prs.DeleteAll();
  common_facilities::Instance()->Prs.Part->InitializePickers();
  common_facilities::Instance()->Prs.Part->Actualize( N.get() );
}
