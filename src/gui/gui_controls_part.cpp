//-----------------------------------------------------------------------------
// Created on: 25 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_controls_part.h>

// Common includes
#include <common_draw_test_suite.h>
#include <common_facilities.h>

// GUI includes
#include <gui_common.h>
#include <gui_dialog_cloudify.h>
#include <gui_dialog_sewing.h>
#include <gui_dialog_STEP.h>

// Engine includes
#include <engine_part.h>

// Feature includes
#include <feature_aag_iterator.h>
#include <feature_cr.h>
#include <feature_delete_faces.h>

// Geometry includes
#include <geom_aag_vtk.h>
#include <geom_build_obb.h>
#include <geom_detach_faces.h>
#include <geom_find_nonmanifold.h>
#include <geom_STEP.h>
#include <geom_utils.h>

// Mesh includes
#include <tess_convert.h>
#include <tess_ply.h>

// Visualization includes
#include <visu_geom_prs.h>
#include <visu_topo_graph.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRepBndLib.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
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
gui_controls_part::gui_controls_part(QWidget* parent) : QWidget(parent)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Buttons
  m_widgets.pLoadBRep           = new QPushButton("Load b-rep");
  m_widgets.pLoadSTEP           = new QPushButton("Load STEP");
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
  //
  m_widgets.pLoadBRep           -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pLoadSTEP           -> setMinimumWidth(BTN_MIN_WIDTH);
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

  // Group box for data interoperability
  QGroupBox*   pExchangeGroup = new QGroupBox("Data Exchange");
  QVBoxLayout* pExchangeLay   = new QVBoxLayout(pExchangeGroup);
  //
  pExchangeLay->addWidget(m_widgets.pLoadBRep);
  pExchangeLay->addWidget(m_widgets.pLoadSTEP);
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

  // Set layout
  m_pMainLayout->addWidget(pExchangeGroup);
  m_pMainLayout->addWidget(pAnalysisGroup);
  m_pMainLayout->addWidget(pProcessingGroup);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  //
  this->setLayout(m_pMainLayout);

  // Connect signals to slots
  connect( m_widgets.pLoadBRep,         SIGNAL( clicked() ), SLOT( onLoadBRep         () ) );
  connect( m_widgets.pLoadSTEP,         SIGNAL( clicked() ), SLOT( onLoadSTEP         () ) );
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
}

//-----------------------------------------------------------------------------

//! Destructor.
gui_controls_part::~gui_controls_part()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//-----------------------------------------------------------------------------

//! On b-rep loading.
void gui_controls_part::onLoadBRep()
{
  QString filename = gui_common::selectBRepFile(gui_common::OpenSaveAction_Open);

  // Read b-rep
  TopoDS_Shape shape;
  if ( !geom_utils::ReadBRep(QStr2AsciiStr(filename), shape) )
  {
    std::cout << "Error: cannot read b-rep file" << std::endl;
    return;
  }

  // Clean up the Model
  common_facilities::Instance()->Model->Clear();

  // Set part geometry
  Handle(geom_part_node) geom_n = common_facilities::Instance()->Model->PartNode();
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
void gui_controls_part::onLoadSTEP()
{
  // Dialog for reading STEP
  gui_dialog_STEP* pDlg = new gui_dialog_STEP(gui_dialog_STEP::Mode_Read, this);
  pDlg->show();
}

//-----------------------------------------------------------------------------

//! Saves model to STEP file.
void gui_controls_part::onSaveSTEP()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Dialog for STEP save
  gui_dialog_STEP* pDlg = new gui_dialog_STEP(gui_dialog_STEP::Mode_Write, this);
  pDlg->show();
}

//-----------------------------------------------------------------------------

//! Saves mesh to PLY file.
void gui_controls_part::onSavePly()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;
  //
  QString filename = gui_common::selectPlyFile(gui_common::OpenSaveAction_Save);

  // Convert shape's inherent mesh to a storable mesh
  Handle(Mesh) storedMesh;
  if ( !tess_convert::ToPersistent(part, storedMesh) )
  {
    std::cout << "Error: cannot convert mesh to persistent form" << std::endl;
    return;
  }

  // Save mesh to ply files
  if ( !tess_ply::Write( storedMesh, QStr2AsciiStr(filename) ) )
  {
    std::cout << "Error: cannot save mesh" << std::endl;
    return;
  }
}

//-----------------------------------------------------------------------------

//! Saves model to BREP file.
void gui_controls_part::onSaveBRep()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;
  //
  QString filename = gui_common::selectBRepFile(gui_common::OpenSaveAction_Save);

  // Save shape
  if ( !geom_utils::WriteBRep( part, QStr2AsciiStr(filename) ) )
  {
    std::cout << "Error: cannot save shape" << std::endl;
    return;
  }
}

//-----------------------------------------------------------------------------

//! Shows topology graph.
void gui_controls_part::onShowTOPOGraph()
{
  // Access Geometry Node
  Handle(geom_part_node) N = common_facilities::Instance()->Model->PartNode();
  if ( N.IsNull() || !N->IsWellFormed() )
    return;

  // Shape to visualize a graph for
  TopoDS_Shape targetShape;

  // Access active face
  Handle(geom_face_node) FN = N->FaceRepresentation();
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
  visu_topo_graph* pGraphView = new visu_topo_graph;
  pGraphView->RenderFull(targetShape, TopAbs_VERTEX);
}

//-----------------------------------------------------------------------------

