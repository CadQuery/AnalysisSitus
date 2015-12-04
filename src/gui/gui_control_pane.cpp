//-----------------------------------------------------------------------------
// Created on: 25 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <gui_control_pane.h>

// A-Situs (common) includes
#include <common_facilities.h>

// A-Situs (GUI) includes
#include <gui_common.h>

// A-Situs (mesh) includes
#include <mesh_ply.h>

// A-Situs (modeling) includes
#include <geom_STEP.h>
#include <geom_utils.h>

// Qt includes
#pragma warning(push, 0)
#include <QFileDialog>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Takes care of proper creation of a Geometry Node which would contain all
//! possible representation items.
//! \param shape [in] CAD part to represent in a Data Model.
//! \return instance of the just created Node.
static Handle(geom_node) CreateGeometryNode(const TopoDS_Shape& shape)
{
  Handle(geom_node) geom_n;
  //
  common_facilities::Instance()->Model->OpenCommand(); // tx start
  {
    // Add geom Node to Partition
    Handle(ActAPI_INode) geom_base = geom_node::Instance();
    common_facilities::Instance()->Model->GeomPartition()->AddNode(geom_base);

    // Initialize geometry
    geom_n = Handle(geom_node)::DownCast(geom_base);
    geom_n->Init();
    geom_n->SetShape(shape);
    geom_n->SetName("Geometry");

    // Create underlying face representation Node
    {
      Handle(ActAPI_INode) geom_face_base = geom_face_node::Instance();
      common_facilities::Instance()->Model->GeomFacePartition()->AddNode(geom_face_base);

      // Initialize
      Handle(geom_face_node) geom_face_n = Handle(geom_face_node)::DownCast(geom_face_base);
      geom_face_n->Init();
      geom_face_n->SetName("Face domain");

      // Set as child
      geom_n->AddChildNode(geom_face_n);
    }

    // Create underlying surface representation Node
    {
      Handle(ActAPI_INode) geom_surf_base = geom_surf_node::Instance();
      common_facilities::Instance()->Model->GeomSurfacePartition()->AddNode(geom_surf_base);

      // Initialize
      Handle(geom_surf_node) geom_surf_n = Handle(geom_surf_node)::DownCast(geom_surf_base);
      geom_surf_n->Init();
      geom_surf_n->SetName("Host surface");

      // Set as child
      geom_n->AddChildNode(geom_surf_n);
    }

    // Set as a child for root
    common_facilities::Instance()->Model->GetRootNode()->AddChildNode(geom_n);
  }
  common_facilities::Instance()->Model->CommitCommand(); // tx commit
  //
  return geom_n;
}

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param parent [in] parent widget.
gui_control_pane::gui_control_pane(QWidget* parent) : QWidget(parent)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Buttons
  m_buttons.LoadPly  = new QPushButton("Load &ply");
  m_buttons.LoadBRep = new QPushButton("Load &b-rep");
  m_buttons.LoadSTEP = new QPushButton("Load &STEP");

  // Set layout
  m_pMainLayout->setSpacing(0);
  //
  m_pMainLayout->addWidget(m_buttons.LoadPly);
  m_pMainLayout->addWidget(m_buttons.LoadBRep);
  m_pMainLayout->addWidget(m_buttons.LoadSTEP);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  m_pMainLayout->setContentsMargins(10, 10, 10, 10);
  //
  this->setLayout(m_pMainLayout);

  // Connect signals to slots
  connect( m_buttons.LoadPly,  SIGNAL( clicked() ), SLOT( onLoadPly() ) );
  connect( m_buttons.LoadBRep, SIGNAL( clicked() ), SLOT( onLoadBRep() ) );
  connect( m_buttons.LoadSTEP, SIGNAL( clicked() ), SLOT( onLoadSTEP() ) );
}

//! Destructor.
gui_control_pane::~gui_control_pane()
{
  delete m_pMainLayout;
  m_buttons.Release();
}

//-----------------------------------------------------------------------------
// Slots
//-----------------------------------------------------------------------------

//! On ply loading.
void gui_control_pane::onLoadPly()
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
void gui_control_pane::onLoadBRep()
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

  // Create Node
  Handle(geom_node) geom_n = ::CreateGeometryNode(shape);

  //---------------------------------------------------------------------------
  // Update UI
  //---------------------------------------------------------------------------

  common_facilities::Instance()->Prs.Part->Actualize(geom_n.get(), false, true);
  //
  common_facilities::Instance()->ObjectBrowser->Populate();
}

//! On STEP loading.
void gui_control_pane::onLoadSTEP()
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

  // Create Node
  Handle(geom_node) geom_n = ::CreateGeometryNode(shape);

  //---------------------------------------------------------------------------
  // Update UI
  //---------------------------------------------------------------------------

  common_facilities::Instance()->Prs.Part->Actualize(geom_n.get(), false, true);
  //
  common_facilities::Instance()->ObjectBrowser->Populate();
}

//-----------------------------------------------------------------------------
// Auxiliary functions
//-----------------------------------------------------------------------------

//! Allows to select filename for import.
//! \return selected filename.
QString gui_control_pane::selectPlyFile() const
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
QString gui_control_pane::selectBRepFile() const
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
QString gui_control_pane::selectSTEPFile() const
{
  QStringList aFilter;
  aFilter << "STEP (*.stp)";

  QString dir;
  QString
    aFileName = QFileDialog::getOpenFileName(NULL, "Select STEP file",
                                             dir, aFilter.join(";;"), NULL);

  return aFileName;
}
