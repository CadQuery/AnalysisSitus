//-----------------------------------------------------------------------------
// Created on: 20 September 2017
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
