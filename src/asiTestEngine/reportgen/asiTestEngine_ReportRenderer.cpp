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

#include <asiTestEngine_ReportRenderer.h>

// Core includes
#include <asiAlgo_FileDumper.h>
#include <asiAlgo_Utils.h>

//! Default constructor.
asiTestEngine_ReportRenderer::asiTestEngine_ReportRenderer() : Standard_Transient()
{}

//! Destructor.
asiTestEngine_ReportRenderer::~asiTestEngine_ReportRenderer()
{}

//! Returns the rendering buffer contents and cleans up the buffer.
//! \param isLogDropped [in] indicates whether the rendering log must be
//!        cleaned up as well.
//! \return buffer contents.
std::string asiTestEngine_ReportRenderer::Flush(const bool isLogDropped)
{
  std::string result(m_buffer);
  this->Reset(isLogDropped);
  return result;
}

//! Cleans up the rendering buffer.
//! \param isLogDropped [in] indicates whether the rendering log must be
//!        cleaned up as well.
void asiTestEngine_ReportRenderer::Reset(const bool isLogDropped)
{
  if ( isLogDropped )
    m_log.CleanLog();

  m_buffer.clear();
}

//! Dumps the contents of the rendering buffer to file. Does not reset
//! the buffer.
//! \param filename [in] filename.
//! \return true if the buffer is dumped successfully, false -- otherwise.
bool asiTestEngine_ReportRenderer::DumpToFile(const std::string& filename)
{
  asiAlgo_FileDumper Dumper;
  if ( !Dumper.Open(filename) )
    return false;

  Dumper.Dump(m_buffer);
  return true;
}

//! Accessor for rendering log.
//! \return rendering log.
const asiTestEngine_ReportRenderer::RenderLog&
  asiTestEngine_ReportRenderer::GetRenderLog() const
{
  return m_log;
}

//! Adds DocType information to buffer.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT asiTestEngine_ReportRenderer::AddDoctype()
{
  m_buffer.append("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">");
  return this;
}

//! Adds text with 'Symbol' font presentation. Internally uses AddText
//! method, so be aware of the related cautions.
//! \param text [in] text to add.
//! \param isScaled [in] indicates whether the text size must be scaled up.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::AddSymboledText(const char* text,
                                                const bool isScaled)
{
  // TODO: use CSS styling rather than not recommended 'face' attribute
  asiTestEngine_ReportTag& fontTag = m_tagFactory.Font();
  fontTag.AddAttribute("face", "Symbol");

  if ( isScaled )
    fontTag.AddAttribute("size", "+1");

  m_buffer.append( fontTag.Result() );
  this->AddText(text);
  m_buffer.append( m_tagFactory.Font(false).Result() );
  return this;
}

//! Adds text marked as error.
//! \param text [in] text to add.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::AddErrorText(const char* text)
{
  // TODO: use CSS styling rather than not recommended 'color' attribute
  asiTestEngine_ReportTag& fontTag = m_tagFactory.Font();
  fontTag.AddAttribute("color", "red");

  m_buffer.append( fontTag.Result() );
  this->AddText(text);
  m_buffer.append( m_tagFactory.Font(false).Result() );
  return this;
}

//! Adds text marked as important (normally, bold style is used for such cases).
//! Internally uses AddText method, so be aware of the related cautions.
//! \param text [in] text to add.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::AddImportantText(const char* text)
{
  // TODO: use CSS styling rather than not recommended 'b' tag
  m_buffer.append( m_tagFactory.B().Result() );
  this->AddText(text);
  m_buffer.append( m_tagFactory.B(false).Result() );
  return this;
}

//! Overloaded method for QString.
//! \param text [in] text to render.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::AddImportantText(const std::string& text)
{
  return this->AddImportantText( text.c_str() );
}

//! Overloaded method for integer.
//! \param num [in] number to render.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::AddImportantText(const int num)
{
  return this->AddImportantText( asiAlgo_Utils::Str::ToString(num) );
}

//! Overloaded method for floating-point value.
//! \param num [in] number to render.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::AddImportantText(const double num)
{
  return this->AddImportantText( asiAlgo_Utils::Str::ToString(num) );
}

