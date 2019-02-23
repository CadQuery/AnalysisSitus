// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#ifndef QDS_LINEEDIT_H
#define QDS_LINEEDIT_H

#include "QDS_Datum.h"

#pragma warning(push, 0)
#include <QLineEdit>
#pragma warning(pop)

class asiUI_EXPORT QDS_LineEdit : public QDS_Datum
{
  Q_OBJECT

  Q_PROPERTY( bool Selection READ hasSelection WRITE setSelection )

public:
  class Editor;

public:
  QDS_LineEdit( const QString&, QWidget* = 0, const int = QDS::All, const QString& = QString() );
  virtual ~QDS_LineEdit();

  virtual void         deselect();
  virtual void         selectAll();
  virtual void         setAlignment( const int, const int = QDS::Label );

  bool                 hasSelection() const;
  void                 setSelection( const bool );

  virtual QString      getString() const;

signals:
  void                 returnPressed();

protected slots:
  virtual void         onParamChanged();

private slots:
  void                 onTextChanged( const QString& );

protected:
  virtual QWidget*     createControl( QWidget* );
  QLineEdit*           lineEdit() const;

  virtual void         setString( const QString& );

  virtual void         unitSystemChanged( const QString& );
};

class QDS_LineEdit::Editor : public QLineEdit
{
public:
  Editor( QWidget* parent = 0 ) : QLineEdit( parent ), myNumber( 2 ) {};
  virtual ~Editor() {};

  void setNumber( const int num ) { myNumber = num; };

  virtual QSize minimumSizeHint() const
  {
    return QLineEdit::minimumSizeHint().
      expandedTo( QSize( fontMetrics().width( "0" ) * myNumber, 0 ) );
  }
  
  virtual QSize sizeHint() const
  {
    return minimumSizeHint();
  }

private:
  int           myNumber;
};

#endif
