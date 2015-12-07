//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <gui_viewer_domain.h>

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
gui_viewer_domain::gui_viewer_domain(QWidget* parent) : gui_viewer(parent)
{
  // Initialize Presentation Manager along with QVTK widget
  common_facilities::Instance()->Prs.Domain = vtkSmartPointer<visu_prs_manager>::New();
  common_facilities::Instance()->Prs.Domain->Initialize(this);
  common_facilities::Instance()->Prs.Domain->SetInteractionMode(visu_prs_manager::InteractionMode_2D);

  // Widgets and layouts
  QVTKWidget*  pViewer     = common_facilities::Instance()->Prs.Domain->GetQVTKWidget();
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
gui_viewer_domain::~gui_viewer_domain()
{
}

//! Updates viewer.
void gui_viewer_domain::Repaint()
{
  common_facilities::Instance()->Prs.Domain->GetQVTKWidget()->repaint();
}

//! Resets view.
void gui_viewer_domain::onResetView()
{
  visu_utils::CameraOnTop( common_facilities::Instance()->Prs.Domain->GetRenderer() );
  this->Repaint();
}
