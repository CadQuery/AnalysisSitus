//-----------------------------------------------------------------------------
// Created on: 04 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of Sergey Slyadnev nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_ViewerHost.h>

// asiVisu includes
#include <asiVisu_Utils.h>

// asiEngine includes
#include <asiEngine_Model.h>

// Qt-VTK includes
#include <QVTKWidget.h>

// Qt includes
#pragma warning(push, 0)
#include <QDesktopWidget>
#include <QVBoxLayout>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Creates a new instance of viewer.
//! \param model  [in] Data Model instance.
//! \param parent [in] parent widget.
asiUI_ViewerHost::asiUI_ViewerHost(const Handle(asiEngine_Model)& model,
                                   QWidget*                       parent) : asiUI_Viewer(parent)
{
  // Initialize presentation manager along with QVTK widget
  m_prs_mgr = vtkSmartPointer<asiVisu_PrsManager>::New();
  //
  m_prs_mgr->SetModel(model);
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

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_ViewerHost::~asiUI_ViewerHost()
{
}

//-----------------------------------------------------------------------------

//! \return size hint.
QSize asiUI_ViewerHost::sizeHint() const
{
  QDesktopWidget desktop;
  const int side   = std::min( desktop.height(), desktop.width() );
  const int width  = (int) (side*0.25);
  const int height = (int) (side*0.25);

  QSize s(width, height);
  return s;
}

//-----------------------------------------------------------------------------

//! Updates viewer.
void asiUI_ViewerHost::Repaint()
{
  m_prs_mgr->GetQVTKWidget()->repaint();
}

//-----------------------------------------------------------------------------

//! Resets view.
void asiUI_ViewerHost::onResetView()
{
  asiVisu_Utils::ResetCamera( m_prs_mgr->GetRenderer(), m_prs_mgr->PropsByTrihedron() );
  this->Repaint();
}
