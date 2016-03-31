//-----------------------------------------------------------------------------
// Created on: 25 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_controls_part.h>

// A-Situs (common) includes
#include <common_draw_test_suite.h>
#include <common_facilities.h>

// A-Situs (GUI) includes
#include <gui_common.h>
#include <gui_dialog_sewing.h>

// A-Situs (engine) includes
#include <engine_part.h>

// A-Situs (modeling) includes
#include <geom_aag_iterator.h>
#include <geom_aag_vtk.h>
#include <geom_delete_faces.h>
#include <geom_detach_faces.h>
#include <geom_STEP.h>
#include <geom_utils.h>

// A-Situs (mesh) includes
#include <mesh_convert.h>
#include <mesh_ply.h>

// A-Situs (visualization) includes
#include <visu_geom_prs.h>
#include <visu_topo_graph.h>

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
// Construction & destruction
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
  m_widgets.pSavePly            = new QPushButton("Save mesh (ply)");
  m_widgets.pSaveBRep           = new QPushButton("Save b-rep");
  //
  m_widgets.pShowTOPOGraph      = new QPushButton("Show TOPO graph");
  m_widgets.pShowAAG            = new QPushButton("Show AA graph");
  m_widgets.pElimSelected       = new QPushButton("Show AA graph w/o selected");
  m_widgets.pCheckShape         = new QPushButton("Check shape");
  m_widgets.pSewing             = new QPushButton("Sewing");
  //
  m_widgets.pLoadBRep           -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pLoadSTEP           -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pSavePly            -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pSaveBRep           -> setMinimumWidth(BTN_MIN_WIDTH);
  //
  m_widgets.pShowTOPOGraph      -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pShowAAG            -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pElimSelected       -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pCheckShape         -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pSewing             -> setMinimumWidth(BTN_MIN_WIDTH);

  // Group box for data interoperability
  QGroupBox*   pExchangeGroup = new QGroupBox("Data Exchange");
  QVBoxLayout* pExchangeLay   = new QVBoxLayout(pExchangeGroup);
  //
  pExchangeLay->addWidget(m_widgets.pLoadBRep);
  pExchangeLay->addWidget(m_widgets.pLoadSTEP);
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
  pAnalysisLay->addWidget(m_widgets.pSewing);

  // Set layout
  m_pMainLayout->addWidget(pExchangeGroup);
  m_pMainLayout->addWidget(pAnalysisGroup);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  //
  this->setLayout(m_pMainLayout);

  // Connect signals to slots
  connect( m_widgets.pLoadBRep,      SIGNAL( clicked() ), SLOT( onLoadBRep           () ) );
  connect( m_widgets.pLoadSTEP,      SIGNAL( clicked() ), SLOT( onLoadSTEP           () ) );
  connect( m_widgets.pSavePly,       SIGNAL( clicked() ), SLOT( onSavePly            () ) );
  connect( m_widgets.pSaveBRep,      SIGNAL( clicked() ), SLOT( onSaveBRep           () ) );
  //
  connect( m_widgets.pShowTOPOGraph, SIGNAL( clicked() ), SLOT( onShowTOPOGraph () ) );
  connect( m_widgets.pShowAAG,       SIGNAL( clicked() ), SLOT( onShowAAG       () ) );
  connect( m_widgets.pElimSelected,  SIGNAL( clicked() ), SLOT( onElimSelected  () ) );
  connect( m_widgets.pCheckShape,    SIGNAL( clicked() ), SLOT( onCheckShape    () ) );
  connect( m_widgets.pSewing,        SIGNAL( clicked() ), SLOT( onSewing        () ) );
}

//! Destructor.
gui_controls_part::~gui_controls_part()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//-----------------------------------------------------------------------------
// Slots
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

  //---------------------------------------------------------------------------
  // Create geometry Node
  //---------------------------------------------------------------------------

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

  //---------------------------------------------------------------------------
  // Update UI
  //---------------------------------------------------------------------------

  common_facilities::Instance()->Prs.DeleteAll();
  common_facilities::Instance()->Prs.Part->InitializePicker();
  common_facilities::Instance()->Prs.Part->Actualize(geom_n.get(), false, true);
  //
  if ( common_facilities::Instance()->ObjectBrowser )
    common_facilities::Instance()->ObjectBrowser->Populate();
}

//! On STEP loading.
void gui_controls_part::onLoadSTEP()
{
  QString filename = gui_common::selectSTEPFile(gui_common::OpenSaveAction_Open);

  // Read STEP
  TopoDS_Shape shape;
  if ( !geom_STEP::Read(QStr2AsciiStr(filename), false, shape) )
  {
    std::cout << "Error: cannot read STEP file" << std::endl;
    return;
  }

  //---------------------------------------------------------------------------
  // Create geometry Node
  //---------------------------------------------------------------------------

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

  //---------------------------------------------------------------------------
  // Update UI
  //---------------------------------------------------------------------------

  common_facilities::Instance()->Prs.DeleteAll();
  common_facilities::Instance()->Prs.Part->InitializePicker();
  common_facilities::Instance()->Prs.Part->Actualize(geom_n.get(), false, true);
  //
  if ( common_facilities::Instance()->ObjectBrowser )
    common_facilities::Instance()->ObjectBrowser->Populate();
}