//! Common method for pushing an arbitrary text to buffer. Note, that this
//! routine is a 'gap' for adding custom layout tags to the resulting
//! string. While it makes the rendering technique more flexible (you can,
//! for instance, store some mark-up in resource files and push it as-is
//! here), you should carefully handle this possibility, being sure that
//! custom tags are well-formed and can co-exist correctly with their
//! surrounding tags.
//! \param text [in] text to add.
//! \param doRemoveNewLines [in] indicates whether new line characters must
//!        be removed or not.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::AddText(const char* text,
                                        const bool  doRemoveNewLines)
{
  std::string textToAdd(text);
  if ( doRemoveNewLines )
    this->removeInconsistent(textToAdd);

  m_buffer.append(textToAdd);
  return this;
}

//! Overloaded method for std::string.
//! \param text [in] text to render.
//! \param doRemoveNewLines [in] indicates whether new line characters must
//!        be removed or not.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::AddText(const std::string& text,
                                        const bool doRemoveNewLines)
{
  return this->AddText( text.c_str(), doRemoveNewLines );
}

//! Overloaded method for integer.
//! \param num [in] value to render.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::AddText(const int num)
{
  return this->AddText( asiAlgo_Utils::Str::ToString(num), false );
}

//! Overloaded method for double.
//! \param num [in] value to render.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::AddText(const double num)
{
  return this->AddText( asiAlgo_Utils::Str::ToString(num), false );
}

//! Renders equality expression in the following format:
//! /TAB/theVar/TAB/=/TAB/theValue/TAB/[theUnits]/TAB/
//! \param var [in] name of the variable.
//! \param value [in] value of the variable.
//! \param units [in] optional units to append.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::AddEquality(const char* var,
                                            const char* value,
                                            const char* units)
{
  this->AddText(var)
      ->AddText("\t")->AddText("=")
      ->AddText("\t")->AddText(value);

  units ? this->AddText("\t")->AddText(units)->AddText("\t") :
          this->AddText("\t");

  return this;
}

//! Renders the passed text followed by the passed subscript.
//! \param text [in] text to render.
//! \param subs [in] subscript to append.
//! \param isTextSymboled [in] indicates whether the passed text must be
//!        rendered in Greek symbols.
//! \param isSubsSymboled [in] indicates whether the passed subscript must
//!        be rendered in Greek symbols.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::AddTextWithSubs(const char* text,
                                                const char* subs,
                                                const bool  isTextSymboled,
                                                const bool  isSubsSymboled)
{
  isTextSymboled ? this->AddSymboledText(text) : this->AddText(text);

  this->StartSubscript();
  isSubsSymboled ? this->AddSymboledText(subs) : this->AddText(subs);
  this->EndSubscript();
  
  return this;
}

//! Adds meta-information to the rendering buffer.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT asiTestEngine_ReportRenderer::AddMeta()
{
  asiTestEngine_ReportTag& tag = m_tagFactory.Meta();
  tag.AddAttribute("name", "GENERATOR");
  tag.AddAttribute("content", "Mobius HTML Generator");
  m_buffer.append( tag.Result() );
  return this;
}

//! Adds table's empty cell.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::AddEmptyTableCell()
{
  return this->StartTableCell()->AddText(" ")->EndTableCell();
}

//! Adds table header's empty cell.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::AddEmptyTableHCell()
{
  return this->StartTableHCell()->AddText(" ")->EndTableHCell();
}

//! Adds table header's cell with the passed text within.
//! \param text [in] text to add into the rendered cell.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::AddTableHCell(const char* text)
{
  return this->StartTableHCell()->AddText(text)->EndTableHCell();
}

//! Adds table's cell with the passed text within.
//! \param text [in] text to add into the rendered cell.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::AddTableCell(const char* text)
{
  return this->StartTableCell()->AddText(text)->EndTableCell();
}

//! Convenient shortcut pushing a paragraph with some nested text.
//! \param text [in] text to present as a paragraph.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::AddParagraph(const char* text)
{
  return this->StartParagraph()->AddText(text)->EndParagraph();
}

//! Adds HR tag.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT asiTestEngine_ReportRenderer::AddHr()
{
  m_buffer.append( m_tagFactory.Hr().Result() );
  return this;
}

//! Adds CSS class definition. Please note that this method should be
//! normally invoked when rendering cursor is located within STYLE
//! HTML container.
//! \param dotName [in] CSS class name.
//! \param style [in] style description.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::AddClass(const std::string& dotName,
                                         const asiTestEngine_ReportStyle& style)
{
  std::string csDesc = std::string(".") + dotName + std::string(" {")
                  + style.MakeDescriptor() + std::string("}");
  m_buffer.append(csDesc);
  return this;
}

//! Inserts starting HTML tag.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT asiTestEngine_ReportRenderer::StartHtml()
{
  m_buffer.append( m_tagFactory.Html().Result() );
  return this;
}

