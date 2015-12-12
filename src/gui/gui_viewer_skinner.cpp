//-----------------------------------------------------------------------------
// Created on: 08 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <gui_viewer_skinner.h>

// GUI includes
#include <gui_controls_skinning.h>

// Common includes
#include <common_facilities.h>

// Visualization includes
#include <visu_node_info.h>
#include <visu_utils.h>

// VTK includes
#include <vtkAssembly.h>
#include <vtkRenderWindow.h>

// Qt-VTK includes
#include <QVTKWidget.h>

// Qt includes
#pragma warning(push, 0)
#include <QVBoxLayout>
#pragma warning(pop)

//! Creates a new instance of viewer.
//! \param parent [in] parent widget.
gui_viewer_skinner::gui_viewer_skinner(QWidget* parent) : gui_viewer(parent)
{
  // Initialize Presentation Manager along with QVTK widget
  common_facilities::Instance()->Prs.Skinner = vtkSmartPointer<visu_prs_manager>::New();
  m_prs_mgr = common_facilities::Instance()->Prs.Skinner;
  //
  m_prs_mgr->Initialize(this);
  m_prs_mgr->SetInteractionMode(visu_prs_manager::InteractionMode_3D);

  // Widgets and layouts
  gui_controls_skinning* pControlPane = new gui_controls_skinning(this);
  QVTKWidget*            pViewer      = m_prs_mgr->GetQVTKWidget();
  QVBoxLayout*           pBaseLayout  = new QVBoxLayout();

  // Configure layout
  pBaseLayout->setSpacing(0);
  pBaseLayout->addWidget(pViewer);
  pBaseLayout->addWidget(pControlPane);
  pBaseLayout->setStretchFactor(pViewer, 1);
  pBaseLayout->setStretchFactor(pControlPane, 0);
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

  // Set observer for detection
  if ( !m_interactorStyleDefault->HasObserver(EVENT_PICK_DEFAULT) )
    m_interactorStyleDefault->AddObserver(EVENT_PICK_DEFAULT, m_pickCallback);

  // Set observer for detection
  if ( !m_interactorStyleDefault->HasObserver(EVENT_DETECT_DEFAULT) )
    m_interactorStyleDefault->AddObserver(EVENT_DETECT_DEFAULT, m_pickCallback);

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

//! Destructor.
gui_viewer_skinner::~gui_viewer_skinner()
{
}

//! Updates viewer.
void gui_viewer_skinner::Repaint()
{
  m_prs_mgr->GetQVTKWidget()->repaint();
}

//! Resets view.
void gui_viewer_skinner::onResetView()
{
  visu_utils::ResetCamera( m_prs_mgr->GetRenderer(), m_prs_mgr->PropsByTrihedron() );
  this->Repaint();
}
