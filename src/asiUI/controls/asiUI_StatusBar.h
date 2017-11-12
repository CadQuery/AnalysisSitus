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

#ifndef asiUI_StatusBar_h
#define asiUI_StatusBar_h

// asiUI includes
#include <asiUI_Common.h>

// OCC includes
#include <TCollection_AsciiString.hxx>

// Qt includes
#pragma warning(push, 0)
#include <QLabel>
#include <QProgressBar>
#include <QStatusBar>
#include <QToolButton>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Status bar widget.
class asiUI_StatusBar : public QStatusBar
{
  Q_OBJECT

public:

  class ClickableLabel;

public:

  asiUI_EXPORT
    asiUI_StatusBar(QWidget* parent = 0);

public:

  asiUI_EXPORT
    void SetText(const QString& statusText);

  asiUI_EXPORT
    void ShowProgress(const bool isShown);

  asiUI_EXPORT
    void UpdateProgress(const int percentage);

  asiUI_EXPORT
    void EnableCancelButton(const bool isEnabled);

  asiUI_EXPORT
    void SetProgressInfinite(const bool isInfinite);

  asiUI_EXPORT
    void SetInfoMessage(const QString& textMsg);

signals:

  //! The signal is emitted when user clicks "cancel" button.
  void CancelClicked();

  //! The signal is emitted when user clicks information message.
  void InfoClicked();

private:

  int counter;

  //! "Cancel" button
  QToolButton* m_pCancelButton;

  //! Text for the progress widget.
  QLabel* m_pMessage;

  //! Progress bar.
  QProgressBar* m_pProgressBar;

  //! Progress widget.
  QWidget* m_pProgressWidget;

  //! Clickable label.
  ClickableLabel* m_pInteractiveLabel;

};

//-----------------------------------------------------------------------------

//! Interactive label.
class asiUI_StatusBar::ClickableLabel : public QLabel
{
  Q_OBJECT

public:

  ClickableLabel(QWidget* parent = 0);

Q_SIGNALS:

  void clicked();

protected:

  void mousePressEvent(QMouseEvent* evt);

};

#endif
