//-----------------------------------------------------------------------------
// Created on: 27 December 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
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
