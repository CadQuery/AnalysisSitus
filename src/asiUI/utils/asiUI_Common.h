//-----------------------------------------------------------------------------
// Created on: 25 November 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiUI_Common_h
#define asiUI_Common_h

// asiUI includes
#include <asiUI.h>

// asiEngine includes
#include <asiEngine_Model.h>

// Active Data includes
#include <ActAux_TimeStamp.h>

// OCCT includes
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TopoDS_Shape.hxx>

// Qt includes
#include <QColor>
#include <QDateTime>

//! GUI utilities.
class asiUI_Common
{
public:

  //! Enumeration for standard open-save dialogs.
  enum OpenSaveAction
  {
    OpenSaveAction_Open,
    OpenSaveAction_Save
  };

public:

  //! Converts TCollection_AsciiString to QString
  //! \return converted string
  static QString ToQString(const TCollection_AsciiString& theStr)
  {
    return QString( theStr.ToCString() );
  }

  //! Converts TCollection_ExtendedString to QString
  //! \return converted string
  static QString ToQString(const TCollection_ExtendedString& theStr)
  {
    return QString( (const QChar*) theStr.ToExtString(), theStr.Length() );
  }

  //! Converts QString to TCollection_AsciiString
  //! \return converted string
  static TCollection_AsciiString ToAsciiString(const QString& theStr)
  {
    return ( !theStr.isEmpty() ) ?
      TCollection_AsciiString( theStr.toLatin1().data() ) : TCollection_AsciiString();
  }

  //! Converts QString to TCollection_ExtendedString
  //! \return converted string
  static TCollection_ExtendedString ToExtString(const QString& theStr)
  {
    TCollection_ExtendedString aRes;
    for ( int i = 0; i < (int) theStr.length(); ++i )
    {
      aRes.Insert(i + 1, theStr[i].unicode());
    }
    return aRes;
  }

  //! Converts color value to an integer representation.
  //! \param theColor [in] color.
  //! \return converted value
  static int ColorToInt(const QColor& theColor)
  {
    unsigned char aRed   = (unsigned char) theColor.red();
    unsigned char aGreen = (unsigned char) theColor.green();
    unsigned char aBlue  = (unsigned char) theColor.blue();
    return aRed << 16 | aGreen << 8 | aBlue;
  }

  //! Converts integer value to a color.
  //! \param theColor [in] integer value.
  //! \return converted value
  static QColor IntToColor(const int theColor)
  {
    unsigned char aRed   = ( theColor >> 16 ) & 0xFF;
    unsigned char aGreen = ( theColor >>  8 ) & 0xFF;
    unsigned char aBlue  =   theColor         & 0xFF;
    return QColor(aRed, aGreen, aBlue);
  }

  //! Converts timestamp to QDateTime value.
  //! \param timeStamp [in] timestamp.
  //! \return date and time as QDateTime.
  static QDateTime ToQDateTime(const Handle(ActAux_TimeStamp)& timeStamp)
  {
    Handle(HIntArray) dateArr = ActAux_TimeStampTool::AsChunked(timeStamp);

    if ( dateArr.IsNull() )
      return QDateTime();

    QDate date(dateArr->Value(5) + 1900, // year [1900-...] -> [0 - ...]
               dateArr->Value(4) + 1,    // months [0-11]   -> [1-12]
               dateArr->Value(3));       // days   [1-31]
    //
    QTime time(dateArr->Value(2),        // hours   [0-23]
               dateArr->Value(1),        // minutes [0-59]
               dateArr->Value(0));       // seconds [0-59]

    return QDateTime(date, time);
  }

public:

  asiUI_EXPORT static QString
    selectBRepFile(const OpenSaveAction action);

  asiUI_EXPORT static QString
    selectIGESFile(const OpenSaveAction action);

  asiUI_EXPORT static QString
    selectSTEPFile(const OpenSaveAction action);

  asiUI_EXPORT static QString
    selectPlyFile(const OpenSaveAction action);

  asiUI_EXPORT static QString
    selectXBFFile(const OpenSaveAction action);

  asiUI_EXPORT static QString
    selectXYZFile(const OpenSaveAction action);

  asiUI_EXPORT static QString
    selectOBJFile(const OpenSaveAction action);

  asiUI_EXPORT static QString
    selectSTLFile(const OpenSaveAction action);

//-----------------------------------------------------------------------------

  asiUI_EXPORT static QString
    selectFile(const QStringList&   filter,
               const QString&       openTitle,
               const QString&       saveTitle,
               const OpenSaveAction action);

public:

  asiUI_EXPORT static bool
    PartShape(const Handle(asiEngine_Model)& model,
              Handle(asiData_PartNode)&      part_n,
              TopoDS_Shape&                  part);

};

#define CStr2ExtStr(CStr) \
  asiUI_Common::ToExtString( QObject::tr(CStr) )

#define CStr2QStr(CStr) \
  QObject::tr(CStr)

#define ExtStr2QStr(ExtStr) \
  QObject::tr( TCollection_AsciiString(ExtStr).ToCString() )

#define AsciiStr2QStr(AsciiStr) \
  QObject::tr( AsciiStr.ToCString() )

#define QStr2AsciiStr(QStr) \
  asiUI_Common::ToAsciiString(QStr)

#define QStr2ExtStr(QStr) \
  asiUI_Common::ToExtString(QStr)

#endif
