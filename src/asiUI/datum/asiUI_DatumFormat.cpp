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

// Own include
#include <asiUI_DatumFormat.h>

// asiUI includes
#include <asiUI_Common.h>

// Qt includes
#pragma warning(push, 0)
#include <QRegExp>
#include <QLocale>
#include <QStringList>
#pragma warning(pop)

#define MAX_NB_SIGNIFICANT_DIGITS 16

//! Check whether the format string correspond to 
bool asiUI_DatumFormat::isFormatX(const QString& theFormat)
{
  const static QRegExp aFmtPattern("^%[0-9]+\\.?[0-9]*[xX][0-9]*$");
  return aFmtPattern.exactMatch(theFormat);
}

//! Check whether the format string correspond to float-point formatting.
//! \param theFormat [in] the format string.
bool asiUI_DatumFormat::isFormatFloat(const QString& theFormat)
{
  return (theFormat.indexOf(QRegExp("[fegFEG]")) >= 0);
}

//=============================================================================
//                Simplified Sprintf Format
//=============================================================================

//! Sprintf-style formatting.
//! This formatter does not support short or long value representations
//! and is supposed to be used with non-floating point value types.
class asiUI_DatumFormat::FmtSprintf : public asiUI_DatumFormat
{
public:

  //! Constructor.
  //! \param theFormat [in] the used format.
  FmtSprintf(const QString& theFormat) :
    asiUI_DatumFormat(),
    m_Format (theFormat) {}

  //! Format string value.
  //! \param theValue [in] the value.
  //! \param theLongFmt [in] if long formatting style should be used.
  //! \return formatted string.
  QString Format(const QString& theValue, const bool asiUI_NotUsed(theLongFmt)) const
  {
    QString str;
    const char* strPtr = m_Format.toStdString().c_str();
#pragma warning (push)
#pragma warning (disable: 4840)
    QString res = str.sprintf(strPtr, theValue);
#pragma warning (pop)
    return res;
  }

  //! Format string value.
  //! \param theValue [in] the value.
  //! \param theLongFmt [in] if long formatting style should be used.
  //! \return formatted string.
  QString Format(const double theValue, const bool asiUI_NotUsed(theLongFmt)) const
  {
    return QString().sprintf(m_Format.toStdString().c_str(), theValue);
  }

  //! Format string value.
  //! \param theValue [in] the value.
  //! \param theLongFmt [in] if long formatting style should be used.
  //! \return formatted string.
  QString Format(const int theValue, const bool asiUI_NotUsed(theLongFmt)) const
  {
    return QString().sprintf(m_Format.toStdString().c_str(), theValue);
  }

private:

  QString m_Format; //!< format string.
};

//=============================================================================
//                Sprintf "F", "G", "E" formatting
//=============================================================================

//! Sprintf-style formatting for floating-point types 'g', 'f', 'e'.
//! Supports short and long value string representations.
class asiUI_DatumFormat::FmtFloat : public asiUI_DatumFormat
{
public:

  //! Constructor.
  //! Create both short and long format strings.
  //! \param theFormat [in] the used format.
  FmtFloat(const QString& theFormat) : asiUI_DatumFormat()
  {
    m_LongFmt = m_ShortFmt = theFormat;
    int aSep  = theFormat.indexOf(".");
    int aType = theFormat.indexOf(QRegExp("[fegFEG]"));
    int aSubStrLen = (aType - aSep - 1);
    if (aSep >= 0 && aType >= 0 && aSubStrLen > 0)
    {
      //bool isFormatG = (theFormat.indexOf(QRegExp("[gG]")) >= 0);
      int aPrecision = ASI_DATUM_DOUBLE_PRECISION;

      m_LongFmt.replace( aSep + 1, aSubStrLen, QString::number(aPrecision) );
    }
  }

  //! Format string value.
  //! \param theValue [in] the value.
  //! \param theLongFmt [in] if long formatting style should be used.
  //! \return formatted string.
  QString Format(const QString& theValue, const bool theLongFmt) const
  {
    int aLen = theValue.split('.').first().length();

    QByteArray aFmt = theLongFmt
      ? correctFormat(m_LongFmt, aLen).toLatin1()
      : correctFormat(m_ShortFmt, aLen).toLatin1();

#pragma warning (push)
#pragma warning (disable: 4840)
    QString aValue = QString().sprintf(aFmt.constData(), theValue);
#pragma warning (pop)
    return theLongFmt ? truncate(aValue, ASI_DATUM_DOUBLE_PRECISION) : aValue;
  }