//! Shows AA graph.
void gui_controls_part::onShowAAG()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Access selected faces (if any)
  TopTools_IndexedMapOfShape selected;
  engine_part::GetHighlightedSubShapes(selected);

  // Show graph
  visu_topo_graph* pGraphView = new visu_topo_graph;
  pGraphView->RenderAdjacency(part, selected);
}

//-----------------------------------------------------------------------------

//! Eliminates selected faces from AAG.
void gui_controls_part::onElimSelected()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Get highlighted sub-shapes
  TopTools_IndexedMapOfShape selected;
  engine_part::GetHighlightedSubShapes(selected);

  // Build AAG
  Handle(feature_aag) aag = new feature_aag(part);

  // Remove highlighted sub-shapes
  aag->Remove(selected);

  // Show graph
  visu_topo_graph* pGraphView = new visu_topo_graph;
  pGraphView->SetAAG(aag);
  //
  vtkSmartPointer<vtkMutableUndirectedGraph> undirected = feature_aag_vtk::Convert(aag);
  pGraphView->Render(undirected, part, visu_topo_graph::Regime_AAG);
}

//-----------------------------------------------------------------------------

//! Runs shape checker.
void gui_controls_part::onCheckShape()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Get highlighted faces
  TopTools_IndexedMapOfShape selected;
  engine_part::GetHighlightedSubShapes(selected);

  if ( selected.IsEmpty() )
  {
    if ( !geom_utils::CheckShape(part, common_facilities::Instance()->Notifier) )
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
      if ( !geom_utils::CheckShape(selected(i), common_facilities::Instance()->Notifier) )
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
void gui_controls_part::onTolerance()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Get highlighted faces
  TopTools_IndexedMapOfShape selected;
  engine_part::GetHighlightedSubShapes(selected);

  if ( selected.IsEmpty() )
  {
    const double tol = geom_utils::MaxTolerance(part);
    std::cout << "Tolerance: " << tol << std::endl;
  }
  else
  {
    double maxTol = 0.0;
    for ( int i = 1; i <= selected.Extent(); ++i )
    {
      const double tol = geom_utils::MaxTolerance( selected(i) );
      maxTol = std::max(tol, maxTol);
    }
    std::cout << "Tolerance (sub-shapes): " << maxTol << std::endl;
  }
}

//-----------------------------------------------------------------------------

//! Finds non-manifold edges.
void gui_controls_part::onNonManifoldEdges()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Find non-manifold edges
  TopTools_IndexedMapOfShape nmEdges;
  geom_find_nonmanifold::FindEdges(part, nmEdges, common_facilities::Instance()->Notifier);
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
  Handle(geom_boundary_edges_node)
    BN = common_facilities::Instance()->Model->PartNode()->BoundaryEdgesRepresentation();
  //
  common_facilities::Instance()->Model->OpenCommand();
  {
    ActParamTool::AsShape( BN->Parameter(geom_boundary_edges_node::PID_Red) )->SetShape(nmEdgesComp);
  }
  common_facilities::Instance()->Model->CommitCommand();

  // Update viewer
  Handle(visu_geom_prs)
    NPrs = Handle(visu_geom_prs)::DownCast(
      common_facilities::Instance()->Prs.Part->GetPresentation( common_facilities::Instance()->Model->PartNode() )
    );
  if ( NPrs.IsNull() )
  {
    std::cout << "Error: there is no available presentation for part" << std::endl;
    return;
  }
  //
  NPrs->MainActor()->GetProperty()->SetOpacity(0.5);
  //
  NPrs->GetPipeline(visu_geom_prs::Pipeline_Contour)->Actor()->SetVisibility(0);
  //
  common_facilities::Instance()->Prs.Part->Actualize( BN.get() );
}

//-----------------------------------------------------------------------------

//! Runs sewing.
void gui_controls_part::onSewing()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Run dialog for sewing properties
  gui_dialog_sewing* wSewing = new gui_dialog_sewing(this);
  wSewing->show();
}

//-----------------------------------------------------------------------------

//! Runs face maximization.
void gui_controls_part::onMaximizeFaces()
{
  // Access Geometry Node
  Handle(geom_part_node) N = common_facilities::Instance()->Model->PartNode();
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
    if ( !geom_utils::MaximizeFaces(part) )
    {
      std::cout << "Error: face merging failed" << std::endl;
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
void gui_controls_part::onOBB()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  TIMER_NEW
  TIMER_GO

  // OBB builder
  geom_build_obb obb_builder(common_facilities::Instance()->Notifier,
                             common_facilities::Instance()->Plotter);
  //
  if ( !obb_builder(part) )
  {
    std::cout << "Error: cannot build OBB" << std::endl;
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Build OBB")
}

//-----------------------------------------------------------------------------

//! Runs canonical recognition.
void gui_controls_part::onCR()
{
  // Access Geometry Node
  Handle(geom_part_node) N = common_facilities::Instance()->Model->PartNode();
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

    // CR tool
    feature_cr recognizer(part,
                          common_facilities::Instance()->Notifier,
                          common_facilities::Instance()->Plotter);
    //
    if ( !recognizer.Perform() )
    {
      std::cout << "Error: canonical recognition failed" << std::endl;
      common_facilities::Instance()->Model->AbortCommand();
      return;
    }

    TIMER_FINISH
    TIMER_COUT_RESULT_MSG("Canonical Recognition")

    TopoDS_Shape result = recognizer.GetResult();
    //
    std::cout << "Recognition done. Visualizing..." << std::endl;
    //
    N->SetShape(result);
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

//! Builds point cloud from the model.
void gui_controls_part::onCloudify()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Run dialog for cloudification
  gui_dialog_cloudify* wCloudify = new gui_dialog_cloudify(this);
  wCloudify->show();
}