//! Inserts ending HTML tag.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT asiTestEngine_ReportRenderer::EndHtml()
{
  m_buffer.append( m_tagFactory.Html(false).Result() );
  return this;
}

//! Inserts starting HEAD tag.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT asiTestEngine_ReportRenderer::StartHeader()
{
  m_buffer.append( m_tagFactory.Head().Result() );
  return this;
}

//! Inserts ending HEAD tag.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT asiTestEngine_ReportRenderer::EndHeader()
{
  m_buffer.append( m_tagFactory.Head(false).Result() );
  return this;
}

//! Inserts starting BODY tag with the passed styling.
//! \param style [in] styling to apply.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::StartBody(const asiTestEngine_ReportStyle& style)
{
  // Generate tag
  asiTestEngine_ReportTag& tag = m_tagFactory.Body();

  if ( !style.IsNull() )
    style.ApplyStyles(tag);

  // Add tag to buffer
  m_buffer.append( tag.Result() );
  return this;
}

//! Inserts starting BODY tag with the passed CSS class for styling.
//! \param className [in] CSS class.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::StartBody(const std::string& className)
{
  asiTestEngine_ReportTag& tag = m_tagFactory.Body();
  tag.AddAttribute("class", className);
  m_buffer.append( tag.Result() );
  return this;
}

//! Inserts ending BODY tag.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT asiTestEngine_ReportRenderer::EndBody()
{
  m_buffer.append( m_tagFactory.Body(false).Result() );
  return this;
}

//! Inserts starting TABLE tag with the passed styling.
//! \param style [in] styling to apply.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::StartTable(const asiTestEngine_ReportStyle& style)
{
  m_log.NbTables++;
  asiTestEngine_ReportTag& tag = m_tagFactory.Table();

  if ( !style.IsNull() )
    style.ApplyStyles(tag);

  m_buffer.append( tag.Result() );
  return this;
}

//! Inserts starting TABLE tag with the passed CSS class for styling.
//! \param className [in] CSS class.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::StartTable(const std::string& className)
{
  m_log.NbTables++;
  asiTestEngine_ReportTag& tag = m_tagFactory.Table();
  tag.AddAttribute("class", className);
  m_buffer.append( tag.Result() );
  return this;
}

//! Inserts ending TABLE tag.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT asiTestEngine_ReportRenderer::EndTable()
{
  m_buffer.append( m_tagFactory.Table(false).Result() );
  return this;
}

//! Inserts starting TR tag with the passed styling.
//! \param style [in] styling to apply.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::StartTableRow(const asiTestEngine_ReportStyle& style)
{
  m_log.NbTableRows++;
  asiTestEngine_ReportTag& tag = m_tagFactory.Tr();

  if ( !style.IsNull() )
    style.ApplyStyles(tag);

  m_buffer.append( tag.Result() );
  return this;
}

//! Inserts starting TR tag with the passed CSS class for styling.
//! \param className [in] CSS class.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::StartTableRow(const std::string& className)
{
  m_log.NbTableRows++;
  asiTestEngine_ReportTag& tag = m_tagFactory.Tr();
  tag.AddAttribute("class", className);
  m_buffer.append( tag.Result() );
  return this;
}

//! Inserts ending TR tag.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT asiTestEngine_ReportRenderer::EndTableRow()
{
  m_buffer.append( m_tagFactory.Tr(false).Result() );
  return this;
}

//! Inserts starting TD tag with the passed styling.
//! \param style [in] styling to apply.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::StartTableCell(const asiTestEngine_ReportStyle& style)
{
  m_log.NbTableCells++;
  asiTestEngine_ReportTag& tag = m_tagFactory.Td();

  if ( !style.IsNull() )
    style.ApplyStyles(tag);

  m_buffer.append( tag.Result() );
  return this;
}

//! Inserts starting TD tag with the passed CSS class for styling.
//! \param className [in] CSS class.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::StartTableCell(const std::string& className)
{
  m_log.NbTableCells++;
  asiTestEngine_ReportTag& tag = m_tagFactory.Td();
  tag.AddAttribute("class", className);
  m_buffer.append( tag.Result() );
  return this;
}