  //! Format string value.
  //! \param theValue [in] the value.
  //! \param theLongFmt [in] if long formatting style should be used.
  //! \return formatted string.
  QString Format(const double theValue, const bool theLongFmt) const
  {
    int anExp = (abs(theValue) > DBL_MIN) ? (int)log10( abs(theValue) ) : 0;
    int aLen = anExp + 1;

    QByteArray aFmt = theLongFmt
      ? correctFormat(m_LongFmt, aLen).toLatin1()
      : correctFormat(m_ShortFmt, aLen).toLatin1();

    QString aValue = QString().sprintf(aFmt.constData(), theValue);
    return theLongFmt ? truncate(aValue, ASI_DATUM_DOUBLE_PRECISION) : aValue;
  }

  //! Format string value.
  //! \param theValue [in] the value.
  //! \param theLongFmt [in] if long formatting style should be used.
  //! \return formatted string.
  QString Format(const int theValue, const bool theLongFmt) const
  {
    int anExp = int( (abs(theValue) > DBL_MIN) ? log10( (double)abs(theValue) ) : 0 );
    int aLen = anExp + 1;

    QByteArray aFmt = theLongFmt
      ? correctFormat(m_LongFmt, aLen).toLatin1()
      : correctFormat(m_ShortFmt, aLen).toLatin1();

    QString aValue = QString().sprintf(aFmt.constData(), theValue);
    return theLongFmt ? truncate(aValue, ASI_DATUM_DOUBLE_PRECISION) : aValue;
  }

protected:

  //! Truncate string to number of significant digits.
  //! \param theValue [in] the string to truncate.
  //! \param theDigits [in] the number of digits in value string.
  QString truncate(const QString& theValue, const int theDigits) const
  {
    QChar aExponentChar = QLocale::c().exponential();
    QChar aDecimalChar  = QLocale::c().decimalPoint();

    // truncate to
    QStringList aSplitAtExp = theValue.split(aExponentChar);
    QString& aOnlyValue = aSplitAtExp.first();
    QStringList aSplitAtDec = aOnlyValue.split(aDecimalChar);

    QString& aLeft = aSplitAtDec.first();
    if (aSplitAtDec.size() > 1)
    {
      QString& aRight = aSplitAtDec.last();
      aRight.truncate( qMax(0, theDigits - aLeft.size()) );
      if (aRight.isEmpty())
      {
        aSplitAtDec.removeLast();
      }
    }

    aLeft.truncate( qMax(0, theDigits) );

    int aPosOfDecimal = aLeft.length();

    
    aOnlyValue = aSplitAtDec.join(aDecimalChar);

    // remove trailing zeroes
    int aPos = aOnlyValue.length() - 1;
    for (; aPos > aPosOfDecimal; --aPos)
    {
      const QChar aChar = aOnlyValue.at(aPos);
      if ( aChar != '0' )
        break;
    }

    aOnlyValue.truncate( (aPos == aPosOfDecimal) ? aPos + 2 : aPos + 1 );

    return aSplitAtExp.join( QLocale::c().exponential() );
  }

  // Correct the format for the number of digits in decimal part.
  // \param theFmt [in] the current format.
  // \param theNbDecimals [in] the number of digits in decimal part.
  // \return corrected format.
  QString correctFormat( const QString& theFmt, const int theNbDecimals) const
  {
    QString aFormat = theFmt;
    int aSep  = aFormat.indexOf(".");
    int aType = aFormat.indexOf(QRegExp("[fF]"));
    int aSubStrLen = (aType - aSep - 1);

    QString aFmtPrec = aFormat.split('.').last().left(aSubStrLen);

    if (aFmtPrec.toInt() + theNbDecimals < ASI_DATUM_DOUBLE_PRECISION)
      return theFmt;

    if (aType >=0)
    {
      // Maximum available number of digits after decimal point.
      int aPrecision = ASI_DATUM_DOUBLE_PRECISION - theNbDecimals;

      if ( aPrecision > aFmtPrec.toInt() )
        aPrecision = aFmtPrec.toInt();

      // Mimimal precision.
      if ( aPrecision < 1)
        aPrecision = 1;

      aFormat.replace( aSep + 1, aSubStrLen, QString::number(aPrecision) );
      return aFormat;
    }
    else
      return theFmt;
  }

private:

