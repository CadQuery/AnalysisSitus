//-----------------------------------------------------------------------------
// Created on: 27 December 2016
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

// asiUI includes
#include <asiUI_StatusBar.h>

// OCCT includes
#include <OSD_Environment.hxx>

// Qt includes
#pragma warning(push, 0)
#include <QHBoxLayout>
#include <QToolButton>
#include <QPalette>
#include <QProgressBar>
#include <QStatusBar>
#include <QFrame>
#include <QToolButton>
#include <QTimer>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Constructor.
//! \param parent [in] parent widget.
asiUI_StatusBar::asiUI_StatusBar(QWidget* parent) : QStatusBar(parent)
{
  m_pProgressWidget = new QWidget(this);

  m_pMessage          = new QLabel(this);
  m_pProgressBar      = new QProgressBar(m_pProgressWidget);
  m_pInteractiveLabel = new ClickableLabel( this );
  //
  QPalette aPalettLabel = m_pInteractiveLabel->palette();
  aPalettLabel.setColor(QPalette::WindowText, Qt::red);
  //
  m_pInteractiveLabel->setPalette( aPalettLabel );
  m_pInteractiveLabel->setSizePolicy( QSizePolicy(QSizePolicy::Preferred,
                                                  QSizePolicy::Expanding) );

  // Icon
  TCollection_AsciiString ico_fn = OSD_Environment("AS_RESOURCES").Value();
  ico_fn += "/asitus_icon_16x16.png";
  QIcon cancelIcon( ico_fn.ToCString() );

  // Cancel button
  m_pCancelButton = new QToolButton(m_pProgressWidget);
  m_pCancelButton->setIcon(cancelIcon);
  m_pCancelButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
  m_pCancelButton->setSizePolicy( QSizePolicy(QSizePolicy::Preferred,
                                              QSizePolicy::Expanding) );

  // Message placeholder
  m_pMessage->setMargin(0);
  m_pMessage->setContentsMargins(5, 0, 5, 0);
  m_pMessage->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  m_pMessage->setSizePolicy( QSizePolicy(QSizePolicy::Expanding,
                                         QSizePolicy::Expanding) );

  // Progress bar
  m_pProgressBar->setRange(0, 0);
  m_pProgressBar->setTextVisible(false);
  m_pProgressBar->setAlignment(Qt::AlignHCenter);
  m_pProgressBar->setMinimumWidth(200);

  // Configure layout
  QHBoxLayout* lay = new QHBoxLayout();
  lay->setMargin(2);
  lay->setSpacing(3);
  lay->addWidget(m_pProgressBar, 10, Qt::AlignVCenter);
  lay->addWidget(m_pCancelButton, 0, Qt::AlignVCenter);
  //
  m_pProgressWidget->setLayout(lay);

  this->addPermanentWidget(m_pMessage);
  this->addPermanentWidget(m_pProgressWidget);
  this->addPermanentWidget(m_pInteractiveLabel);

  // Adjust visibility
  m_pMessage          -> setVisible(false);
  m_pProgressWidget   -> setVisible(false);
  m_pInteractiveLabel -> setVisible(false);

  // Connect signals to slots
  connect( m_pCancelButton,     SIGNAL( clicked() ), this, SIGNAL( CancelClicked() ) );
  connect( m_pInteractiveLabel, SIGNAL( clicked() ), this, SIGNAL( InfoClicked()   ) );
}

//-----------------------------------------------------------------------------

//! Shows the specified text in the status bar.
//! \param statusText [in] text to show as a status hint.
void asiUI_StatusBar::SetText(const QString& statusText)
{
  m_pMessage->setText(statusText);
}

//-----------------------------------------------------------------------------

//! Shows or hides the progress indicator.
//! \param isOn [in] true/false.
void asiUI_StatusBar::ShowProgress(const bool isOn)
{
  m_pMessage->setVisible(isOn);
  m_pProgressWidget->setVisible(isOn);
}

//-----------------------------------------------------------------------------

//! Updates the progress indicator using specified percentage value.
//! \param percentage [in] percentage value to set.
void asiUI_StatusBar::UpdateProgress(const int percentage)
{
  m_pProgressBar->setValue(percentage);
}

//-----------------------------------------------------------------------------

//! Enables/disables cancellation button.
//! \param isEnabled [in] true/false.
void asiUI_StatusBar::EnableCancelButton(const bool isEnabled)
{
  m_pCancelButton->setEnabled(isEnabled);
}

//-----------------------------------------------------------------------------

//! Switches on/off infinite state for the progress bar.
//! \param isInfinite [in] true/false.
void asiUI_StatusBar::SetProgressInfinite(const bool isInfinite)
{
  if ( isInfinite )
    m_pProgressBar->setRange(0, 0);
  else
    m_pProgressBar->setRange(0, 100);
}

//-----------------------------------------------------------------------------

//! Shows the passed information message. Pass an empty string to hide
//! the message.
//! \param textMsg [in] text to show.
void asiUI_StatusBar::SetInfoMessage(const QString& textMsg)
{
  m_pInteractiveLabel->setVisible( !textMsg.isEmpty() );
  m_pInteractiveLabel->setText( textMsg );
}

//-----------------------------------------------------------------------------

//! ctor for the interactive label.
//! \param parent [in] parent widget.
asiUI_StatusBar::ClickableLabel::ClickableLabel(QWidget* parent) : QLabel(parent)
{}

//-----------------------------------------------------------------------------

//! Reaction to mouse press event.
//! \param evt [in] event to react on.
void asiUI_StatusBar::ClickableLabel::mousePressEvent(QMouseEvent* evt)
{
  Q_UNUSED(evt)
  emit clicked();
}
