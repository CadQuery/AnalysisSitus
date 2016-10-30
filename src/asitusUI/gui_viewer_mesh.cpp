//-----------------------------------------------------------------------------
// Created on: 14 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_viewer_mesh.h>

// Common includes
#include <common_facilities.h>

// Visualization includes
#include <visu_utils.h>

// GUI includes
#include <gui_controls_mesh.h>

// VTK includes
#include <vtkAssembly.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkVertex.h>

// Qt-VTK includes
#include <QVTKWidget.h>

// Qt includes
#pragma warning(push, 0)
#include <QVBoxLayout>
#pragma warning(pop)

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

//-----------------------------------------------------------------------------

//! Creates a new instance of viewer.
//! \param parent [in] parent widget.
gui_viewer_mesh::gui_viewer_mesh(QWidget* parent) : gui_viewer(parent)
{
  // Initialize Presentation Manager along with QVTK widget
  common_facilities::Instance()->Prs.Mesh = vtkSmartPointer<visu_prs_manager>::New();
  m_prs_mgr = common_facilities::Instance()->Prs.Mesh;
  //
  m_prs_mgr->Initialize(this);
  m_prs_mgr->SetInteractionMode(visu_prs_manager::InteractionMode_3D);
  m_prs_mgr->SetSelectionMode(SelectionMode_Workpiece);

  // Widgets and layouts
  gui_controls_mesh* pControlPane = new gui_controls_mesh(this);
  QVTKWidget*        pViewer      = m_prs_mgr->GetQVTKWidget();
  QHBoxLayout*       pBaseLayout  = new QHBoxLayout();

  // Configure layout
  pBaseLayout->setSpacing(0);
  pBaseLayout->addWidget(pControlPane);
  pBaseLayout->addWidget(pViewer);
  pBaseLayout->setAlignment(Qt::AlignTop);
  pBaseLayout->setContentsMargins(0, 0, 0, 0);

  // Set central widget
  this->setLayout(pBaseLayout);

  /* ===================================
   *  Setting up picking infrastructure
   * =================================== */

  // Default interactor style
  m_interactorStyleDefault = vtkSmartPointer<visu_interactor_style_pick>::New();
  m_interactorStyleDefault->SetRenderer( m_prs_mgr->GetRenderer() );

  // Initialize Callback instance for Pick operation
  m_pickCallback = vtkSmartPointer<visu_pick_callback>::New();
  m_pickCallback->SetViewer(this);
  m_pickCallback->SelectMeshNodesOn();

  // Set observer for detection
  if ( !m_interactorStyleDefault->HasObserver(EVENT_PICK_DEFAULT) )
    m_interactorStyleDefault->AddObserver(EVENT_PICK_DEFAULT, m_pickCallback);

  // Set observer for detection
  if ( !m_interactorStyleDefault->HasObserver(EVENT_DETECT_DEFAULT) )
    m_interactorStyleDefault->AddObserver(EVENT_DETECT_DEFAULT, m_pickCallback);

  // Get notified once any sensitive is picked on a mesh
  connect( m_pickCallback, SIGNAL( meshNodePicked() ), this, SLOT( onMeshNodePicked() ) );

  /* ===============================
   *  Setting up rotation callbacks
   * =============================== */

  // Initialize Callback instance for rotation
  m_rotoCallback = vtkSmartPointer<visu_rotation_callback>::New();
  m_rotoCallback->SetViewer(this);

  // Set observer for starting rotation
  if ( !m_interactorStyleDefault->HasObserver(EVENT_ROTATION_START) )
    m_interactorStyleDefault->AddObserver(EVENT_ROTATION_START, m_rotoCallback);

  // Set observer for ending rotation
  if ( !m_interactorStyleDefault->HasObserver(EVENT_ROTATION_END) )
    m_interactorStyleDefault->AddObserver(EVENT_ROTATION_END, m_rotoCallback);

  //---------------------------------------------------------------------------

  // Set default interactor style
  m_prs_mgr->GetQVTKWidget()->GetRenderWindow()->GetInteractor()->SetInteractorStyle(m_interactorStyleDefault);

  /* ========================
   *  Initialize axes widget
   * ======================== */

  vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
  vtkSmartPointer<vtkAssembly> assm = vtkSmartPointer<vtkAssembly>::New();
  assm->AddPart(axes);
  //
  m_axesWidget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
  m_axesWidget->SetOrientationMarker(assm);
  //
  vtkRenderer* renderer = m_prs_mgr->GetRenderer();
  renderer->SetRenderWindow( m_prs_mgr->GetQVTKWidget()->GetRenderWindow() );
  m_axesWidget->SetCurrentRenderer( m_prs_mgr->GetRenderer() );
  m_axesWidget->SetInteractor( m_prs_mgr->GetQVTKWidget()->GetRenderWindow()->GetInteractor() );
  //
  m_axesWidget->SetEnabled(1);
  m_axesWidget->SetInteractive(0);
  m_axesWidget->SetViewport(0, 0, 0.25, 0.25);

  /* =====================================
   *  Finalize initial state of the scene
   * ===================================== */

  this->onResetView();
}

//-----------------------------------------------------------------------------

//! Destructor.
gui_viewer_mesh::~gui_viewer_mesh()
{
}

//-----------------------------------------------------------------------------

//! Updates viewer.
void gui_viewer_mesh::Repaint()
{
  m_prs_mgr->GetQVTKWidget()->repaint();
}

//-----------------------------------------------------------------------------

//! Resets view.
void gui_viewer_mesh::onResetView()
{
  visu_utils::ResetCamera( m_prs_mgr->GetRenderer(), m_prs_mgr->PropsByTrihedron() );
  this->Repaint();
}

//-----------------------------------------------------------------------------

//! Callback for picking event.
void gui_viewer_mesh::onMeshNodePicked()
{
  // Access picking results
  const visu_actual_selection& sel      = m_prs_mgr->GetCurrentSelection();
  const visu_pick_result&      pick_res = sel.PickResult(SelectionNature_Detection);
  const visu_actor_elem_map&   elem_map = pick_res.GetPickMap();

  // Prepare cumulative set of all picked element IDs
  for ( visu_actor_elem_map::Iterator it(elem_map); it.More(); it.Next() )
  {
    const vtkSmartPointer<vtkActor>&  picked_actor = it.Key();
    const TColStd_PackedMapOfInteger& pointIDs     = it.Value();

    // Access polygonal data mapper and its data
    vtkPolyDataMapper* pMapper = vtkPolyDataMapper::SafeDownCast( picked_actor->GetMapper() );
    vtkPolyData*       pData   = vtkPolyData::SafeDownCast( pMapper->GetInput() );

    // Get mesh nodes IDs
    vtkIntArray*
      nodeIDs = vtkIntArray::SafeDownCast( pData->GetPointData()->GetArray(ARRNAME_MESH_NODE_IDS) );

    // Loop over the selected cells
    for ( TColStd_MapIteratorOfPackedMapOfInteger it(pointIDs); it.More(); it.Next() )
    {
      const int pointID = it.Key();
      const int nodeID  = nodeIDs->GetValue(pointID);
      //
      std::cout << "Reacted on PID = " << pointID << " => node is " << nodeID << std::endl;
    }
  }
}
