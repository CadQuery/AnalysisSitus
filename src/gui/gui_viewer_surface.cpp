//-----------------------------------------------------------------------------
// Created on: 04 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <gui_viewer_surface.h>

// Common includes
#include <common_facilities.h>

// Visualization includes
#include <visu_utils.h>

// Qt-VTK includes
#include <QVTKWidget.h>

// Qt includes
#pragma warning(push, 0)
#include <QVBoxLayout>
#pragma warning(pop)

//! Creates a new instance of viewer.
//! \param parent [in] parent widget.
gui_viewer_surface::gui_viewer_surface(QWidget* parent) : gui_viewer(parent)
{
  // Initialize Presentation Manager along with QVTK widget
  common_facilities::Instance()->Prs.Surface = vtkSmartPointer<visu_prs_manager>::New();
  m_prs_mgr = common_facilities::Instance()->Prs.Surface;
  //
  m_prs_mgr->Initialize(this);
  m_prs_mgr->SetInteractionMode(visu_prs_manager::InteractionMode_3D);

  // Widgets and layouts
  QVTKWidget*  pViewer     = m_prs_mgr->GetQVTKWidget();
  QHBoxLayout* pBaseLayout = new QHBoxLayout();

  // Configure layout
  pBaseLayout->setSpacing(0);
  pBaseLayout->addWidget(pViewer);
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
gui_viewer_surface::~gui_viewer_surface()
{
}

//! Updates viewer.
void gui_viewer_surface::Repaint()
{
  m_prs_mgr->GetQVTKWidget()->repaint();
}

//! Resets view.
void gui_viewer_surface::onResetView()
{
  visu_utils::ResetCamera( m_prs_mgr->GetRenderer(), m_prs_mgr->PropsByTrihedron() );
  this->Repaint();
}
