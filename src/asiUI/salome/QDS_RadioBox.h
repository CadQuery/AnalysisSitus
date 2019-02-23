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

#ifndef QDS_RADIOBOX_H
#define QDS_RADIOBOX_H

#include "QDS_Datum.h"

#include <Qtx.h>

#pragma warning(push, 0)
#include <QMap>
#include <QStringList>
#pragma warning(pop)

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QButtonGroup;
class QGroupBox;
class QRadioButton;

class asiUI_EXPORT QDS_RadioBox : public QDS_Datum
{
  Q_OBJECT

public:
  QDS_RadioBox( const QString&, QWidget* = 0, const int = QDS::Control, const QString& = QString() );
  virtual ~QDS_RadioBox();

  int                        count( bool = false ) const;
  void                       values( QList<int>&, bool = false ) const;

  int                        columns() const;
  void                       setColumns( const int );

  bool                       state( const int ) const;
  void                       setState( const bool, const int, const bool = true );
  void                       setState( const bool, const QList<int>&, const bool = true );
  void                       setValues( const QList<int>&, const QStringList& );
  void                       setValues( const QStringList& );

  virtual QString            getString() const;

signals:
  void                       activated( int );

protected slots:
  virtual void               onToggled( bool );

protected:
  QButtonGroup*              buttonGroup() const;
  QGroupBox*                 groupBox() const;
  virtual QWidget*           createControl( QWidget* );
  void                       buttons( QList<QRadioButton*>& ) const;

  virtual void               setString( const QString& );

  virtual void               unitSystemChanged( const QString& );

private:
  void                       updateRadioBox();

private:
  typedef QMap<int, QString> IdValueMap;
  typedef QMap<int, bool>    IdStateMap;

private:
  IdValueMap                 myValue;
  IdStateMap                 myState;

  QIntList                   myDataIds;
  QIntList                   myUserIds;
  QStringList                myUserNames;

  QButtonGroup*              myButtonGroup;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
