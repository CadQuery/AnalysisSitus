//-----------------------------------------------------------------------------
// Created on: 25 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

// Own include
#include <gui_controls_part.h>

// A-Situs (common) includes
#include <common_facilities.h>

// A-Situs (GUI) includes
#include <gui_common.h>

// A-Situs (modeling) includes
#include <geom_STEP.h>
#include <geom_utils.h>

// A-Situs (mesh) includes
#include <mesh_convert.h>
#include <mesh_ply.h>

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
  m_widgets.pLoadBRep  = new QPushButton("Load &b-rep");
  m_widgets.pLoadSTEP  = new QPushButton("Load &STEP");
  m_widgets.pShowGraph = new QPushButton("Show &graph");
  m_widgets.pSavePly   = new QPushButton("Save mesh (ply)");
  //
  m_widgets.pLoadBRep  -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pLoadSTEP  -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pShowGraph -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pSavePly   -> setMinimumWidth(BTN_MIN_WIDTH);

  // Set layout
  m_pMainLayout->setSpacing(0);
  //
  m_pMainLayout->addWidget(m_widgets.pLoadBRep);
  m_pMainLayout->addWidget(m_widgets.pLoadSTEP);
  m_pMainLayout->addWidget(m_widgets.pShowGraph);
  m_pMainLayout->addWidget(m_widgets.pSavePly);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  m_pMainLayout->setContentsMargins(10, 10, 10, 10);
  //
  this->setLayout(m_pMainLayout);

  // Connect signals to slots
  connect( m_widgets.pLoadBRep,  SIGNAL( clicked() ), SLOT( onLoadBRep() ) );
  connect( m_widgets.pLoadSTEP,  SIGNAL( clicked() ), SLOT( onLoadSTEP() ) );
  connect( m_widgets.pShowGraph, SIGNAL( clicked() ), SLOT( onShowGraph() ) );
  connect( m_widgets.pSavePly,   SIGNAL( clicked() ), SLOT( onSavePly() ) );
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

  // No shape, no graph
  if ( targetShape.IsNull() )
  {
    std::cout << "Error: target shape is null" << std::endl;
    return;
  }

  // Show graph
  visu_topo_graph* pGraphView = new visu_topo_graph;
  pGraphView->Render(targetShape);
}

//! Saves mesh to PLY file.
void gui_controls_part::onSavePly()
{
  QString filename = this->selectPlyFile();

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

//-----------------------------------------------------------------------------
// Auxiliary functions
//-----------------------------------------------------------------------------

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

//! Allows to select filename for ply export.
//! \return selected filename.
QString gui_controls_part::selectPlyFile() const
{
  QStringList aFilter;
  aFilter << "PLY (*.ply)";

  QString dir;
  QString
    aFileName = QFileDialog::getSaveFileName(NULL, "Select PLY file",
                                             dir, aFilter.join(";;"), NULL);

  return aFileName;
}
