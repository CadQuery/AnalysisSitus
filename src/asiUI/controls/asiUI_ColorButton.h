//-----------------------------------------------------------------------------
// Created on: 20 September 2017
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
//    * Neither the name of the copyright holder(s) nor the
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
// Implementation is inspired by QtxColorButton class from
// SALOME platform (see http://www.salome-platform.org/).
//-----------------------------------------------------------------------------

#ifndef asiUI_ColorButton_h
#define asiUI_ColorButton_h

// asiUI includes
#include <asiUI.h>

// Qt includes
#pragma warning(push, 0)
#include <QMap>
#include <QColor>
#include <QPixmap>
#include <QToolButton>
#pragma warning(pop)

class QPaintDevice;

//! Color picking button.
class asiUI_EXPORT asiUI_ColorButton : public QToolButton
{
  Q_OBJECT

public:

  asiUI_ColorButton(QWidget* = 0);
  virtual ~asiUI_ColorButton();

public:

  //! \return current color.
  QColor
    color() const;

  //! Sets current color.
  //! \param[in] color color to set as current.
  void
    setColor(const QColor& color);

  //! Filters events for the child widgets.
  bool
    eventFilter(QObject*, QEvent*);

signals:

  void clicked(QColor);
  void changed(QColor);

private slots:

  //! Called when the popup menu is about to show.
  void
    onAboutToShow();

  //! Called when the button is clicked by the user.
  void
    onClicked(bool);

  //! Called when any color selection button from popup menu is clicked.
  void
    onToggled(bool);

  //! Called the "Other colors" child button from popup menu is clicked.
  //! Invokes standard "Select color" dialog box allowing user to select
  //! custom color. If the current color is changed by the user, emits
  //! the signal changed().
  void
    onDialogClicked(bool);

protected:

  //! Customizes paint event for the widget.
  virtual void
    paintEvent(QPaintEvent*);

private:

  //! Updates widget's state.
  void
    updateState();

  //! Updates child button state.
  void
    updateButton(QToolButton*);

  //! Generates (if necessary) or gets the icon for the button.
  QPixmap
    buttonIcon(const QColor&) const;

  //! Draws pixmap for color rect.
  void
    drawColor(QPaintDevice*, const QColor&, const int = 1) const;

  //! Returns the predefined list of colors to be used in the popup menu.
  QList<QColor>
    colorsList() const;

private:

  typedef QMap<const QToolButton*, QColor> ColorMap;

private:

  ColorMap m_colors;

};

#endif
