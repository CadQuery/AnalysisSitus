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

#ifndef QDS_CHECKBOX_H
#define QDS_CHECKBOX_H

#include "QDS_Datum.h"

class QCheckBox;

class asiUI_EXPORT QDS_CheckBox : public QDS_Datum
{
  Q_OBJECT

public:
  QDS_CheckBox( const QString&, QWidget* = 0, const int = QDS::All, const QString& = QString() );
  virtual ~QDS_CheckBox();

  bool                 isChecked() const;
  void                 setChecked( const bool );

  virtual QString      getString() const;

  virtual void         clear();

signals:
  void                 toggled( bool );

private slots:
  void                 onParamChanged();
  void                 onStateChanged( int );

protected:
  QCheckBox*           checkBox() const;
  virtual QWidget*     createControl( QWidget* );

  virtual void         setString( const QString& );
};

#endif
