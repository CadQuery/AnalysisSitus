// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// File:      QtxColorButton.h
// Author:    Sergey TELKOV
//
#ifndef QTXCOLORBUTTON_H
#define QTXCOLORBUTTON_H

#pragma warning(disable : 4251 4718)

#include "Qtx.h"

#pragma warning(push, 0)
#include <QMap>
#include <QColor>
#include <QPixmap>
#include <QToolButton>
#pragma warning(pop)

class QPaintDevice;

class asiUI_EXPORT QtxColorButton : public QToolButton
{
  Q_OBJECT

public:
  QtxColorButton( QWidget* = 0 );
  virtual ~QtxColorButton();

  QColor        color() const;
  void          setColor( const QColor& );

  bool          eventFilter( QObject*, QEvent* );

  bool          isAutoEnabled() const;
  void          setAutoEnabled( bool );

  QString       autoText() const;
  void          setAutoText( const QString& );

signals:
  void          clicked( QColor );
  void          changed( QColor );

private slots:
  void          onClicked( bool );
  void          onToggled( bool );

  void          onAboutToShow();
  void          onAutoClicked( bool );
  void          onDialogClicked( bool );

protected:
  virtual void  paintEvent( QPaintEvent* );

private:
  QList<QColor> colorsList() const;

  void          updateState();
  void          updateButton( QToolButton* );
  QPixmap       buttonIcon( const QColor& ) const;
  void          drawColor( QPaintDevice*, const QColor&, const int = 1 ) const;

private:
  typedef QMap<const QToolButton*, QColor> ColorMap;

private:
  ColorMap      myColors;
  QToolButton*  myAutoButton;
};

#pragma warning(default : 4714 4718)

#endif
