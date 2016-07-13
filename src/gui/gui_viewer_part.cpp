//-----------------------------------------------------------------------------
// Created on: 27 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_viewer_part.h>

// Common includes
#include <common_facilities.h>

// Visualization includes
#include <visu_node_info.h>
#include <visu_utils.h>

// GUI includes
#include <gui_controls_part.h>
#include <gui_dialog_find_face.h>

// VTK includes
#include <vtkAssembly.h>
#include <vtkRenderWindow.h>

// Qt-VTK includes
#include <QVTKWidget.h>

// Qt includes
#pragma warning(push, 0)
#include <QVBoxLayout>
#pragma warning(pop)

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

//! Creates a new instance of viewer.
//! \param parent [in] parent widget.
gui_viewer_part::gui_viewer_part(QWidget* parent) : gui_viewer(parent)
{
  // Store in common facilities
  common_facilities::Instance()->ViewerPart = this;

  // Initialize Presentation Manager along with QVTK widget
  common_facilities::Instance()->Prs.Part = vtkSmartPointer<visu_prs_manager>::New();
  m_prs_mgr = common_facilities::Instance()->Prs.Part;
  //
  m_prs_mgr->Initialize(this);
  m_prs_mgr->SetInteractionMode(visu_prs_manager::InteractionMode_3D);
  m_prs_mgr->SetSelectionMode(SelectionMode_Face);

  // Widgets and layouts
  QVTKWidget*  pViewer      = m_prs_mgr->GetQVTKWidget();
  QHBoxLayout* pBaseLayout  = new QHBoxLayout();

  // Configure layout
  pBaseLayout->setSpacing(0);
  pBaseLayout->addWidget(pViewer);
  pBaseLayout->setAlignment(Qt::AlignTop);
  pBaseLayout->setContentsMargins(0, 0, 0, 0);

  // Set central widget
  this->setLayout(pBaseLayout);

  /* ===================================
   *  Setting up picking infrastructure
   * =================================== */

  // Initialize Callback instance for Pick operation
  m_pickCallback = vtkSmartPointer<visu_pick_callback>::New();
  m_pickCallback->SetViewer(this);

  // Initialize Callback instance for handling events related to Part viewer
  m_partCallback = vtkSmartPointer<visu_part_callback>::New();
  m_partCallback->SetViewer(this);

  // Set observer for detection
  if ( !m_prs_mgr->GetDefaultInteractorStyle()->HasObserver(EVENT_PICK_DEFAULT) )
    m_prs_mgr->GetDefaultInteractorStyle()->AddObserver(EVENT_PICK_DEFAULT, m_pickCallback);

  // Set observer for detection
  if ( !m_prs_mgr->GetDefaultInteractorStyle()->HasObserver(EVENT_DETECT_DEFAULT) )
    m_prs_mgr->GetDefaultInteractorStyle()->AddObserver(EVENT_DETECT_DEFAULT, m_pickCallback);

  // Set observer for finding face
  if ( !m_prs_mgr->GetDefaultInteractorStyle()->HasObserver(EVENT_FIND_FACE) )
    m_prs_mgr->GetDefaultInteractorStyle()->AddObserver(EVENT_FIND_FACE, m_partCallback);

  // Get notified once a sub-shape is picked
  connect( m_pickCallback, SIGNAL( partPicked() ), this, SLOT( onSubShapesPicked() ) );

  // Get notified about part events
  connect( m_partCallback, SIGNAL( findFace() ), this, SLOT( onFindFace() ) );

  /* ===============================
   *  Setting up rotation callbacks
   * =============================== */

  // Initialize Callback instance for rotation
  m_rotoCallback = vtkSmartPointer<visu_rotation_callback>::New();
  m_rotoCallback->SetViewer(this);

  // Set observer for starting rotation
  if ( !m_prs_mgr->GetDefaultInteractorStyle()->HasObserver(EVENT_ROTATION_START) )
    m_prs_mgr->GetDefaultInteractorStyle()->AddObserver(EVENT_ROTATION_START, m_rotoCallback);

  // Set observer for ending rotation
  if ( !m_prs_mgr->GetDefaultInteractorStyle()->HasObserver(EVENT_ROTATION_END) )
    m_prs_mgr->GetDefaultInteractorStyle()->AddObserver(EVENT_ROTATION_END, m_rotoCallback);

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
  //
  m_axesWidget->SetCurrentRenderer( m_prs_mgr->GetRenderer() );
  m_axesWidget->SetInteractor( m_prs_mgr->GetQVTKWidget()->GetRenderWindow()->GetInteractor() );
  m_axesWidget->SetEnabled(1);
  m_axesWidget->SetInteractive(0);
  m_axesWidget->SetViewport(0, 0, 0.25, 0.25);

  /* =====================================
   *  Finalize initial state of the scene
   * ===================================== */

  this->onResetView();
}