//! Inserts starting TD tag with the passed styling and COLSPAN attribute.
//! \param colSpan [in] colSpan value to apply.
//! \param style [in] styling to apply.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::StartColSpanTableCell(const int colSpan,
                                                      const asiTestEngine_ReportStyle& style)
{
  m_log.NbTableCells++;
  asiTestEngine_ReportTag& tag = m_tagFactory.Td();

  if ( !style.IsNull() )
    style.ApplyStyles(tag);

  tag.AddAttribute( "colspan", asiAlgo_Utils::Str::ToString(colSpan) );
  m_buffer.append( tag.Result() );
  return this;
}

//! Inserts starting TD tag with the passed CSS class for styling
//! and COLSPAN attribute.
//! \param colSpan [in] COLSPAN value to apply.
//! \param className [in] CSS class.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::StartColSpanTableCell(const int colSpan,
                                                      const std::string& className)
{
  m_log.NbTableCells++;
  asiTestEngine_ReportTag& tag = m_tagFactory.Td();
  tag.AddAttribute("class", className);
  tag.AddAttribute( "colspan", asiAlgo_Utils::Str::ToString(colSpan) );
  m_buffer.append( tag.Result() );
  return this;
}

//! Inserts starting TH tag with the passed styling and COLSPAN attribute.
//! \param colSpan [in] COLSPAN value to apply.
//! \param style [in] styling to apply.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::StartColSpanTableHCell(const int colSpan,
                                                       const asiTestEngine_ReportStyle& style)
{
  m_log.NbTableCells++;
  asiTestEngine_ReportTag& tag = m_tagFactory.Th();

  if ( !style.IsNull() )
    style.ApplyStyles(tag);

  tag.AddAttribute( "colspan", asiAlgo_Utils::Str::ToString(colSpan) );
  m_buffer.append( tag.Result() );
  return this;
}

//! Inserts starting TH tag with the passed CSS class for styling
//! and COLSPAN attribute.
//! \param colSpan [in] COLSPAN value to apply.
//! \param className [in] CSS class.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::StartColSpanTableHCell(const int colSpan,
                                                       const std::string& className)
{
  m_log.NbTableCells++;
  asiTestEngine_ReportTag& tag = m_tagFactory.Th();
  tag.AddAttribute("class", className);
  tag.AddAttribute( "colspan", asiAlgo_Utils::Str::ToString(colSpan) );
  m_buffer.append( tag.Result() );
  return this;
}

//! Inserts starting TD tag with the passed styling and ROWSPAN attribute.
//! \param rowSpan [in] ROWSPAN value to apply.
//! \param style [in] styling to apply.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::StartRowSpanTableCell(const int rowSpan,
                                                      const asiTestEngine_ReportStyle& style)
{
  m_log.NbTableCells++;
  asiTestEngine_ReportTag& tag = m_tagFactory.Td();

  if ( !style.IsNull() )
    style.ApplyStyles(tag);

  tag.AddAttribute( "rowspan", asiAlgo_Utils::Str::ToString(rowSpan) );
  m_buffer.append( tag.Result() );
  return this;
}

//! Inserts starting TD tag with the passed CSS class for styling
//! and ROWSPAN attribute.
//! \param rowSpan [in] ROWSPAN value to apply.
//! \param className [in] CSS class.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::StartRowSpanTableCell(const int rowSpan,
                                                      const std::string& className)
{
  m_log.NbTableCells++;
  asiTestEngine_ReportTag& tag = m_tagFactory.Td();
  tag.AddAttribute("class", className);
  tag.AddAttribute( "rowspan", asiAlgo_Utils::Str::ToString(rowSpan) );
  m_buffer.append( tag.Result() );
  return this;
}

//! Inserts starting TH tag with the passed styling and ROWSPAN attribute.
//! \param rowSpan [in] ROWSPAN value to apply.
//! \param style [in] styling to apply.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::StartRowSpanTableHCell(const int rowSpan,
                                                       const asiTestEngine_ReportStyle& style)
{
  m_log.NbTableCells++;
  asiTestEngine_ReportTag& tag = m_tagFactory.Th();

  if ( !style.IsNull() )
    style.ApplyStyles(tag);

  tag.AddAttribute( "rowspan", asiAlgo_Utils::Str::ToString(rowSpan) );
  m_buffer.append( tag.Result() );
  return this;
}

//! Inserts starting TH tag with the passed CSS class for styling
//! and ROWSPAN attribute.
//! \param rowSpan [in] ROWSPAN value to apply.
//! \param className [in] styling class.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::StartRowSpanTableHCell(const int rowSpan,
                                                       const std::string& className)
{
  m_log.NbTableCells++;
  asiTestEngine_ReportTag& tag = m_tagFactory.Th();
  tag.AddAttribute("class", className);
  tag.AddAttribute( "rowspan", asiAlgo_Utils::Str::ToString(rowSpan) );
  m_buffer.append( tag.Result() );
  return this;
}