  QString m_LongFmt;
  QString m_ShortFmt;
};

//=============================================================================
//                asiUI Specific X formatting
//=============================================================================

//! asiUI "X-type" formatter.
//! Expected format string is "%(ORDER).(PRECISION)x(EXPSTEP)"
//! Supports both short and long string value representations.
class asiUI_DatumFormat::FmtAsiX : public asiUI_DatumFormat
{
public:

  //! Constructor.
  //! Create both short and long format strings.
  //! \param theFormat [in] the used format.
  FmtAsiX(const QString& theFormat) : asiUI_DatumFormat()
  {
    const static QRegExp aOrdMatch("^%(\\d+)");
    const static QRegExp aDecMatch("\\.(\\d+)[xX]");
    const static QRegExp aStepMatch("[xX](\\d*)$");

    aOrdMatch.indexIn(theFormat);
    aDecMatch.indexIn(theFormat);
    aStepMatch.indexIn(theFormat);

    m_iOrder    = qMax( 1, aOrdMatch.cap(1).toInt() );
    m_iDecimals = qMax( 1, aDecMatch.cap(1).toInt() );
    m_iExpStep  = qMax( 1, aStepMatch.cap(1).toInt() );
    m_bLower    = (theFormat.indexOf("x") >= 0);
  }

  //! Format string value.
  //! \param theValue [in] the value.
  //! \param theLongFmt [in] if long formatting style should be used.
  //! \return formatted string.
  QString Format(const QString& theValue, const bool theLongFmt) const
  {
    bool isOk = false;
    double aValue = theValue.toDouble(&isOk);
    if (!isOk)
      return QString("");

    return convert(aValue, theLongFmt);
  }

  //! Format string value.
  //! \param theValue [in] the value.
  //! \param theLongFmt [in] if long formatting style should be used.
  //! \return formatted string.
  QString Format(const double theValue, const bool theLongFmt) const
  {
    return convert(theValue, theLongFmt);
  }

  //! Format string value.
  //! \param theValue [in] the value.
  //! \param theLongFmt [in] if long formatting style should be used.
  //! \return formatted string.
  QString Format(const int theValue, const bool theLongFmt) const
  {
    return convert((double)theValue, theLongFmt);
  }

protected:

