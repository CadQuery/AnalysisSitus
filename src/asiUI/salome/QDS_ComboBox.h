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

#ifndef QDS_COMBOBOX_H
#define QDS_COMBOBOX_H

#include "QDS_Datum.h"

#pragma warning(push, 0)
#include <QMap>
#include <QPixmap>
#include <QStringList>
#pragma warning(pop)

#include <Qtx.h>

class QtxComboBox;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class asiUI_EXPORT QDS_ComboBox : public QDS_Datum
{
  Q_OBJECT

public:
  QDS_ComboBox( const QString&, QWidget* = 0, const int = QDS::All, const QString& = QString() );
  virtual ~QDS_ComboBox();

  bool                       editable() const;
  void                       setEditable( const bool );

  int                        count( bool = false ) const;
  void                       values( QList<int>&, bool = false ) const;

  virtual int                integerValue() const;
  virtual double             doubleValue() const;
  virtual void               setIntegerValue( const int );
  virtual void               setDoubleValue( const double );

  bool                       state( const int ) const;
  void                       setState( const bool, const int, const bool = true );
  void                       setState( const bool, const QList<int>&, const bool = true );
  void                       setValues( const QList<int>&, const QStringList& );
  void                       setValues( const QStringList& );

  virtual QString            getString() const;

  virtual void               reset();

  int                        stringToValue( const QString& ) const;
  QString                    valueToString( const int ) const;

signals:
  void                       activated( int );
  void                       activated( const QString& );

protected slots:
  virtual void               onActivated( int );
  virtual void               onTextChanged( const QString& );

protected:
  QtxComboBox*               comboBox() const;
  virtual QWidget*           createControl( QWidget* );

  virtual void               setString( const QString& );

  virtual void               unitSystemChanged( const QString& );

private:
  int                        getId( const int ) const;
  int                        getId( const QString& ) const;
  int                        getIndex( const int ) const;
  int                        getIndex( const QString& ) const;

  void                       updateComboBox();

private:
  typedef QMap<int, QPixmap> IdIconsMap;
  typedef QMap<int, QString> IdValueMap;
  typedef QMap<int, bool>    IdStateMap;
  typedef QMap<int, int>     IdIndexMap;

private:
  IdValueMap                 myValue;
  IdStateMap                 myState;
  IdIndexMap                 myIndex;
  IdIconsMap                 myIcons;

  QList<int>                 myDataIds;
  QList<int>                 myUserIds;
  QStringList                myUserNames;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
