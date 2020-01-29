//-----------------------------------------------------------------------------
// Created on: 15 October 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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

#ifndef asiTestEngine_FileComparator_HeaderFile
#define asiTestEngine_FileComparator_HeaderFile

// asiTestEngine includes
#include <asiTestEngine.h>

// Active Data (auxiliary) includes
#include <ActAux.h>
#include <ActAux_Utils.h>

//-----------------------------------------------------------------------------

//! Base class for line comparators.
class asiTestEngine_BaseLineComparator : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiTestEngine_BaseLineComparator, Standard_Transient)

public:

  virtual bool
    AreEqual(const TCollection_AsciiString& string1,
             const TCollection_AsciiString& string2) = 0;

};

//-----------------------------------------------------------------------------

//! Default comparator for lines.
class asiTestEngine_DefaultLineComparator : public asiTestEngine_BaseLineComparator
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiTestEngine_DefaultLineComparator, asiTestEngine_BaseLineComparator)

public:

  //! Standard way to compare lines as string values.
  //! \param[in] string1 first string.
  //! \param[in] string2 second string.
  //! \return true/false.
  virtual bool AreEqual(const TCollection_AsciiString& string1,
                        const TCollection_AsciiString& string2)
  {
    return string1.IsEqual(string2);
  }

};

//-----------------------------------------------------------------------------

//! Comparator for lines containing floating-point values.
class asiTestEngine_FltLineComparator : public asiTestEngine_BaseLineComparator
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiTestEngine_FltLineComparator, asiTestEngine_BaseLineComparator)

public:

  //! Constructor accepting a precision for comparison. NOTICE: precision is
  //! understood as percentage.
  //! \param[in] deviation  allowed deviation for floating-point comparison.
  //! \param[in] resolution this parameter defines resolution of the comparator.
  //!                       If the difference between two values is less than
  //!                       resolution, then there is no sense to check their
  //!                       relative difference. This can be useful if you want
  //!                       to skip small numbers like 0.0001 vs 0.000001.
  //!                       While the latter number is 100 times less than the
  //!                       first values, their difference is insignificant
  //!                       from engineering point of view.
  asiTestEngine_FltLineComparator(const int    deviation,
                                  const double resolution = 1.0e-3) : asiTestEngine_BaseLineComparator()
  {
    m_iDev        = deviation;
    m_fResolution = resolution;
  }

public:

  //! Compares the passed lines as floating-point values if possible. If not,
  //! compares them as standard string values. For floating-point comparison
  //! precision value is used.
  //! \param[in] string1 first string.
  //! \param[in] string2 second string.
  //! \return true/false.
  virtual bool AreEqual(const TCollection_AsciiString& string1,
                        const TCollection_AsciiString& string2)
  {
    TCollection_AsciiString Token1, Token2;
    int t1 = 1, t2 = 1;
    double val1, val2;

    do
    {
      Token1 = string1.Token(" \t", t1++);
      Token2 = string2.Token(" \t", t2++);

      bool areEqual;
      if ( Token1.IsRealValue() && Token2.IsRealValue() )
      {
        val1 = Token1.RealValue();
        val2 = Token2.RealValue();

        // Fuzzy comparison
        areEqual = asiTestEngine::FuzzyCompare(val1, val2, m_iDev, m_fResolution);
      }
      else
        areEqual = Token1.IsEqual(Token2);

      if ( !areEqual )
        return false;
    }
    while ( !Token1.IsEmpty() && !Token2.IsEmpty() );

    // If we are here, then every token has passed the check.
    return true;
  }

private:

  //! Allowable deviation for floating-point comparison.
  int m_iDev;

  //! Resolution.
  double m_fResolution;

};

//-----------------------------------------------------------------------------

//! Tool to compare text files.
class asiTestEngine_FileComparator
{
public:

  asiTestEngine_EXPORT
    asiTestEngine_FileComparator(const TCollection_AsciiString&                  filename1,
                                 const TCollection_AsciiString&                  filename2,
                                 ActAux_StrFilter                                lineFilter     = nullptr,
                                 const Handle(asiTestEngine_BaseLineComparator)& lineComparator = nullptr);

public:

  asiTestEngine_EXPORT void
    Perform();

public:

  //! Returns true if comparison has been performed.
  //! \return true/false.
  bool IsDone() const
  {
    return m_bDone;
  }

  //! Returns true if compared files are the same.
  //! \return true/false.
  bool AreSame() const
  {
    return m_bResult;
  }

  //! Returns index of the first line where some differences have been
  //! detected. Returns -1 if no such line found.
  //! \return index of line, starting from 1.
  int LineWithDiffs() const
  {
    return m_iLineWithDiffs;
  }

private:

  TCollection_AsciiString                  m_filename1;  //!< Filename 1.
  TCollection_AsciiString                  m_filename2;  //!< Filename 2.
  ActAux_StrFilter                         m_filter;     //!< Line filter.
  Handle(asiTestEngine_BaseLineComparator) m_comparator; //!< Line comparator.

private:

  bool m_bDone;          //!< Completeness indicator.
  bool m_bResult;        //!< Comparison result.
  int  m_iLineWithDiffs; //!< First line containing differences.

};

#endif