//! Inserts ending TD tag.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT asiTestEngine_ReportRenderer::EndTableCell()
{
  m_buffer.append( m_tagFactory.Td(false).Result() );
  return this;
}

//! Inserts staring TH tag with the passed styling.
//! \param style [in] styling to apply.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::StartTableHCell(const asiTestEngine_ReportStyle& style)
{
  m_log.NbTableCells++;
  asiTestEngine_ReportTag& tag = m_tagFactory.Th();

  if ( !style.IsNull() )
    style.ApplyStyles(tag);

  m_buffer.append( tag.Result() );
  return this;
}

//! Inserts staring TH tag with the passed CSS style class.
//! \param className [in] CSS class to apply.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::StartTableHCell(const std::string& className)
{
  m_log.NbTableCells++;
  asiTestEngine_ReportTag& tag = m_tagFactory.Th();
  tag.AddAttribute( "class", className );
  m_buffer.append( tag.Result() );
  return this;
}

//! Inserts ending TH tag.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT asiTestEngine_ReportRenderer::EndTableHCell()
{
  m_buffer.append( m_tagFactory.Th(false).Result() );
  return this;
}

//! Inserts staring P tag with the passed styling.
//! \param style [in] styling to apply.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::StartParagraph(const asiTestEngine_ReportStyle& style)
{
  asiTestEngine_ReportTag& tag = m_tagFactory.P();

  if ( !style.IsNull() )
    style.ApplyStyles(tag);

  m_buffer.append( tag.Result() );
  return this;
}

//! Inserts staring P tag with the passed CSS style class.
//! \param className [in] styling class to apply.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT
  asiTestEngine_ReportRenderer::StartParagraph(const std::string& className)
{
  asiTestEngine_ReportTag& tag = m_tagFactory.P();
  tag.AddAttribute( "class", className );
  m_buffer.append( tag.Result() );
  return this;
}

//! Inserts ending P tag.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT asiTestEngine_ReportRenderer::EndParagraph()
{
  m_buffer.append( m_tagFactory.P(false).Result() );
  return this;
}

//! Inserts staring SUB tag.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT asiTestEngine_ReportRenderer::StartSubscript()
{
  m_buffer.append( m_tagFactory.Sub().Result() );
  return this;
}

//! Inserts ending SUB tag.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT asiTestEngine_ReportRenderer::EndSubscript()
{
  m_buffer.append( m_tagFactory.Sub(false).Result() );
  return this;
}

//! Inserts staring H1 tag.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT asiTestEngine_ReportRenderer::StartH1()
{
  m_buffer.append( m_tagFactory.H1().Result() );
  return this;
}

//! Inserts ending H1 tag.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT asiTestEngine_ReportRenderer::EndH1()
{
  m_buffer.append( m_tagFactory.H1(false).Result() );
  return this;
}

//! Inserts staring style tag.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT asiTestEngine_ReportRenderer::StartStyle()
{
  m_buffer.append( m_tagFactory.Style().Result() );
  return this;
}

//! Inserts ending style tag.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT asiTestEngine_ReportRenderer::EndStyle()
{
  m_buffer.append( m_tagFactory.Style(false).Result() );
  return this;
}

//! Inserts staring center tag.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT asiTestEngine_ReportRenderer::StartCenter()
{
  m_buffer.append( m_tagFactory.Center().Result() );
  return this;
}

//! Inserts ending center tag.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT asiTestEngine_ReportRenderer::EndCenter()
{
  m_buffer.append( m_tagFactory.Center(false).Result() );
  return this;
}

//! Inserts BR tag to add new line to the rendered report.
//! \return THIS reference for streaming.
asiTestEngine_ReportRenderer::THAT asiTestEngine_ReportRenderer::BreakRow()
{
  m_buffer.append( m_tagFactory.Br().Result() );
  return this;
}

//! Removes inconsistent characters from the passed string. Used (optionally)
//! by AddText() method to prevent corrupting of the layout with some
//! unacceptable entries (e.g. newline characters).
//! \param str [in/out] string to adjust.
void asiTestEngine_ReportRenderer::removeInconsistent(std::string& str)
{
  asiAlgo_Utils::Str::ReplaceAll(str, "\n", "");
  asiAlgo_Utils::Str::ReplaceAll(str, "\r", "");
}
