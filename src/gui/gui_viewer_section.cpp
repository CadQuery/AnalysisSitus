//-----------------------------------------------------------------------------
// Created on: 09 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <gui_viewer_section.h>

// Common includes
#include <common_facilities.h>

// Visualization includes
#include <visu_utils.h>

// GUI includes
#include <gui_controls_section.h>

// Qt-VTK includes
#include <QVTKWidget.h>

// Qt includes
#pragma warning(push, 0)
#include <QVBoxLayout>
#pragma warning(pop)

//! Creates a new instance of viewer.
//! \param parent [in] parent widget.
gui_viewer_section::gui_viewer_section(QWidget* parent) : gui_viewer(parent)
{
  // Initialize Presentation Manager along with QVTK widget
  common_facilities::Instance()->Prs.Section = vtkSmartPointer<visu_prs_manager>::New();
  m_prs_mgr = common_facilities::Instance()->Prs.Section;
  //
  m_prs_mgr->Initialize(this);
  m_prs_mgr->SetInteractionMode(visu_prs_manager::InteractionMode_2D);

  // Widgets and layouts
  gui_controls_section* pControlPane = new gui_controls_section(this);
  QVTKWidget*           pViewer      = m_prs_mgr->GetQVTKWidget();
  QVBoxLayout*          pBaseLayout  = new QVBoxLayout();

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

  /* =====================================
   *  Finalize initial state of the scene
   * ===================================== */

  this->onResetView();
}

//! Destructor.
gui_viewer_section::~gui_viewer_section()
{
}

//! Updates viewer.
void gui_viewer_section::Repaint()
{
  m_prs_mgr->GetQVTKWidget()->repaint();
}

//! Resets view.
void gui_viewer_section::onResetView()
{
  visu_utils::CameraOnTop( m_prs_mgr->GetRenderer() );
  this->Repaint();
}
