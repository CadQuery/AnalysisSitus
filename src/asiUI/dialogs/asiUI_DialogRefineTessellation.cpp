//-----------------------------------------------------------------------------
// Created on: 06 April 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_DialogRefineTessellation.h>

// asiUI includes
#include <asiUI_Common.h>

// asiVisu includes
#include <asiVisu_ShapeRobustTessellator.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// asiEngine includes
#include <asiEngine_Part.h>

// OCCT includes
#include <TopExp.hxx>

// Qt includes
#include <QGroupBox>
#include <QLabel>
#include <QSizePolicy>

//-----------------------------------------------------------------------------

#define CONTROL_EDIT_WIDTH    100
#define CONTROL_BTN_WIDTH     100
#define CONTROL_BTN_WIDTH_BIG 150

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param model   [in] Data Model instance.
//! \param prsMgr  [in] presentation manager.
//! \param pViewer [in] connected viewer.
//! \param parent  [in] parent widget.
asiUI_DialogRefineTessellation::asiUI_DialogRefineTessellation(const Handle(asiEngine_Model)&             model,
                                                               const vtkSmartPointer<asiVisu_PrsManager>& prsMgr,
                                                               asiUI_ViewerPart*                          pViewer,
                                                               QWidget*                                   parent)
//
: QDialog   (parent),
  m_model   (model),
  m_prsMgr  (prsMgr),
  m_pViewer (pViewer)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Group box for parameters
  QGroupBox* pGroup = new QGroupBox("Faceter settings");

  // Editors
  m_widgets.pLinDefl = new asiUI_LineEdit();
  m_widgets.pAngDefl = new asiUI_LineEdit();

  // Sizing
  m_widgets.pLinDefl->setMinimumWidth(CONTROL_EDIT_WIDTH);
  m_widgets.pAngDefl->setMinimumWidth(CONTROL_EDIT_WIDTH);

  //---------------------------------------------------------------------------
  // Line editors
  //---------------------------------------------------------------------------

  // Create layout
  QGridLayout* pGrid = new QGridLayout(pGroup);
  pGrid->setSpacing(5);
  //
  pGrid->addWidget(new QLabel("Linear deflection [model units]:"), 0, 0);
  pGrid->addWidget(new QLabel("Angular deflection [degrees]:"),    1, 0);
  //
  pGrid->addWidget(m_widgets.pLinDefl, 0, 1);
  pGrid->addWidget(m_widgets.pAngDefl, 1, 1);
  //
  pGrid->setColumnStretch(0, 0);
  pGrid->setColumnStretch(1, 1);
  pGrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);

  //---------------------------------------------------------------------------
  // Buttons
  //---------------------------------------------------------------------------

  m_widgets.pReset  = new QPushButton("Reset defaults");
  m_widgets.pRefine = new QPushButton("Refine");

  // Sizing
  m_widgets.pReset->setMinimumWidth(CONTROL_BTN_WIDTH_BIG);
  m_widgets.pRefine->setMinimumWidth(CONTROL_BTN_WIDTH);

  // Reaction
  connect( m_widgets.pReset,  SIGNAL( clicked() ), this, SLOT( onReset() ) );
  connect( m_widgets.pRefine, SIGNAL( clicked() ), this, SLOT( onRefine() ) );

  // Create layout
  QFrame* pButtonFrame = new QFrame(this);
  QHBoxLayout* pButtonLayout = new QHBoxLayout(pButtonFrame);
  pButtonLayout->setSpacing(5);
  pButtonLayout->setAlignment(Qt::AlignRight);
  //
  pButtonLayout->addWidget(m_widgets.pRefine);
  pButtonLayout->addWidget(m_widgets.pReset);

  //---------------------------------------------------------------------------
  // Main layout
  //---------------------------------------------------------------------------

  // Configure main layout
  m_pMainLayout->addWidget(pGroup);
  m_pMainLayout->addWidget(pButtonFrame);
  m_pMainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  m_pMainLayout->setContentsMargins(10, 10, 10, 10);

  this->setLayout(m_pMainLayout);
  this->setWindowModality(Qt::WindowModal);
  this->setWindowTitle("Refine tessellation");

  // Adjust initial state
  this->init();
}

//! Destructor.
asiUI_DialogRefineTessellation::~asiUI_DialogRefineTessellation()
{
  delete m_pMainLayout;
}

//-----------------------------------------------------------------------------

//! Reaction on reset.
void asiUI_DialogRefineTessellation::onReset()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Ask tessellator to return its default settings
  const double linDefl = asiAlgo_Utils::AutoSelectLinearDeflection(part);
  const double angDefl = asiAlgo_Utils::AutoSelectAngularDeflection(part);

  // Set data to controls
  m_widgets.pLinDefl->setText( QString::number(linDefl) );
  m_widgets.pAngDefl->setText( QString::number(angDefl) );
}

//-----------------------------------------------------------------------------

//! Reaction on refine.
void asiUI_DialogRefineTessellation::onRefine()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Read user inputs
  const double linDefl = m_widgets.pLinDefl->text().toDouble();
  const double angDefl = m_widgets.pAngDefl->text().toDouble();

  // Modify Data Model
  m_model->OpenCommand();
  {
    part_n->SetLinearDeflection(linDefl);
    part_n->SetAngularDeflection(angDefl);
  }
  m_model->CommitCommand();

  // Actualize
  m_prsMgr->Actualize(part_n);
}

//-----------------------------------------------------------------------------

//! Initializes deflection values from Data Model.
void asiUI_DialogRefineTessellation::init()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Ask tessellator to return its default settings
  const double linDefl = part_n->GetLinearDeflection();
  const double angDefl = part_n->GetAngularDeflection();

  // Set data to controls
  m_widgets.pLinDefl->setText( QString::number(linDefl) );
  m_widgets.pAngDefl->setText( QString::number(angDefl) );
}
