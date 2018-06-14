//-----------------------------------------------------------------------------
// Created on: 11 June 2013
//-----------------------------------------------------------------------------
// Copyright (c) 2013-present, Sergey Slyadnev
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
//    * Neither the name of Sergey Slyadnev nor the
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

// Windows includes
#include <windows.h>

// Own include
#include <asiTestEngine_Launcher.h>

// asiTestEngine includes
#include <asiTestEngine_DescriptionProc.h>
#include <asiTestEngine_ReportRenderer.h>
#include <asiTestEngine_ReportStyleFactory.h>

// asiAlgo includes
#include <asiAlgo_TimeStamp.h>
#include <asiAlgo_Utils.h>

// STD includes
#include <fstream>

//! Adds the passed Test Case Launcher to the internal collection.
//! \param CaseLauncher [in] Test Case Launcher to add.
//! \return this for subsequent streaming.
asiTestEngine_Launcher&
  asiTestEngine_Launcher::operator<<(const Handle(asiTestEngine_CaseLauncherAPI)& CaseLauncher)
{
  m_launchers.push_back(CaseLauncher);
  return *this;
}

//! Launches all managed Test Cases.
//! \param out [in, optional] output stream.
//! \return true if all Cases have succeeded, false -- otherwise.
bool asiTestEngine_Launcher::Launch(std::ostream* out) const
{
  /* ==============================
   *  Launch Test Cases one by one
   * ============================== */

  bool isOk = true;
  int numTotal = 0, numFailed = 0;
  for ( int l = 0; l < (int) m_launchers.size(); ++l )
  {
    const Handle(asiTestEngine_CaseLauncherAPI)& CaseLauncher = m_launchers.at(l);
    const bool nextOk = CaseLauncher->Launch();

    // Put message to output stream
    if ( out )
    {
      *out << "\tCase " << CaseLauncher->CaseID() << ": " << (nextOk ? "Ok" : "Failed");
      *out << "; (Total / Failed) = (" << CaseLauncher->NumberOfExecuted() << " / "
                                       << CaseLauncher->NumberOfFailed() << ")\n";
    }

    numTotal += CaseLauncher->NumberOfExecuted();
    numFailed += CaseLauncher->NumberOfFailed();

    if ( !nextOk && isOk )
      isOk = false;
  }

  if ( out )
  {
    *out << "\t***\n";
    *out << "\tTotal executed: " << numTotal << "\n";
    *out << "\tTotal failed: " << numFailed << "\n";
  }

  /* ================
   *  Prepare report
   * ================ */

  if ( out )
    *out << "\t***\n";
  if ( this->generateReport(out) )
  {
    if ( out )
      *out << "\tReport generation succeeded\n";
  }
  else
  {
    if ( out )
      *out << "\tReport generation failed (!!!)\n";
  }

  return isOk;
}