//! Destructor.
gui_viewer_part::~gui_viewer_part()
{
}

//-----------------------------------------------------------------------------

//! Updates viewer.
void gui_viewer_part::Repaint()
{
  m_prs_mgr->GetQVTKWidget()->repaint();
}

//-----------------------------------------------------------------------------

//! Resets view.
void gui_viewer_part::onResetView()
{
  visu_utils::ResetCamera( m_prs_mgr->GetRenderer(), m_prs_mgr->PropsByTrihedron() );
  this->Repaint();
}

//-----------------------------------------------------------------------------

//! Callback for picking event.
void gui_viewer_part::onSubShapesPicked()
{
  Handle(geom_part_node) geom_n = common_facilities::Instance()->Model->PartNode();
  if ( geom_n.IsNull() || !geom_n->IsWellFormed() )
  {
    std::cout << "Geometry Node is not accessible" << std::endl;
    return; // No target Node to proceed with
  }

  if ( !m_prs_mgr->IsPresented(geom_n) )
    return;

  //---------------------------------------------------------------------------
  // Retrieve current selection
  //---------------------------------------------------------------------------

  // Access picking results
  const visu_actual_selection& sel      = m_prs_mgr->GetCurrentSelection();
  const visu_pick_result&      pick_res = sel.PickResult(SelectionNature_Pick);
  const visu_actor_elem_map&   elem_map = pick_res.GetPickMap();

  // Prepare arrays for selected elements
  std::vector<int>                 picked_face_IDs;
  std::vector<ActAPI_DataObjectId> picked_node_IDs;

  // Prepare cumulative set of all picked element IDs
  for ( visu_actor_elem_map::Iterator it(elem_map); it.More(); it.Next() )
  {
    const vtkSmartPointer<vtkActor>&  picked_actor = it.Key();
    const TColStd_PackedMapOfInteger& face_mask    = it.Value();

    // Retrieve the corresponding Node ID by picked Actor
    ActAPI_DataObjectId
      picked_node_id = visu_node_info::Retrieve(picked_actor)->GetNodeId();

    // Fill coherent collections of references: face IDs against owning Nodes
    for ( TColStd_MapIteratorOfPackedMapOfInteger maskIt(face_mask); maskIt.More(); maskIt.Next() )
    {
      picked_face_IDs.push_back( maskIt.Key() );
      picked_node_IDs.push_back(picked_node_id);
    }
  }

  if ( picked_face_IDs.size() )
  {
    std::cout << "Picked faces:";
    for ( size_t k = 0; k < picked_face_IDs.size(); ++k )
    {
      std::cout << " " << picked_face_IDs[k] << " [" << picked_node_IDs[k].ToCString() << "]";
    }
    std::cout << std::endl;
  }

  //---------------------------------------------------------------------------
  // Store active selection in the Data Model
  //---------------------------------------------------------------------------

  common_facilities::Instance()->Model->OpenCommand(); // tx start
  {
    // Store index of the active face
    if ( picked_face_IDs.size() )
    {
      geom_n->FaceRepresentation()    ->SetSelectedFace(picked_face_IDs[0]);
      geom_n->SurfaceRepresentation() ->SetSelectedFace(picked_face_IDs[0]);
      //
      std::cout << "Active face has been stored..." << std::endl;
    }
    else // Reset stored indices
    {
      geom_n->FaceRepresentation()    ->SetSelectedFace(0);
      geom_n->SurfaceRepresentation() ->SetSelectedFace(0);
      //
      std::cout << "Active face has been reset..." << std::endl;
    }
  }
  common_facilities::Instance()->Model->CommitCommand(); // tx commit

  //---------------------------------------------------------------------------
  // Actualize presentations
  //---------------------------------------------------------------------------

  if ( common_facilities::Instance()->Prs.Domain )
    common_facilities::Instance()->Prs.Domain->Actualize(geom_n->FaceRepresentation().get(), false, true);

  if ( common_facilities::Instance()->Prs.Surface )
    common_facilities::Instance()->Prs.Surface->Actualize(geom_n->SurfaceRepresentation().get(), false, true);
}

//-----------------------------------------------------------------------------

//! Callback for face find request.
void gui_viewer_part::onFindFace()
{
  // Run dialog
  gui_dialog_find_face* wFindFace = new gui_dialog_find_face(this);
  wFindFace->show();
}
