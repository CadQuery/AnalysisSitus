//-----------------------------------------------------------------------------
// Created on: 04 February 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_ViewerCurve2d.h>

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
//! \param prs_mgr [in/out] externally defined Presentation Manager.
//! \param parent  [in]     parent widget.
asiUI_ViewerCurve2d::asiUI_ViewerCurve2d(vtkSmartPointer<asiVisu_PrsManager>& prs_mgr,
                                         QWidget*                             parent) : asiUI_Viewer(parent)
{
  // Initialize presentation manager along with QVTK widget
  prs_mgr   = vtkSmartPointer<asiVisu_PrsManager>::New();
  m_prs_mgr = prs_mgr;
  //
  m_prs_mgr->Initialize(this);
  m_prs_mgr->SetInteractionMode(asiVisu_PrsManager::InteractionMode_2D);
  m_prs_mgr->SetSelectionMode(SelectionMode_Workpiece);

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
asiUI_ViewerCurve2d::~asiUI_ViewerCurve2d()
{
}

//-----------------------------------------------------------------------------

//! Updates viewer.
void asiUI_ViewerCurve2d::Repaint()
{
  m_prs_mgr->GetQVTKWidget()->repaint();
}

//-----------------------------------------------------------------------------

//! Resets view.
void asiUI_ViewerCurve2d::onResetView()
{
  asiVisu_Utils::CameraOnTop( m_prs_mgr->GetRenderer() );
  this->Repaint();
}
