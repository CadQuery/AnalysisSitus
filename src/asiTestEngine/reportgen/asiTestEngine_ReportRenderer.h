//-----------------------------------------------------------------------------
// Created on: 26 November 2013
//-----------------------------------------------------------------------------
// Copyright (c) 2013-2018, Sergey Slyadnev
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

#ifndef asiTestEngine_ReportRenderer_HeaderFile
#define asiTestEngine_ReportRenderer_HeaderFile

// asiTestEngine includes
#include <asiTestEngine_ReportStyle.h>
#include <asiTestEngine_ReportTagFactory.h>

// OCCT includes
#include <Standard_Type.hxx>

// Standard includes
#include <string>

//! Utility for generating reports in HTML format. Supplies a predefined set
//! of atomic layout portions to be streamed one-by-one, producing the
//! resulting one-string report's content so.
//!
//! HTML 4.01 in XHTML form is supported.
//!
//! This class provides a mechanism to apply CSS on the HTML tags being
//! generated.
class asiTestEngine_ReportRenderer : public Standard_Transient
{
public:

  //! Convenient shortcut for this.
  typedef asiTestEngine_ReportRenderer* THAT;

  //! Rendering log.
  struct RenderLog
  {
    int NbTables;     //!< Number of rendered tables
    int NbTableRows;  //!< Number of rendered table rows
    int NbTableCells; //!< Number of rendered table cells

    //! Constructor.
    RenderLog()
    {
      this->CleanLog();
    }

    //! Nullifies the logged statistics.
    void CleanLog()
    {
      NbTables = NbTableRows = NbTableCells = 0;
    }
  };

public:

  asiTestEngine_EXPORT
    asiTestEngine_ReportRenderer();

  asiTestEngine_EXPORT
    ~asiTestEngine_ReportRenderer();

public:

  asiTestEngine_EXPORT std::string
    Flush(const bool isLogDropped = false);

  asiTestEngine_EXPORT void
    Reset(const bool isLogDropped = false);

  asiTestEngine_EXPORT bool
    DumpToFile(const std::string& filename);

  asiTestEngine_EXPORT const RenderLog&
    GetRenderLog() const;

// ADD functions:
public:

  asiTestEngine_EXPORT THAT
    AddDoctype();

  asiTestEngine_EXPORT THAT
    AddSymboledText(const char* text,
                    const bool isScaled = true);

  asiTestEngine_EXPORT THAT
    AddErrorText(const char* text);

  asiTestEngine_EXPORT THAT
    AddImportantText(const char* text);

  asiTestEngine_EXPORT THAT
    AddImportantText(const std::string& text);

  asiTestEngine_EXPORT THAT
    AddImportantText(const int num);

  asiTestEngine_EXPORT THAT
    AddImportantText(const double num);

  asiTestEngine_EXPORT THAT
    AddText(const char* text,
            const bool doRemoveNewLines = true);

  asiTestEngine_EXPORT THAT
    AddText(const std::string& text,
            const bool doRemoveNewLines = true);

  asiTestEngine_EXPORT THAT
    AddText(const int num);

  asiTestEngine_EXPORT THAT
    AddText(const double num);

  asiTestEngine_EXPORT THAT
    AddEquality(const char* var,
                const char* value,
                const char* units = 0);

  asiTestEngine_EXPORT THAT
    AddTextWithSubs(const char* text,
                    const char* subs,
                    const bool isTextSymboled = false,
                    const bool isSubsSymboled = false);

  asiTestEngine_EXPORT THAT
    AddMeta();

  asiTestEngine_EXPORT THAT
    AddEmptyTableCell();

  asiTestEngine_EXPORT THAT
    AddEmptyTableHCell();

  asiTestEngine_EXPORT THAT
    AddTableHCell(const char* text);

  asiTestEngine_EXPORT THAT
    AddTableCell(const char* text);

  asiTestEngine_EXPORT THAT
    AddParagraph(const char* text);

  asiTestEngine_EXPORT THAT
    AddHr();

  asiTestEngine_EXPORT THAT
    AddClass(const std::string& dotName,
             const asiTestEngine_ReportStyle& style);

// START/END functions:
public:

