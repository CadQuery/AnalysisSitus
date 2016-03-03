//-----------------------------------------------------------------------------
// Created on: 14 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_controls_mesh.h>

// A-Situs (common) includes
#include <common_facilities.h>

// A-Situs (GUI) includes
#include <gui_common.h>

// A-Situs (mesh) includes
#include <mesh_convert.h>
#include <mesh_ply.h>

// A-Situs (visualization) includes
#include <visu_mesh_prs.h>

// VTK includes
#include <vtkDecimatePro.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>

//-----------------------------------------------------------------------------

#define BTN_MIN_WIDTH 120

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param parent [in] parent widget.
gui_controls_mesh::gui_controls_mesh(QWidget* parent) : QWidget(parent)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Buttons
  m_widgets.pLoadPly  = new QPushButton("Load &ply");
  m_widgets.pDecimate = new QPushButton("Decimate [VTK]");
  //
  m_widgets.pLoadPly  -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pDecimate -> setMinimumWidth(BTN_MIN_WIDTH);

  // Set layout
  m_pMainLayout->setSpacing(0);
  //
  m_pMainLayout->addWidget(m_widgets.pLoadPly);
  m_pMainLayout->addWidget(m_widgets.pDecimate);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  m_pMainLayout->setContentsMargins(10, 10, 10, 10);
  //
  this->setLayout(m_pMainLayout);

  // Connect signals to slots
  connect( m_widgets.pLoadPly,  SIGNAL( clicked() ), SLOT( onLoadPly() ) );
  connect( m_widgets.pDecimate, SIGNAL( clicked() ), SLOT( onDecimate() ) );
}

//! Destructor.
gui_controls_mesh::~gui_controls_mesh()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//-----------------------------------------------------------------------------
// Slots
//-----------------------------------------------------------------------------

//! On ply loading.
void gui_controls_mesh::onLoadPly()
{
  // Select filename
  QString filename = gui_common::selectPlyFile(gui_common::OpenSaveAction_Open);

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

  // Set mesh
  Handle(mesh_node) mesh_n = common_facilities::Instance()->Model->MeshNode();
  //
  common_facilities::Instance()->Model->OpenCommand(); // tx start
  {
    mesh_n->SetMesh(mesh_data);
  }
  common_facilities::Instance()->Model->CommitCommand(); // tx commit

  //---------------------------------------------------------------------------
  // Update UI
  //---------------------------------------------------------------------------

  common_facilities::Instance()->Prs.Mesh->Actualize(mesh_n.get(), false, true);
}

//-----------------------------------------------------------------------------

//! On decimation.
void gui_controls_mesh::onDecimate()
{
  std::cout << "Decimation" << std::endl;

  // Get Mesh Node
  Handle(mesh_node) mesh_n = common_facilities::Instance()->Model->MeshNode();
  if ( mesh_n.IsNull() || !mesh_n->IsWellFormed() )
  {
    std::cout << "Error: cannot access Mesh Node" << std::endl;
    return;
  }

  // Get Mesh Presentation
  Handle(visu_mesh_prs)
    mesh_prs = Handle(visu_mesh_prs)::DownCast( common_facilities::Instance()->Prs.Mesh
                                                                             ->GetPresentation(mesh_n) );
  if ( mesh_prs.IsNull() )
  {
    std::cout << "Error: seems there is no Presentation for your mesh..." << std::endl;
    return;
  }

  // Access mapper associated with the visualized mesh actor
  vtkMapper*
    mapper = mesh_prs->GetPipeline(visu_mesh_prs::Pipeline_Mesh)->Mapper();
  //
  vtkPolyDataMapper* polyMapper = vtkPolyDataMapper::SafeDownCast(mapper);
  if ( !polyMapper )
  {
    std::cout << "Error: we expected to find a poly mapper, but..." << std::endl;
    return;
  }

  // Access polygonal data
  vtkPolyData* input = polyMapper->GetInput();
  //
  std::cout << "Before decimation" << std::endl << "------------" << std::endl;
  std::cout << "There are " << input->GetNumberOfPoints() << " points" << std::endl;
  std::cout << "There are " << input->GetNumberOfPolys() << " polygons" << std::endl;

  // Prepare decimation algorithm & run
  vtkSmartPointer<vtkDecimatePro> decimate = vtkSmartPointer<vtkDecimatePro>::New();
  decimate->SetInputData(input);
  decimate->PreserveTopologyOn();
  //decimate->SetTargetReduction(.50); // % of reduction (if there was 100 triangles, now there will be 100 - %)
  decimate->Update();

  // Access decimation result
  vtkSmartPointer<vtkPolyData> decimated = vtkSmartPointer<vtkPolyData>::New();
  decimated->ShallowCopy( decimate->GetOutput() );
  //
  std::cout << "After decimation" << std::endl                      << "------------" << std::endl;
  std::cout << "There are "       << decimated->GetNumberOfPoints() << " points."     << std::endl;
  std::cout << "There are "       << decimated->GetNumberOfPolys()  << " polygons."   << std::endl;

  // Convert to a persistent form
  Handle(OMFDS_Mesh) decimatedMesh;
  if ( !mesh_convert::ToPersistent(decimated, decimatedMesh) )
  {
    std::cout << "Error: cannot convert decimated mesh to a persistent form" << std::endl;
    return;
  }

  // Modify mesh
  common_facilities::Instance()->Model->OpenCommand(); // tx open
  {
    mesh_n->SetMesh(decimatedMesh);
  }
  common_facilities::Instance()->Model->CommitCommand(); // tx commit

  // Update Presentation
  common_facilities::Instance()->Prs.Mesh->Actualize( mesh_n.get() );
}