//! Saves mesh to PLY file.
void gui_controls_part::onSavePly()
{
  QString filename = gui_common::selectPlyFile(gui_common::OpenSaveAction_Save);

  // Access Geometry Node
  Handle(geom_part_node) N = common_facilities::Instance()->Model->PartNode();
  if ( N.IsNull() || !N->IsWellFormed() )
    return;

  // Shape to extract the mesh from
  TopoDS_Shape targetShape = N->GetShape();
  if ( targetShape.IsNull() )
  {
    std::cout << "Error: target shape is null" << std::endl;
    return;
  }

  // Convert shape's inherent mesh to a storable mesh
  Handle(OMFDS_Mesh) storedMesh;
  if ( !mesh_convert::ToPersistent(targetShape, storedMesh) )
  {
    std::cout << "Error: cannot convert mesh to persistent form" << std::endl;
    return;
  }

  // Save mesh to ply files
  if ( !mesh_ply::Write( storedMesh, QStr2AsciiStr(filename) ) )
  {
    std::cout << "Error: cannot save mesh" << std::endl;
    return;
  }
}

//! Saves model to brep file.
void gui_controls_part::onSaveBRep()
{
  QString filename = gui_common::selectBRepFile(gui_common::OpenSaveAction_Save);

  // Access Geometry Node
  Handle(geom_part_node) N = common_facilities::Instance()->Model->PartNode();
  if ( N.IsNull() || !N->IsWellFormed() )
    return;

  // Shape to save
  TopoDS_Shape targetShape = N->GetShape();
  if ( targetShape.IsNull() )
  {
    std::cout << "Error: target shape is null" << std::endl;
    return;
  }

  // Save mesh to ply files
  if ( !geom_utils::WriteBRep( targetShape, QStr2AsciiStr(filename) ) )
  {
    std::cout << "Error: cannot save shape" << std::endl;
    return;
  }
}

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
    const TopoDS_Face& F = TopoDS::Face( M.FindKey(f_idx) );
    //
    targetShape = F;
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

//! Shows AA graph.
void gui_controls_part::onShowAAG()
{
  // Access Geometry Node
  Handle(geom_part_node) N = common_facilities::Instance()->Model->PartNode();
  if ( N.IsNull() || !N->IsWellFormed() )
    return;

  // Shape to visualize a graph for
  TopoDS_Shape targetShape;
  TopoDS_Face selectedFace;

  // Access whole shape
  targetShape = N->GetShape();
  //
  // No shape, no graph
  if ( targetShape.IsNull() )
  {
    std::cout << "Error: target shape is null" << std::endl;
    return;
  }

  // Access selected faces (if any)
  TopTools_IndexedMapOfShape selected;
  engine_part::GetHighlightedSubShapes(selected);

  // Show graph
  visu_topo_graph* pGraphView = new visu_topo_graph;
  pGraphView->RenderAdjacency(targetShape, selected);
}

//! Eliminates selected faces from AAG.
void gui_controls_part::onElimSelected()
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

  // Get highlighted sub-shapes
  TopTools_IndexedMapOfShape selected;
  engine_part::GetHighlightedSubShapes(selected);

  // Build AAG
  Handle(geom_aag) aag = new geom_aag(part);

  // Remove highlighted sub-shapes
  aag->Remove(selected);

  // Show graph
  visu_topo_graph* pGraphView = new visu_topo_graph;
  pGraphView->SetAAG(aag);
  //
  vtkSmartPointer<vtkMutableUndirectedGraph> undirected = geom_aag_vtk::Convert(aag);
  pGraphView->Render(undirected, part, visu_topo_graph::Regime_AAG);
}

//! Runs shape checker.
void gui_controls_part::onCheckShape()
{
  // Access Geometry Node
  Handle(geom_part_node) N = common_facilities::Instance()->Model->PartNode();
  if ( N.IsNull() || !N->IsWellFormed() )
    return;

  // Shape to work with
  TopoDS_Shape part = N->GetShape();
  //
  if ( part.IsNull() )
  {
    std::cout << "Error: part shape is null" << std::endl;
    return;
  }

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

//! Runs sewing.
void gui_controls_part::onSewing()
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

  // Run dialog for sewing properties
  gui_dialog_sewing* wSewing = new gui_dialog_sewing(this);
  wSewing->show();
}