//-----------------------------------------------------------------------------
// Created on: 04 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_viewer_func_univariate.h>

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

// VTK includes
#include <vtkProperty.h>
#include <vtkTextActor.h>
#include <vtkTextRepresentation.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

//! Creates a new instance of viewer.
//! \param prs_mgr [in/out] externally defined Presentation Manager.
//! \param parent  [in]     parent widget.
gui_viewer_func_univariate::gui_viewer_func_univariate(vtkSmartPointer<visu_prs_manager>& prs_mgr,
                                                       QWidget*                           parent) : gui_viewer(parent)
{
  // Initialize Presentation Manager along with QVTK widget
  prs_mgr   = vtkSmartPointer<visu_prs_manager>::New();
  m_prs_mgr = prs_mgr;
  //
  m_prs_mgr->Initialize(this);
  m_prs_mgr->SetInteractionMode(visu_prs_manager::InteractionMode_2D);
  m_prs_mgr->SetSelectionMode(SelectionMode_Workpiece);
  m_prs_mgr->GetRenderer()->SetGradientBackground(true);
  m_prs_mgr->GetRenderer()->SetBackground(0.15, 0.15, 0.15);
  m_prs_mgr->GetRenderer()->SetBackground2(0.1, 0.1, 0.1);

  // Widgets and layouts
  QVTKWidget*  pViewer     = m_prs_mgr->GetQVTKWidget();
  QVBoxLayout* pBaseLayout = new QVBoxLayout();

  // Configure layout
  pBaseLayout->setSpacing(0);
  pBaseLayout->addWidget(pViewer);
  pBaseLayout->setContentsMargins(0, 0, 0, 0);

  // Set central widget
  this->setLayout(pBaseLayout);

  /* =====================================
   *  Finalize initial state of the scene
   * ===================================== */

  // Initialize text widget used for annotations
  m_textWidget = vtkSmartPointer<vtkTextWidget>::New();
  visu_utils::InitTextWidget(m_textWidget);
  //
  vtkTextRepresentation* rep = vtkTextRepresentation::SafeDownCast( m_textWidget->GetRepresentation() );
  rep->SetWindowLocation(vtkTextRepresentation::UpperRightCorner);
  //
  m_textWidget->SetInteractor      ( m_prs_mgr->GetRenderer()->GetRenderWindow()->GetInteractor() );
  m_textWidget->SetDefaultRenderer ( m_prs_mgr->GetRenderer() );
  m_textWidget->SetCurrentRenderer ( m_prs_mgr->GetRenderer() );

  // Reset camera
  this->onResetView();
}

//! Destructor.
gui_viewer_func_univariate::~gui_viewer_func_univariate()
{
}

//-----------------------------------------------------------------------------

//! Updates viewer.
void gui_viewer_func_univariate::Repaint()
{
  m_prs_mgr->GetQVTKWidget()->repaint();
}

//-----------------------------------------------------------------------------

//! Resets view.
void gui_viewer_func_univariate::onResetView()
{
  visu_utils::CameraOnTop( m_prs_mgr->GetRenderer() );
  this->Repaint();
}