  asiTestEngine_EXPORT THAT
    StartHtml();

  asiTestEngine_EXPORT THAT
    EndHtml();

  asiTestEngine_EXPORT THAT
    StartHeader();

  asiTestEngine_EXPORT THAT
    EndHeader();

  asiTestEngine_EXPORT THAT
    StartBody(const asiTestEngine_ReportStyle& style = nullptr);

  asiTestEngine_EXPORT THAT
    StartBody(const std::string& className);

  asiTestEngine_EXPORT THAT
    EndBody();

  asiTestEngine_EXPORT THAT
    StartTable(const asiTestEngine_ReportStyle& style = nullptr);

  asiTestEngine_EXPORT THAT
    StartTable(const std::string& className);

  asiTestEngine_EXPORT THAT
    EndTable();

  asiTestEngine_EXPORT THAT
    StartTableRow(const asiTestEngine_ReportStyle& style = 0);

  asiTestEngine_EXPORT THAT
    StartTableRow(const std::string& className);

  asiTestEngine_EXPORT THAT
    EndTableRow();

  asiTestEngine_EXPORT THAT
    StartTableCell(const asiTestEngine_ReportStyle& style = 0);

  asiTestEngine_EXPORT THAT
    StartTableCell(const std::string& className);

  asiTestEngine_EXPORT THAT
    StartColSpanTableCell(const int colSpan = 1,
                          const asiTestEngine_ReportStyle& theStyle = 0);

  asiTestEngine_EXPORT THAT
    StartColSpanTableCell(const int colSpan,
                          const std::string& className);

  asiTestEngine_EXPORT THAT
    StartColSpanTableHCell(const int colSpan = 1,
                           const asiTestEngine_ReportStyle& style = 0);

  asiTestEngine_EXPORT THAT
    StartColSpanTableHCell(const int colSpan,
                           const std::string& className);

  asiTestEngine_EXPORT THAT
    StartRowSpanTableCell(const int rowSpan = 1,
                          const asiTestEngine_ReportStyle& style = 0);

  asiTestEngine_EXPORT THAT
    StartRowSpanTableCell(const int rowSpan,
                          const std::string& className);

  asiTestEngine_EXPORT THAT
    StartRowSpanTableHCell(const int rowSpan = 1,
                           const asiTestEngine_ReportStyle& style = 0);

  asiTestEngine_EXPORT THAT
    StartRowSpanTableHCell(const int rowSpan,
                           const std::string& className);

  asiTestEngine_EXPORT THAT
    EndTableCell();

  asiTestEngine_EXPORT THAT
    StartTableHCell(const asiTestEngine_ReportStyle& style = 0);

  asiTestEngine_EXPORT THAT
    StartTableHCell(const std::string& className);

  asiTestEngine_EXPORT THAT
    EndTableHCell();

  asiTestEngine_EXPORT THAT
    StartParagraph(const asiTestEngine_ReportStyle& style = 0);

  asiTestEngine_EXPORT THAT
    StartParagraph(const std::string& className);

  asiTestEngine_EXPORT THAT
    EndParagraph();

  asiTestEngine_EXPORT THAT
    StartSubscript();

  asiTestEngine_EXPORT THAT
    EndSubscript();

  asiTestEngine_EXPORT THAT
    StartH1();

  asiTestEngine_EXPORT THAT
    EndH1();

  asiTestEngine_EXPORT THAT
    StartStyle();

  asiTestEngine_EXPORT THAT
    EndStyle();

  asiTestEngine_EXPORT THAT
    StartCenter();

  asiTestEngine_EXPORT THAT
    EndCenter();

// Additional functions:
public:

  asiTestEngine_EXPORT THAT
    BreakRow();

private:

  static void
    removeInconsistent(std::string& str);

private:

  //! Tag factory to generate HTML mark-up.
  asiTestEngine_ReportTagFactory m_tagFactory;

  //! Rendering buffer.
  std::string m_buffer;

  //! Rendering log.
  RenderLog m_log;

};

#endif