  //! Do formatting according to x format specification.
  //! \param theValue [in] the value to be formatted.
  //! \param theIsLong [in] indicates whether the long representation
  //! should be choosen or not.
  QString convert(const double theValue, const bool theIsLong) const
  {
    int aPnt, aSign;

    // estimate value exponent
    int anExp = int(abs(theValue) > DBL_MIN) ? int( floor( log10( abs(theValue) ) ) ) : 0;
    int aPrintDecimals = theIsLong ? ASI_DATUM_DOUBLE_PRECISION : m_iDecimals;

    // check if non-scientific formatting possible
    bool isScientific = abs(anExp) > m_iOrder || anExp < -m_iDecimals;
    if (!isScientific)
    {
      int aDecPart = anExp+1;
      if (aDecPart + ASI_DATUM_DOUBLE_PRECISION > MAX_NB_SIGNIFICANT_DIGITS && theIsLong)
        aPrintDecimals = MAX_NB_SIGNIFICANT_DIGITS - aDecPart;

      if (aPrintDecimals > ASI_DATUM_DOUBLE_PRECISION)
        aPrintDecimals = ASI_DATUM_DOUBLE_PRECISION;

      QString aFormatted = QString::number(theValue, 'f', aPrintDecimals);
      removeTrailingZeroes (aFormatted);
      return aFormatted;
    }

    /* ======================================================================= *
     * compute exponent and point position using the step, e.g E(+-aExponent)  *
     * ======================================================================= */

    // correction for negative exponent
    int aCorExp = anExp;
    if ( aCorExp < 0 )
      aCorExp++;

    // compute exponent according to exponential step.
    int aFormatExp = aCorExp - aCorExp % m_iExpStep;

    // position of decimal point against string of significant digits.
    // 10   -> (significant 10): aPointPos =  2;
    // 0.01 -> (significant  1): aPointPos = -1;
    int aPointPos = (anExp >= 0)
      ? anExp - aFormatExp + 1
      : anExp - aFormatExp + 1;

    // number of significant digits to be shown.
    int aNbHighOrder = (aPointPos > 0 ? aPointPos : 0);
    int aNbLowOrder  = (aPointPos > 0 ? aPrintDecimals : aPrintDecimals + aPointPos);
    int aNbDigits    = aNbHighOrder + aNbLowOrder;

    // If nubmer of significant digits is more then maximal - do correction.
    if ( aNbDigits > MAX_NB_SIGNIFICANT_DIGITS )
    {
      aNbLowOrder = MAX_NB_SIGNIFICANT_DIGITS - aNbHighOrder;

      if ( aNbLowOrder > ASI_DATUM_DOUBLE_PRECISION )
        aNbLowOrder = ASI_DATUM_DOUBLE_PRECISION;

      aNbDigits = aNbHighOrder + aNbLowOrder;
    }

    // generate string of significant digits
#pragma warning (push)
#pragma warning (disable: 4996)
    QString aDigits = QString::fromLatin1( ecvt(theValue, aNbDigits, &aPnt, &aSign) );
#pragma warning(pop)

    // remove trailing zeroes, but keep number of significant
    // digits (even zeroes) specified by format.
    int aKeepSignificant = theIsLong 
      ? qMax(0, aNbHighOrder + 1)
      : qMax(0, aNbHighOrder + m_iDecimals);

    // remove trailing zeroes
    removeTrailingZeroes (aDigits, aKeepSignificant);

    QLocale aCLocale = QLocale::c();
    QChar aDecimalChar = aCLocale.decimalPoint();

    // add decimal point
    if (aPointPos <= 0)
    {
      QString aPrefix("0");
      aPrefix.append(aDecimalChar);
      aPrefix.append( QString().fill('0', -aPointPos) );
      aDigits.prepend(aPrefix);
    }
    else
    {
      aDigits.insert(aPointPos, aDecimalChar);
    }

    // add exponent
    QString aPrintStr;
    QChar aExpChar = aCLocale.exponential();
    aDigits.append( m_bLower ? aExpChar.toLower() : aExpChar.toUpper() );
    aDigits.append( aPrintStr.sprintf("%+i", aFormatExp) );
    if (aSign > 0)
    {
      aDigits.prepend( aCLocale.negativeSign() );
    }

    return aDigits;
  }

  //! Remove trailing zeroes in the string starting from the end.
  //! \param theStr [in] the str to modify.
  //! \param theStart [in] the position in the str to start from.
  void removeTrailingZeroes(QString& theStr, int theStart = 0) const
  {
    int aPos = theStr.length() - 1;
    for (; aPos >= theStart; --aPos)
    {
      if (theStr.at(aPos) != '0')
        break;
    }

    if ( theStr.at(aPos) == QLocale::c().decimalPoint() )
      aPos--;

    theStr.truncate (aPos + 1);
  }

private:

  bool m_bLower;     //!< whether to print lower character 'e'
  int  m_iOrder;     //!< max order before switching to scientific notation
  int  m_iDecimals;  //!< max number of decimals
  int  m_iExpStep;   //!< exp step
};

//! Create appropriate formatter instance.
//! The formatter class is choosen depending on the passed format string.
//! \param theFormat [in] the format string.
asiUI_DatumFormatPtr asiUI_DatumFormat::Make(const QString& theFormat)
{
  asiUI_DatumFormat* aFormatter = 0;

  if ( isFormatX(theFormat) )
  {
    aFormatter = new FmtAsiX(theFormat);
  }
  else if ( isFormatFloat(theFormat) )
  {
    aFormatter = new FmtFloat(theFormat);
  }
  else if ( !theFormat.isEmpty() )
  {
    aFormatter = new FmtSprintf(theFormat);
  }

  return asiUI_DatumFormatPtr(aFormatter);
}
