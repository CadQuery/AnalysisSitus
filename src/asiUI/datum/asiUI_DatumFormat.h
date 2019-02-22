//-----------------------------------------------------------------------------
// Created on: 21 February 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Anton Poletaev, Sergey Slyadnev
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

#ifndef asiUI_DatumFormat_HeaderFile
#define asiUI_DatumFormat_HeaderFile

// asiUI includes
#include <asiUI.h>

// Qt includes
#pragma warning(push, 0)
#include <QString>
#include <QSharedPointer>
#pragma warning(pop)

#define ASI_DATUM_DOUBLE_PRECISION 14

class asiUI_DatumFormat;

typedef QSharedPointer<asiUI_DatumFormat> asiUI_DatumFormatPtr;

//! Class encapsulates logic of value formatting used in datums.
//! Formatter instance can be initialized from format string.
//! Formatter has an option to format passed value in long or short form.
//! - Long form keeps value precision, the significant decimals are not
//! truncated even if format string declares so.
//! - Short form fully correspond to format string specification.
class asiUI_EXPORT asiUI_DatumFormat
{
public:

  static asiUI_DatumFormatPtr Make(const QString& theFormat);

  //! Format string value.
  //! \param theValue [in] the value.
  //! \param theLongFmt [in] if long formatting style should be used.
  //! \return formatted string.
  virtual QString Format(const QString& theValue, const bool theLongFmt) const = 0;

  //! Format double value.
  //! \param theValue [in] the value.
  //! \param theLongFmt [in] if long formatting style should be used.
  //! \return formatted string.
  virtual QString Format(const double theValue, const bool theLongFmt) const = 0;

  //! Format integer value.
  //! \param theValue [in] the value.
  //! \param theLongFmt [in] if long formatting style should be used.
  //! \return formatted string.
  virtual QString Format(const int theValue, const bool theLongFmt) const = 0;

public:

  static bool isFormatX(const QString& theFormat);

  static bool isFormatFloat(const QString& theFormat);

public:

  class FmtSprintf; // standard sprintf formatting
  class FmtFloat;   // standard sprintf formatting for g,f,e
  class FmtAsiX;    // Asi x formatting
};

#endif
