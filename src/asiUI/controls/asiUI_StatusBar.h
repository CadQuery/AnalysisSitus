//-----------------------------------------------------------------------------
// Created on: 27 December 2016
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
