//-----------------------------------------------------------------------------
// Created on: 25 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
#pragma warning(push, 0)
#include <QColor>
#include <QDateTime>
#pragma warning(pop)

//-----------------------------------------------------------------------------

// Invalid resource pixmap.
static const char* asiUI_PixmapNotFound[] = {
"16 16 3 1",
"       c None",
".      c #000000",
"+      c #A80000",
"                ",
"                ",
"    .     .     ",
"   .+.   .+.    ",
"  .+++. .+++.   ",
"   .+++.+++.    ",
"    .+++++.     ",
"     .+++.      ",
"    .+++++.     ",
"   .+++.+++.    ",
"  .+++. .+++.   ",
"   .+.   .+.    ",
"    .     .     ",
"                ",
"                ",
"                "};

static const char* asiUI_BrowsePixmap[] = {
"16 16 5 1",
"  c none",
". c #ffff00",
"# c #848200",
"a c #ffffff",
"b c #000000",
"                ",
"          bbb   ",
"         b   b b",
"              bb",
"  bbb        bbb",
" ba.abbbbbbb    ",
" b.a.a.a.a.b    ",
" ba.a.a.a.ab    ",
" b.a.abbbbbbbbbb",
" ba.ab#########b",
" b.ab#########b ",
" bab#########b  ",
" bb#########b   ",
" bbbbbbbbbbb    ",
"                ",
"                "
};

//-----------------------------------------------------------------------------

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

  //! Converts QString to std::string
  //! \return converted string
  static std::string ToStandardString(const QString& theStr)
  {
    std::string utf8_text = theStr.toUtf8().constData();
    return utf8_text;
  }

  //! Converts TCollection_ExtendedString to std::string
  //! \return converted string
  static std::string ToStandardString(const TCollection_ExtendedString& theStr)
  {
    std::string res = TCollection_AsciiString(theStr).ToCString();
    return res;
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

  //! Convert QDateTime to a time stamp representation.
  //! \param[in] dateTime qt's date and time representation.
  //! \return date and time converted to time stamp.
  static Handle(ActAux_TimeStamp) ToTimeStamp(const QDateTime& dateTime)
  {
    Handle(HIntArray) dateArr = new HIntArray(0, 9);

    QTime time = dateTime.time();
    dateArr->ChangeValue(0) = time.second();      // seconds [0-59]
    dateArr->ChangeValue(1) = time.minute();      // minutes [0-59]
    dateArr->ChangeValue(2) = time.hour();        // hours   [0-23]

    QDate date = dateTime.date();
    dateArr->ChangeValue(3) = date.day();          // days   [1-31]
    dateArr->ChangeValue(4) = date.month() - 1;    // months [1-12]  -> months [0-11]
    dateArr->ChangeValue(5) = date.year()  - 1900; // year [0 - ...] -> [1900-...]

    return ActAux_TimeStampTool::FromChunked(dateArr);
  }

public:

  asiUI_EXPORT static QString
    selectGraphicsFile(const OpenSaveAction action);

  asiUI_EXPORT static QString
    selectBRepFile(const OpenSaveAction action);

  asiUI_EXPORT static QString
    selectIGESFile(const OpenSaveAction action);

  asiUI_EXPORT static QString
    selectSTEPFile(const OpenSaveAction action);

  asiUI_EXPORT static QString
    selectPLYFile(const OpenSaveAction action);

  asiUI_EXPORT static QString
    selectXBFFile(const OpenSaveAction action);

  asiUI_EXPORT static QString
    selectXYZFile(const OpenSaveAction action);

  asiUI_EXPORT static QString
    selectREKFile(const OpenSaveAction action);

  asiUI_EXPORT static QString
    selectOBJFile(const OpenSaveAction action);

  asiUI_EXPORT static QString
    selectSTLFile(const OpenSaveAction action);

  asiUI_EXPORT static QString
    selectDOTFile(const OpenSaveAction action);

//-----------------------------------------------------------------------------

  asiUI_EXPORT static QString
    selectFile(const QString&       filter,
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

#define ExtStr2StdStr(ExtStr) \
  asiUI_Common::ToStandardString(ExtStr)

#define AsciiStr2QStr(AsciiStr) \
  QObject::tr( AsciiStr.ToCString() )

#define QStr2AsciiStr(QStr) \
  asiUI_Common::ToAsciiString(QStr)

#define QStr2StdStr(QStr) \
  asiUI_Common::ToStandardString(QStr)

#define QStr2ExtStr(QStr) \
  asiUI_Common::ToExtString(QStr)

#endif
