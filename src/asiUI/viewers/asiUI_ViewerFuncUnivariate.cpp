//-----------------------------------------------------------------------------
// Created on: 04 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_ViewerFuncUnivariate.h>

// Visualization includes
#include <asiVisu_Utils.h>

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
//! \param parent [in] parent widget.
asiUI_ViewerFuncUnivariate::asiUI_ViewerFuncUnivariate(QWidget* parent) : asiUI_Viewer(parent)
{
  // Initialize presentation manager along with QVTK widget
  m_prs_mgr = vtkSmartPointer<asiVisu_PrsManager>::New();
  //
  m_prs_mgr->Initialize(this);
  m_prs_mgr->SetInteractionMode(asiVisu_PrsManager::InteractionMode_2D);
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
  asiVisu_Utils::InitTextWidget(m_textWidget);
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
asiUI_ViewerFuncUnivariate::~asiUI_ViewerFuncUnivariate()
{
}

//-----------------------------------------------------------------------------

//! Updates viewer.
void asiUI_ViewerFuncUnivariate::Repaint()
{
  m_prs_mgr->GetQVTKWidget()->repaint();
}

//-----------------------------------------------------------------------------

//! Resets view.
void asiUI_ViewerFuncUnivariate::onResetView()
{
  asiVisu_Utils::CameraOnTop( m_prs_mgr->GetRenderer() );
  this->Repaint();
}
