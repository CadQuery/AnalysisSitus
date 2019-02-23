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

#ifndef QDS_H
#define QDS_H

// asiUI includes
#include <asiUI.h>

#if defined WIN32
#pragma warning ( disable:4251 )
#pragma warning ( disable:4786 )
#endif

// Qt includes
#pragma warning(push, 0)
#include <QString>
#include <QList>
#pragma warning(pop)

// OCCT includes
#include <TCollection_HAsciiString.hxx>
#include <TCollection_HExtendedString.hxx>

class QDS_Datum;

class asiUI_EXPORT QDS
{
public:
  //! Enum describes bit flags of the Qt datum view and behaviour
  typedef enum
  {
    None           = 0x00,         //!< Non specified any flags (Default behaviour)
    Label          = 0x01,         //!< Create subwidget for datum label
    Control        = 0x02,         //!< Create subwidget for datum input control
    Units          = 0x04,         //!< Create subwidget for datum units of measure
    NotFormat      = 0x08,         //!< Don't format initial value
    NotAccel       = 0x10,         //!< Not support accelerators in datum label
    NotConvert     = 0x20,         //!< Don't convert numeric value
    UnitsWithLabel = 0x40,         //!< Display units of measure in label like "<label_text> (<units_text>) instead separate text"
    All = Label | Control | Units  //!< Create all subwidgets
  } DatumFlags;

public:
  static bool                       load( const QString& );

  static QString                    unitSystemLabel( const QString&,
                                                     const QString& = QString() );
  static QString                    activeUnitSystem( const QString& = QString() );
  static void                       setActiveUnitSystem( const QString&,
                                                         const QString& = QString() );

  static QString                    toQString( const TCollection_AsciiString& );
  static QString                    toQString( const TCollection_ExtendedString& );
  static QString                    toQString( const Handle(TCollection_HAsciiString)& );
  static QString                    toQString( const Handle(TCollection_HExtendedString)& );

  static TCollection_AsciiString    toAsciiString( const QString& );
  static TCollection_AsciiString    toAsciiString( const TCollection_ExtendedString& );
  static TCollection_AsciiString    toAsciiString( const Handle(TCollection_HExtendedString)& );

  static TCollection_ExtendedString toExtString( const QString& );
  static TCollection_ExtendedString toExtString( const TCollection_AsciiString& );

protected:
  static void                       insertDatum( QDS_Datum* );
  static void                       removeDatum( QDS_Datum* );

private:
  static QList<QDS_Datum*>          _datumList;
};

#endif
