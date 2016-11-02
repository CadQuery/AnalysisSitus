//-----------------------------------------------------------------------------
// Created on: 04 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_ViewerSurface.h>

// Visualization includes
#include <asiVisu_Utils.h>

// Qt-VTK includes
#include <QVTKWidget.h>

// Qt includes
#pragma warning(push, 0)
#include <QVBoxLayout>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Creates a new instance of viewer.
//! \param parent [in] parent widget.
asiUI_ViewerSurface::asiUI_ViewerSurface(QWidget* parent) : asiUI_Viewer(parent)
{
  // Initialize presentation manager along with QVTK widget
  m_prs_mgr = vtkSmartPointer<asiVisu_PrsManager>::New();
  //
  m_prs_mgr->Initialize(this);
  m_prs_mgr->SetInteractionMode(asiVisu_PrsManager::InteractionMode_3D);

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
asiUI_ViewerSurface::~asiUI_ViewerSurface()
{
}

//! Updates viewer.
void asiUI_ViewerSurface::Repaint()
{
  m_prs_mgr->GetQVTKWidget()->repaint();
}

//! Resets view.
void asiUI_ViewerSurface::onResetView()
{
  asiVisu_Utils::ResetCamera( m_prs_mgr->GetRenderer(), m_prs_mgr->PropsByTrihedron() );
  this->Repaint();
}
