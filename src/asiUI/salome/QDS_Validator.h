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

#ifndef QDS_VALIDATOR_H
#define QDS_VALIDATOR_H

#include "QDS.h"

#pragma warning(push, 0)
#include <QValidator>
#pragma warning(pop)

class asiUI_EXPORT QDS_IntegerValidator : public QIntValidator
{
public:
  QDS_IntegerValidator( QObject* p = 0 );
  QDS_IntegerValidator( const QString& f, QObject* p = 0 );
  virtual ~QDS_IntegerValidator();

  virtual State validate( QString&, int& ) const;

private:
  QString myFilter;    //!< string filter
};

class asiUI_EXPORT QDS_DoubleValidator : public QDoubleValidator
{
public:
  QDS_DoubleValidator( QObject* p = 0 );
  QDS_DoubleValidator( const QString& f, QObject* p = 0 );
  virtual ~QDS_DoubleValidator();

  virtual State validate( QString&, int& ) const;

private:
  QString myFilter;    //!< string filter
};

class asiUI_EXPORT QDS_StringValidator : public QValidator
{
public:
  QDS_StringValidator( QObject* p = 0 );
  QDS_StringValidator( const QString& f, QObject* p = 0 );
  QDS_StringValidator( const QString& ft, const QString& fg, QObject* p = 0 );
  virtual ~QDS_StringValidator();

  virtual State validate( QString&, int& ) const;

  int           length() const;
  void          setLength( const int );

private:
  int           myLen;        //!< string length
  QString       myFlags;      //!< string format flags
  QString       myFilter;     //!< string filter
};

#endif
