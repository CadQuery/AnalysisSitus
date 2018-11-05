//-----------------------------------------------------------------------------
// Created on: 23 June 2017
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
#include <asiUI_DialogOCAFDump.h>

// Active Data includes
#include <ActData_CAFDumper.h>

// Qt includes
#include <QGroupBox>
#include <QLabel>
#include <QSizePolicy>

//-----------------------------------------------------------------------------

#define BTN_MIN_WIDTH 120

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param model    [in] Data Model instance.
//! \param progress [in] progress notifier.
//! \param parent   [in] parent widget.
asiUI_DialogOCAFDump::asiUI_DialogOCAFDump(const Handle(ActAPI_IModel)& model,
                                           ActAPI_ProgressEntry         progress,
                                           QWidget*                     parent)
: QDialog    (parent),
  m_model    (model),
  m_notifier (progress)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Group box for parameters
  QGroupBox* pGroup = new QGroupBox("Contents of current project");

  // Editors
  m_widgets.pEditor = new asiUI_StyledTextEdit();
  //
  QVBoxLayout* boxLayout = new QVBoxLayout(pGroup);
  boxLayout->addWidget(m_widgets.pEditor);

  //---------------------------------------------------------------------------
  // Buttons
  //---------------------------------------------------------------------------

  m_widgets.pClose = new QPushButton("Close");

  // Sizing
  m_widgets.pClose->setMaximumWidth(BTN_MIN_WIDTH);

  // Reaction
  connect( m_widgets.pClose, SIGNAL( clicked() ), this, SLOT( onClose() ) );

  //---------------------------------------------------------------------------
  // Layout
  //---------------------------------------------------------------------------

  // Configure main layout
  m_pMainLayout->addWidget(pGroup);
  m_pMainLayout->addWidget(m_widgets.pClose);
  m_pMainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  m_pMainLayout->setContentsMargins(10, 10, 10, 10);

  this->setLayout(m_pMainLayout);
  this->setWindowModality(Qt::WindowModal);
  this->setWindowTitle("Project dump");

  // Set good initial size
  this->setMinimumSize( QSize(900, 600) );

  //---------------------------------------------------------------------------
  // Fill editor with data
  //---------------------------------------------------------------------------

  this->initialize();
}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_DialogOCAFDump::~asiUI_DialogOCAFDump()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//-----------------------------------------------------------------------------
// Initialization
//-----------------------------------------------------------------------------

//! Fills editor with OCAF dump.
void asiUI_DialogOCAFDump::initialize()
{
  std::ostringstream buff;

  if ( !ActData_CAFDumper::Dump(buff, m_model) )
    m_widgets.pEditor->setText("### FAILED TO DUMP");
  else
    m_widgets.pEditor->setText( buff.str().c_str() );
}

//-----------------------------------------------------------------------------
// Slots
//-----------------------------------------------------------------------------

//! Reaction on close.
void asiUI_DialogOCAFDump::onClose()
{
  this->close();
}
