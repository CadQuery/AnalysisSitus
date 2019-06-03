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

#include "QDS.h"

#include "QDS_Datum.h"

#pragma warning(push, 0)
#include <QTextCodec>
#pragma warning(pop)

#include <DDS_DicItem.h>
#include <DDS_Dictionary.h>

#include <TCollection_HAsciiString.hxx>
#include <TCollection_HExtendedString.hxx>

QList<QDS_Datum*> QDS::_datumList;

/*!
  \class QDS
  \brief A set of usefull static functions.
*/

/*!
  \brief Convert the OpenCascade ASCII string to Qt string.
  \param src OCC ASCII string
  \return Qt string
*/
QString QDS::toQString( const TCollection_AsciiString& src )
{
  QTextCodec* codec = QTextCodec::codecForLocale();
  QString res;
  if ( !src.IsEmpty() )
    res = codec ? codec->toUnicode( (char*)src.ToCString(), src.Length() ) :
                  QString( (char*)src.ToCString() );
  return res;
}

/*!
  \brief Convert the OpenCascade Unicode string to Qt string.
  \param src OCC Unicode string
  \return Qt string
*/
QString QDS::toQString( const TCollection_ExtendedString& src )
{
  if ( src.IsAscii() )
    return toQString( TCollection_AsciiString( src ) );
  else
    return QString( (QChar*)src.ToExtString(), src.Length() );
}

/*!
  \brief Convert the OpenCascade ASCII string to Qt string.
  \param src handle to OCC ASCII string
  \return Qt string
*/
QString QDS::toQString( const Handle(TCollection_HAsciiString)& src )
{
  if ( src.IsNull() )
    return QString();
  else
    return toQString( src->String() );
}

/*!
  \brief Convert the OpenCascade Unicode string to Qt string.
  \param src handle to OCC Unicode string
  \return Qt string
*/
QString QDS::toQString( const Handle(TCollection_HExtendedString)& src )
{
  if ( src.IsNull() )
    return QString();
  else
    return toQString( src->String() );
}

/*!
  \brief Convert the Qt string to OpenCascade ASCII string.
  \param src Qt string
  \return OCC ASCII string
*/
TCollection_AsciiString QDS::toAsciiString( const QString& src )
{
  TCollection_AsciiString res;
  if ( src.toLatin1().constData() )
  {
    QTextCodec* codec = QTextCodec::codecForLocale();
    if ( codec )
    {
      QByteArray str = codec->fromUnicode( src );
      res = TCollection_AsciiString( (Standard_CString)(const char*)str, str.size() );
    }
    else
      res = TCollection_AsciiString( (char*)src.toLatin1().constData() );
  }
  return res;
}

/*!
  \brief Convert the OpenCascade Unicode string to OpenCascade ASCII string.
  \param src OCC Unicode string
  \return OCC ASCII string
*/
TCollection_AsciiString QDS::toAsciiString( const TCollection_ExtendedString& src )
{
  return TCollection_AsciiString( src );
}

/*!
  \brief Convert the OpenCascade Unicode string to OpenCascade ASCII string.
  \param src handle to OCC Unicode string
  \return OCC ASCII string
*/
TCollection_AsciiString QDS::toAsciiString( const Handle(TCollection_HExtendedString)& src )
{
  TCollection_AsciiString res;
  if ( !src.IsNull() )
    res = toAsciiString( src->String() );
  return res;
}

/*!
  \brief Convert the Qt string to OpenCascade Unicode string.
  \param src Qt string
  \return OCC Unicode string
*/
TCollection_ExtendedString QDS::toExtString( const QString& src )
{
  if ( src.isEmpty() )
    return TCollection_ExtendedString();

  int len = src.length();
  Standard_ExtCharacter* extStr = new Standard_ExtCharacter[( len + 1 ) * 2];
  memcpy( extStr, src.unicode(), len * 2 );
  extStr[len] = 0;

  TCollection_ExtendedString trg( extStr );

  delete [] extStr;

  return trg;
}

