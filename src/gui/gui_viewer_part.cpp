//-----------------------------------------------------------------------------
// Created on: 27 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <gui_viewer_part.h>

// Common includes
#include <common_facilities.h>

// Visualization includes
#include <visu_mesh_prs.h>
#include <visu_node_info.h>
#include <visu_utils.h>

// GUI includes
#include <gui_control_pane.h>

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
  // Initialize Presentation Manager along with QVTK widget
  common_facilities::Instance()->Prs.Part = vtkSmartPointer<visu_prs_manager>::New();
  common_facilities::Instance()->Prs.Part->Initialize(this);
  common_facilities::Instance()->Prs.Part->SetInteractionMode(visu_prs_manager::InteractionMode_3D);

  // Widgets and layouts
  gui_control_pane* pControlPane = new gui_control_pane(this);
  QVTKWidget*       pViewer      = common_facilities::Instance()->Prs.Part->GetQVTKWidget();
  QHBoxLayout*      pBaseLayout  = new QHBoxLayout();

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
  m_interactorStyleDefault->SetRenderer( common_facilities::Instance()->Prs.Part->GetRenderer() );

  // Initialize Callback instance for Pick operation
  m_pickCallback = vtkSmartPointer<visu_pick_callback>::New();
  m_pickCallback->SetViewer(this);

  // Set observer for detection
  if ( !m_interactorStyleDefault->HasObserver(EVENT_PICK_DEFAULT) )
    m_interactorStyleDefault->AddObserver(EVENT_PICK_DEFAULT, m_pickCallback);

  // Set observer for detection
  if ( !m_interactorStyleDefault->HasObserver(EVENT_DETECT_DEFAULT) )
    m_interactorStyleDefault->AddObserver(EVENT_DETECT_DEFAULT, m_pickCallback);

  // Get notified once a sub-shape is picked
  connect( m_pickCallback, SIGNAL( subShapesPicked() ), this, SLOT( onSubShapesPicked() ) );

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
  common_facilities::Instance()->Prs.Part->GetQVTKWidget()
                                         ->GetRenderWindow()
                                         ->GetInteractor()
                                         ->SetInteractorStyle(m_interactorStyleDefault);

  /* ========================
   *  Initialize axes widget
   * ======================== */

  vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
  vtkSmartPointer<vtkAssembly> assm = vtkSmartPointer<vtkAssembly>::New();
  assm->AddPart(axes);

  m_axesWidget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
  m_axesWidget->SetOrientationMarker(assm);

  vtkRenderer* renderer = common_facilities::Instance()->Prs.Part->GetRenderer();
  renderer->SetRenderWindow( common_facilities::Instance()->Prs.Part->GetQVTKWidget()
                                                                    ->GetRenderWindow() );
  m_axesWidget->SetCurrentRenderer( common_facilities::Instance()->Prs.Part->GetRenderer() );
  m_axesWidget->SetInteractor( common_facilities::Instance()->Prs.Part->GetQVTKWidget()
                                                                      ->GetRenderWindow()
                                                                      ->GetInteractor() );

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

//! Updates viewer.
void gui_viewer_part::Repaint()
{
  common_facilities::Instance()->Prs.Part->GetQVTKWidget()->repaint();
}

//! Resets view.
void gui_viewer_part::onResetView()
{
  visu_utils::ResetCamera( common_facilities::Instance()->Prs.Part->GetRenderer(),
                           common_facilities::Instance()->Prs.Part->PropsByTrihedron() );
  this->Repaint();
}

//! Callback for picking event.
void gui_viewer_part::onSubShapesPicked()
{
  Handle(geom_node) geom_n = common_facilities::Instance()->Model->GeometryNode();
  if ( geom_n.IsNull() || !geom_n->IsWellFormed() )
  {
    std::cout << "Geometry Node is not accessible" << std::endl;
    return; // No target Node to proceed with
  }

  //---------------------------------------------------------------------------
  // Retrieve current selection
  //---------------------------------------------------------------------------

  // Access picking results
  const visu_actual_selection& sel      = common_facilities::Instance()->Prs.Part->GetCurrentSelection();
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
