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
  common_facilities::Instance()->Prs.Surface->Initialize(this);
  common_facilities::Instance()->Prs.Surface->SetInteractionMode(visu_prs_manager::InteractionMode_3D);

  // Widgets and layouts
  QWidget*     pBasePane   = new QWidget(this);
  QVTKWidget*  pViewer     = common_facilities::Instance()->Prs.Surface->GetQVTKWidget();
  QHBoxLayout* pBaseLayout = new QHBoxLayout();

  // Configure layout
  pBaseLayout->setSpacing(0);
  pBaseLayout->addWidget(pViewer);
  pBaseLayout->setAlignment(Qt::AlignTop);
  pBasePane->setLayout(pBaseLayout);
  pBaseLayout->setContentsMargins(0, 0, 0, 0);

  // Set central widget
  this->setCentralWidget(pBasePane);

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
  common_facilities::Instance()->Prs.Surface->GetQVTKWidget()->repaint();
}

//! Resets view.
void gui_viewer_surface::onResetView()
{
  visu_utils::ResetCamera( common_facilities::Instance()->Prs.Surface->GetRenderer(),
                           common_facilities::Instance()->Prs.Surface->PropsByTrihedron() );
  this->Repaint();
}
