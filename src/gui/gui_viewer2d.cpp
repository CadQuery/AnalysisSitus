//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <gui_viewer2d.h>

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
gui_viewer2d::gui_viewer2d(QWidget* parent) : QMainWindow(parent)
{
  // Initialize Presentation Manager along with QVTK widget
  common_facilities::Instance()->PrsManager2d = vtkSmartPointer<visu_prs_manager>::New();
  common_facilities::Instance()->PrsManager2d->Initialize(this);

  // Widgets and layouts
  QWidget*     pBasePane   = new QWidget(this);
  QVTKWidget*  pViewer     = common_facilities::Instance()->PrsManager2d->GetQVTKWidget();
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
gui_viewer2d::~gui_viewer2d()
{
}

//! Updates View Window.
void gui_viewer2d::Repaint()
{
  common_facilities::Instance()->PrsManager->GetQVTKWidget()->repaint();
}

//! Callback for "Reset View" action.
void gui_viewer2d::onResetView()
{
  visu_utils::CameraOnTop( common_facilities::Instance()->PrsManager2d->GetRenderer() );
  this->Repaint();
}