/*!
  \brief Convert the OpenCascade ASCII string to OpenCascade Unicode string.
  \param src OCC ASCII string
  \return OCC Unicode string
*/
TCollection_ExtendedString QDS::toExtString( const TCollection_AsciiString& src )
{
  return TCollection_ExtendedString( src );
}

/*!
  \brief Load datum definitions from XML file \a dictPath to the dictionary.
  \return \c true if loading is successed or \c false otherwise.
*/
bool QDS::load( const QString& dictPath )
{
  if ( dictPath.isEmpty() )
    return false;

  return DDS_Dictionary::Load( toAsciiString( dictPath ) );
}

/*!
  \brief Get the label of unit system \a sys.

  If component \a comp is specified and not empty then the function 
  searches the given unit system in the specified component, otherwise 
  all components will be searched.

  \param sys unit system
  \param comp component
  \return unit system lavel or empty string if unit system is not found
*/
QString QDS::unitSystemLabel( const QString& sys, const QString& comp )
{
  QString lab;
  TCollection_AsciiString system = toAsciiString( sys );
  Handle(DDS_Dictionary) dic = DDS_Dictionary::Get();
  if ( !dic.IsNull() )
    lab = toQString( comp.isEmpty() ? dic->GetUnitSystemLabel( system ) :
                                      dic->GetUnitSystemLabel( system, toAsciiString( comp ) ) );
  return lab;
}

/*!
  \brief Get the name of active unit system from the specified component \a comp.

  If component is not specified or empty string, then the first found 
  component will be used.
  
  If component exists, then active unit system name is returned. Otherwise, 
  empty string is returned.
  
  \param comp component
  \return name of the active unit system
*/
QString QDS::activeUnitSystem( const QString& comp )
{
  QString sys;
  Handle(DDS_Dictionary) dic = DDS_Dictionary::Get();
  if ( !dic.IsNull() )
    sys = toQString( comp.isEmpty() ? dic->GetActiveUnitSystem() :
                                      dic->GetActiveUnitSystem( toAsciiString( comp ) ) );
  return sys;
}

/*!
  \brief Set the active unit system.

  If not empty component name \a comp is specified, then the unit system
  will be activated in the given component, otherwise all components 
  will be processed.

  After the changing of active unit system function notifies about it all
  registered datums from processed components using method 
  QDS_Datum::unitSystemChanged()

  \param unit system to be set active
  \param comp component
*/
void QDS::setActiveUnitSystem( const QString& sys, const QString& comp )
{
  Handle(DDS_Dictionary) dic = DDS_Dictionary::Get();
  if ( dic.IsNull() )
    return;

  TCollection_AsciiString system = toAsciiString( sys );
  comp.isEmpty() ? dic->SetActiveUnitSystem( system ) :
                   dic->SetActiveUnitSystem( system, toAsciiString( comp ) );

  QString unitSys = activeUnitSystem( comp );
  if ( sys == unitSys )
    return;

  TCollection_AsciiString aComp = toAsciiString( comp );
  for ( QList<QDS_Datum*>::iterator it = _datumList.begin(); it != _datumList.end(); ++it )
  {
    QDS_Datum* datum = *it;
    if ( !datum )
      continue;

    bool ok = aComp.IsEmpty();
    if ( !ok )
    {
      Handle(DDS_DicItem) item = datum->dicItem();
      ok = !item.IsNull() && aComp == item->GetComponent();
    }

    if ( ok )
      datum->unitSystemChanged( unitSys );
  }
}

/*!
  \brief Register given datum \a datum in the global list.
  
  This function is invoked automatically by QDS_Datum constructor.
  
  \param datum datum being registered
*/
void QDS::insertDatum( QDS_Datum* datum )
{
  if ( !datum )
    return;

  _datumList.append( datum );
}

/*!
  \brief Remove given datum \a datum from the global list.

  This function is invoked automatically by QDS_Datum destructor.

  \param datum datum being unregistered
*/
void QDS::removeDatum( QDS_Datum* datum )
{
  if ( !datum )
    return;

  _datumList.removeAt( _datumList.indexOf(datum) );
}
