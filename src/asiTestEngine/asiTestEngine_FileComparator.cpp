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

// Own include
#include <asiTestEngine_FileComparator.h>

// OCCT includes
#include <OSD_File.hxx>
#include <OSD_Protection.hxx>

#define BUFLEN 1024

//-----------------------------------------------------------------------------

//! Complete constructor.
//! \param[in] filename1      first filename.
//! \param[in] filename2      second filename.
//! \param[in] lineFilter     string filter.
//! \param[in] lineComparator comparator for lines.
asiTestEngine_FileComparator::asiTestEngine_FileComparator(const TCollection_AsciiString&                  filename1,
                                                           const TCollection_AsciiString&                  filename2,
                                                           ActAux_StrFilter                                lineFilter,
                                                           const Handle(asiTestEngine_BaseLineComparator)& lineComparator)
: m_filename1      (filename1),
  m_filename2      (filename2),
  m_filter         (lineFilter),
  m_comparator     (lineComparator.IsNull() ? new asiTestEngine_DefaultLineComparator : lineComparator),
  m_bDone          (false),
  m_bResult        (false),
  m_iLineWithDiffs (-1)
{}

//-----------------------------------------------------------------------------

//! Performs actual comparison of file contents.
void asiTestEngine_FileComparator::Perform()
{
  m_bDone  = true;
  m_bResult = true;

  /* ===========================
   *  Open files for comparison
   * =========================== */

  OSD_File FILE1 = OSD_File( OSD_Path(m_filename1) );
  OSD_File FILE2 = OSD_File( OSD_Path(m_filename2) );

  OSD_Protection aProtection;

  FILE1.Open(OSD_ReadOnly, aProtection);
  FILE2.Open(OSD_ReadOnly, aProtection);

  if ( !FILE1.IsOpen() || !FILE2.IsOpen() )
  {
    m_bDone = false;
    return;
  }

  /* =================================
   *  Perform line-by-line comparison
   * ================================= */

  // Stuff for streaming
  int NbChar1, NbChar2;
  TCollection_AsciiString Buff1, Buff2;
  int LineIdx = 1;

  while ( !FILE1.IsAtEnd() )
  {
    FILE1.ReadLine(Buff1, BUFLEN, NbChar1);

    if ( FILE2.IsAtEnd() )
    {
      m_bResult = false;
      m_iLineWithDiffs = LineIdx;
      return;
    }

    FILE2.ReadLine(Buff2, BUFLEN, NbChar2);

    // Check filters. Normally filters are useful to exclude some dedicated
    // lines from comparison process. E.g. we can skip comparison of comment
    // lines so.
    if ( m_filter && !(*m_filter)(Buff1) && !(*m_filter)(Buff2) )
    {
      ++LineIdx;
      continue;
    }

    if ( !m_comparator->AreEqual(Buff1, Buff2) )
    {
      m_bResult = false;
      m_iLineWithDiffs = LineIdx;
      return;
    }

    ++LineIdx;
  }

  /* ==============
   *  Finalization
   * ============== */

  FILE1.Close();
  FILE2.Close();
}
