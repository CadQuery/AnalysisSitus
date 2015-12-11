//-----------------------------------------------------------------------------
// Created on: 25 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <gui_controls_part.h>

// A-Situs (common) includes
#include <common_facilities.h>

// A-Situs (GUI) includes
#include <gui_common.h>

// A-Situs (mesh) includes
#include <mesh_ply.h>

// A-Situs (modeling) includes
#include <geom_STEP.h>
#include <geom_utils.h>

// A-Situs (visualization) includes
#include <visu_topo_graph.h>

// OCCT includes
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

// Qt includes
#pragma warning(push, 0)
#include <QFileDialog>
#pragma warning(pop)

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
  m_widgets.pLoadPly   = new QPushButton("Load &ply");
  m_widgets.pLoadBRep  = new QPushButton("Load &b-rep");
  m_widgets.pLoadSTEP  = new QPushButton("Load &STEP");
  m_widgets.pShowGraph = new QPushButton("Show &graph");

  // Set layout
  m_pMainLayout->setSpacing(0);
  //
  m_pMainLayout->addWidget(m_widgets.pLoadPly);
  m_pMainLayout->addWidget(m_widgets.pLoadBRep);
  m_pMainLayout->addWidget(m_widgets.pLoadSTEP);
  m_pMainLayout->addWidget(m_widgets.pShowGraph);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  m_pMainLayout->setContentsMargins(10, 10, 10, 10);
  //
  this->setLayout(m_pMainLayout);

  // Connect signals to slots
  connect( m_widgets.pLoadPly,   SIGNAL( clicked() ), SLOT( onLoadPly() ) );
  connect( m_widgets.pLoadBRep,  SIGNAL( clicked() ), SLOT( onLoadBRep() ) );
  connect( m_widgets.pLoadSTEP,  SIGNAL( clicked() ), SLOT( onLoadSTEP() ) );
  connect( m_widgets.pShowGraph, SIGNAL( clicked() ), SLOT( onShowGraph() ) );
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

//! On ply loading.
void gui_controls_part::onLoadPly()
{
  // Select filename
  QString filename = this->selectPlyFile();

  // Load mesh
  Handle(OMFDS_Mesh)                          mesh_data;
  NCollection_Sequence<mesh_ply::TNamedArray> NodeArrays;
  NCollection_Sequence<mesh_ply::TNamedArray> ElemArrays;
  //
  if ( !mesh_ply::Read(QStr2AsciiStr(filename), mesh_data, NodeArrays, ElemArrays) )
  {
    std::cout << "Error: cannot read ply file" << std::endl;
    return;
  }

  //---------------------------------------------------------------------------
  // Create mesh Node
  //---------------------------------------------------------------------------

  // Clean up the Model
  common_facilities::Instance()->Model->Clear();

  Handle(mesh_node) mesh_n;
  //
  common_facilities::Instance()->Model->OpenCommand(); // tx start
  {
    // Add mesh Node to Partition
    Handle(ActAPI_INode) mesh_base = mesh_node::Instance();
    common_facilities::Instance()->Model->MeshPartition()->AddNode(mesh_base);

    // Initialize mesh
    mesh_n = Handle(mesh_node)::DownCast(mesh_base);
    mesh_n->Init();
    mesh_n->SetMesh(mesh_data);
    mesh_n->SetName("Tessellation");

    // Set as a child for root
    common_facilities::Instance()->Model->GetRootNode()->AddChildNode(mesh_n);
  }
  common_facilities::Instance()->Model->CommitCommand(); // tx commit

  //---------------------------------------------------------------------------
  // Update UI
  //---------------------------------------------------------------------------

  common_facilities::Instance()->Prs.Part->Actualize(mesh_n.get(), false, true);
  //
  common_facilities::Instance()->ObjectBrowser->Populate();
}

//! On b-rep loading.
void gui_controls_part::onLoadBRep()
{
  QString filename = this->selectBRepFile();

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

  common_facilities::Instance()->Prs.Part->InitializePicker();
  common_facilities::Instance()->Prs.Part->Actualize(geom_n.get(), false, true);
  //
  if ( common_facilities::Instance()->ObjectBrowser )
    common_facilities::Instance()->ObjectBrowser->Populate();
}

//! On STEP loading.
void gui_controls_part::onLoadSTEP()
{
  QString filename = this->selectSTEPFile();

  // Read STEP
  TopoDS_Shape shape;
  if ( !geom_STEP::Read(QStr2AsciiStr(filename), true, shape) )
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

  common_facilities::Instance()->Prs.Part->InitializePicker();
  common_facilities::Instance()->Prs.Part->Actualize(geom_n.get(), false, true);
  //
  if ( common_facilities::Instance()->ObjectBrowser )
    common_facilities::Instance()->ObjectBrowser->Populate();
}

//! Shows topology graph.
void gui_controls_part::onShowGraph()
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

  // Show graph
  visu_topo_graph* pGraphView = new visu_topo_graph;
  pGraphView->Render(targetShape);
}

//-----------------------------------------------------------------------------
// Auxiliary functions
//-----------------------------------------------------------------------------

//! Allows to select filename for import.
//! \return selected filename.
QString gui_controls_part::selectPlyFile() const
{
  QStringList aFilter;
  aFilter << "PLY (*.ply)";

  QString dir;
  QString
    aFileName = QFileDialog::getOpenFileName(NULL, "Select ply file with mesh",
                                             dir, aFilter.join(";;"), NULL);

  return aFileName;
}

//! Allows to select filename for import.
//! \return selected filename.
QString gui_controls_part::selectBRepFile() const
{
  QStringList aFilter;
  aFilter << "B-Rep (*.brep)";

  QString dir;
  QString
    aFileName = QFileDialog::getOpenFileName(NULL, "Select B-Rep file",
                                             dir, aFilter.join(";;"), NULL);

  return aFileName;
}

//! Allows to select filename for import.
//! \return selected filename.
QString gui_controls_part::selectSTEPFile() const
{
  QStringList aFilter;
  aFilter << "STEP (*.stp)";

  QString dir;
  QString
    aFileName = QFileDialog::getOpenFileName(NULL, "Select STEP file",
                                             dir, aFilter.join(";;"), NULL);

  return aFileName;
}
