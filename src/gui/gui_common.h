//-----------------------------------------------------------------------------
// Created on: 25 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_common_h
#define gui_common_h

// A-Situs includes
#include <analysis_situs.h>

// Qt includes
#include <QColor>

//! GUI utilities.
class gui_common
{
public:

  //! Enumeration for standard open-save dialogs.
  enum OpenSaveAction
  {
    OpenSaveAction_Open,
    OpenSaveAction_Save
  };

public:

  //! Convert TCollection_AsciiString to QString
  //! \return converted string
  static QString ToQString(const TCollection_AsciiString& theStr)
  {
    return QString( theStr.ToCString() );
  }

  //! Convert TCollection_ExtendedString to QString
  //! \return converted string
  static QString ToQString(const TCollection_ExtendedString& theStr)
  {
    return QString( (const QChar*) theStr.ToExtString(), theStr.Length() );
  }

  //! Convert QString to TCollection_AsciiString
  //! \return converted string
  static TCollection_AsciiString ToAsciiString(const QString& theStr)
  {
    return ( !theStr.isEmpty() ) ?
      TCollection_AsciiString( theStr.toLatin1().data() ) : TCollection_AsciiString();
  }

  //! Convert QString to TCollection_ExtendedString
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

  //! Convert color value to an integer representation.
  //! \param theColor [in] color.
  //! \return converted value
  static int ColorToInt(const QColor& theColor)
  {
    unsigned char aRed   = (unsigned char) theColor.red();
    unsigned char aGreen = (unsigned char) theColor.green();
    unsigned char aBlue  = (unsigned char) theColor.blue();
    return aRed << 16 | aGreen << 8 | aBlue;
  }

  //! Convert integer value to a color.
  //! \param theColor [in] integer value.
  //! \return converted value
  static QColor IntToColor(const int theColor)
  {
    unsigned char aRed   = ( theColor >> 16 ) & 0xFF;
    unsigned char aGreen = ( theColor >>  8 ) & 0xFF;
    unsigned char aBlue  =  theColor          & 0xFF;
    return QColor(aRed, aGreen, aBlue);
  }

public:

  static QString
    selectBRepFile(const OpenSaveAction action);

  static QString
    selectIGESFile(const OpenSaveAction action);

  static QString
    selectSTEPFile(const OpenSaveAction action);

  static QString
    selectPlyFile(const OpenSaveAction action);

  static QString
    selectXBFFile(const OpenSaveAction action);

  static QString
    selectXYZFile(const OpenSaveAction action);

  static QString
    selectOBJFile(const OpenSaveAction action);

//-----------------------------------------------------------------------------

  static QString
    selectFile(const QStringList&   filter,
               const QString&       openTitle,
               const QString&       saveTitle,
               const OpenSaveAction action);

};

#define CStr2ExtStr(CStr) \
  gui_common::ToExtString( QObject::tr(CStr) )

#define CStr2QStr(CStr) \
  QObject::tr(CStr)

#define ExtStr2QStr(ExtStr) \
  QObject::tr( TCollection_AsciiString(ExtStr).ToCString() )

#define AsciiStr2QStr(AsciiStr) \
  QObject::tr( AsciiStr.ToCString() )

#define QStr2AsciiStr(QStr) \
  gui_common::ToAsciiString(QStr)

#define QStr2ExtStr(QStr) \
  gui_common::ToExtString(QStr)

#endif