//! Generates HTML report for the Test Cases identified by the managed
//! Launchers.
//! \param out [in] output stream.
//! \return true in case of success, false -- otherwise.
bool asiTestEngine_Launcher::generateReport(std::ostream* out) const
{
  /* ===========================
   *  Render header information
   * =========================== */

  Handle(asiTestEngine_ReportRenderer) Rdr = new asiTestEngine_ReportRenderer;

  // Global style for HTML body
  asiTestEngine_ReportStyle BodyStyle;
  BodyStyle.SetFontFamily("Verdana");

  // Global style for TD elements
  asiTestEngine_ReportStyle CellStyle;
  CellStyle.SetFontSize(11);

  // Global style for header cells
  asiTestEngine_ReportStyle HCellStyle;
  HCellStyle.SetBgColor( asiTestEngine_ReportStyle::Color(215, 215, 200) );

  // Global style for TD elements for "good" results
  asiTestEngine_ReportStyle GoodCellStyle;
  GoodCellStyle.SetBgColor( asiTestEngine_ReportStyle::Color(180, 220, 25) );

  // Global style for TD elements for "bad" results
  asiTestEngine_ReportStyle BadCellStyle;
  BadCellStyle.SetBgColor( asiTestEngine_ReportStyle::Color(255, 0, 0) );

  // Global style for tables
  asiTestEngine_ReportStyle TableStyle;
  TableStyle.SetBorder(1);
  TableStyle.SetPadding(5);

  // Generate HTML heading section
  Rdr->AddDoctype()
     ->StartHtml()
     ->StartHeader()
     ->AddMeta()
     ->StartStyle()
     ->AddClass("body_class", BodyStyle)
     ->AddClass("table_class", TableStyle)
     ->AddClass("cell_class", CellStyle)
     ->AddClass("good_cell_class", GoodCellStyle)
     ->AddClass("bad_cell_class", BadCellStyle)
     ->AddClass("header_cell_class", HCellStyle)
     ->EndStyle()
     ->EndHeader()
     ->StartBody("body_class");

  // Generate table header
  Rdr->StartTable("table_class")
     ->StartTableRow()
     ->StartColSpanTableHCell(2, "table_class cell_class")
     ->AddText(asiTestEngine_Macro_TEST)
     ->EndTableHCell()
     ->StartTableHCell("table_class cell_class")
     ->AddText(asiTestEngine_Macro_RESULT)
     ->EndTableHCell()
     ->EndTableRow();

  /* =======================================
   *  Render information per each Test Case
   * ======================================= */

  // Iterate over Test Cases
  for ( int l = 0; l < (int) m_launchers.size(); ++l )
  {
    const Handle(asiTestEngine_CaseLauncherAPI)& CaseLauncher = m_launchers.at(l);

    // Get filename for description
    std::string descGroupDir = CaseLauncher->CaseDescriptionDir();
    std::string descFilename = CaseLauncher->CaseDescriptionFn() + asiTestEngine_Macro_DOT + asiTestEngine_Macro_DESCR_EXT;
    std::string descDir      = asiAlgo_Utils::Str::Slashed( asiAlgo_Utils::Env::AsiTestDescr() ) + descGroupDir;

    // Description processing tool
    std::string title;
    std::vector<std::string> overviewBlocks, detailBlocks;
    if ( !asiTestEngine_DescriptionProc::Process(descDir,
                                                 descFilename,
                                                 CaseLauncher->Variables(),
                                                 CaseLauncher->CaseID(),
                                                 title,
                                                 overviewBlocks,
                                                 detailBlocks) )
    {
      if ( out )
        *out << "\tFailed to read description from \"" << descFilename.c_str() << "\"\n";
      return false;
    }

    // Local statistics
    const int nTotal = CaseLauncher->NumberOfExecuted();
    const int nFailed = CaseLauncher->NumberOfFailed();
    const double passedPercent = (double) (nTotal-nFailed)/nTotal*100.0;

    // Render header for Test Case
    Rdr->StartTableRow()
       ->StartTableHCell("table_class cell_class header_cell_class")
       ->AddText( CaseLauncher->CaseID() )
       ->EndTableHCell()
       ->StartTableHCell("table_class cell_class header_cell_class")
       ->AddText(title)
       ->EndTableHCell();

    // Finish row with local statistics
    Rdr->StartTableHCell( (nFailed == 0) ? "table_class cell_class good_cell_class"
                                         : "table_class cell_class bad_cell_class" );
    Rdr->AddText(passedPercent)->AddText("%")->EndTableHCell();
    Rdr->EndTableRow();

    // Check number of OVERVIEW blocks
    if ( (int) overviewBlocks.size() < nTotal )
    {
      if ( out )
        *out << "\tNot enough OVERVIEW blocks in \"" << descFilename.c_str() << "\"\n";
      return false;
    }

    // Add rows for Test Functions
    for ( int f = 0; f < nTotal; ++f )
    {
      // Prepare global ID of Test Function
      std::string GID = asiAlgo_Utils::Str::ToString( CaseLauncher->CaseID() ) +
                        asiTestEngine_Macro_COLON +
                        asiAlgo_Utils::Str::ToString(f+1);

      // Add table row
      Rdr->StartTableRow()
         ->StartTableCell("table_class cell_class")->AddText(GID)->EndTableCell()
         ->StartTableCell("table_class cell_class")
         ->AddText( overviewBlocks[f] );

      // Add section for details
      if ( ( (int) detailBlocks.size() >= (f+1) ) && detailBlocks[f].length() )
      {
        std::string details = detailBlocks[f];

        Rdr->BreakRow()->BreakRow()
           ->AddText("<i>Details:</i>")
           ->AddText("<div style='border: 1px dotted rgb(100, 100, 100); "
                     "font-size: 11; background-color: rgb(250, 245, 160); "
                     "padding: 5px; margin: 5px;'>")
           ->AddText(details)
           ->AddText("</div>");
      }

      // Finish description cell
      Rdr->EndTableCell();

      // Result of Test Function
      if ( CaseLauncher->IsPassed(f) )
        Rdr->StartTableCell("table_class cell_class good_cell_class")->AddText(asiTestEngine_Macro_OK);
      else
        Rdr->StartTableCell("table_class cell_class bad_cell_class")->AddText(asiTestEngine_Macro_FAILED);

      // Finish row
      Rdr->EndTableCell()->EndTableRow();
    }
  }

  // Finish table
  Rdr->EndTable();

  /* ===============
   *  Render footer
   * =============== */

  Rdr->EndBody()->EndHtml();

  /* ==========================
   *  Prepare filesystem stuff
   * ========================== */

  std::string dirName = std::string("ut_") + this->uniqueDirName();

  if ( out )
    *out << "\tTemporary directory: " << dirName.c_str() << "\n";

  // Prepare full name of the temporary directory
  std::string
    fullDirName = asiAlgo_Utils::Str::Slashed( asiAlgo_Utils::Env::AsiTestDumping() ) + dirName;

  // TODO: for Windows only (!!!)
  // Create directory
  if ( !CreateDirectory(fullDirName.c_str(), NULL) )
  {
    if ( out )
      *out << "\tFailed to create directory: " << fullDirName.c_str() << "\n";
    return false;
  }

  // Filename for HTML report
  std::string
    filename = asiAlgo_Utils::Str::Slashed(fullDirName) +
               asiTestEngine_Macro_REPORT_FN + asiTestEngine_Macro_DOT + asiTestEngine_Macro_REPORT_EXT;

  // Create file for HTML report
  std::ofstream file;
  file.open(filename.c_str(), std::ios::out | std::ios::trunc);
  if ( !file.is_open() )
  {
    if ( out )
      *out << "Cannot open file " << filename.c_str() << " for writing" << "\n";
    return false;
  }

  // Dump rendered information to file
  file << Rdr->Flush();

  // Release file
  file.close();

  return true;
}

//! Generates unique name for the directory containing all results for
//! current test session. The used format is as follows:
//! <pre>
//! ut_{week-day}_{month}_{day}_{{hour}{min}{sec}}_{year}
//!
//! E.g:
//!
//! ut_Sat_Dec_07_190744_2013
//!
//! </pre>
//! \return generated unique name.
std::string asiTestEngine_Launcher::uniqueDirName() const
{
  Handle(asiAlgo_TimeStamp) TS = asiAlgo_TimeStampTool::Generate();
  return TS->ToString(false, true);
}
