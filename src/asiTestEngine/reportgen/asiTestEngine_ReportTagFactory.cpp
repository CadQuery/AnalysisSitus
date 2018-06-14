//-----------------------------------------------------------------------------
// Created on: 26 November 2013
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

// Own include
#include <asiTestEngine_ReportTagFactory.h>

//! Default constructor.
asiTestEngine_ReportTagFactory::asiTestEngine_ReportTagFactory()
{
}

//! Destructor.
asiTestEngine_ReportTagFactory::~asiTestEngine_ReportTagFactory()
{
}

//! Returns HTML tag, opening or closing, depending on the passed
//! parameter.
//! \param isOpen [in] indicates whether the resulting tag is opening or
//!        closing.
//! \return requested tag.
asiTestEngine_ReportTag& asiTestEngine_ReportTagFactory::Html(const bool isOpen)
{
  setPairedTag(m_tag, "html", isOpen);
  return m_tag;
}

//! Returns HEAD tag, opening or closing, depending on the passed
//! parameter.
//! \param isOpen [in] indicates whether the resulting tag is opening or
//!        closing.
//! \return requested tag.
asiTestEngine_ReportTag& asiTestEngine_ReportTagFactory::Head(const bool isOpen)
{
  setPairedTag(m_tag, "head", isOpen);
  return m_tag;
}

//! Returns META tag.
//! \return requested tag.
asiTestEngine_ReportTag& asiTestEngine_ReportTagFactory::Meta()
{
  setUnpairedTag(m_tag, "meta");
  return m_tag;
}

//! Returns BODY tag, opening or closing, depending on the passed
//! parameter.
//! \param isOpen [in] indicates whether the resulting tag is opening or
//!        closing.
//! \return requested tag.
asiTestEngine_ReportTag& asiTestEngine_ReportTagFactory::Body(const bool isOpen)
{
  setPairedTag(m_tag, "body", isOpen);
  return m_tag;
}

//! Returns P tag, opening or closing, depending on the passed
//! parameter.
//! \param isOpen [in] indicates whether the resulting tag is opening or
//!        closing.
//! \return requested tag.
asiTestEngine_ReportTag& asiTestEngine_ReportTagFactory::P(const bool isOpen)
{
  setPairedTag(m_tag, "p", isOpen);
  return m_tag;
}

//! Returns B tag, opening or closing, depending on the passed
//! parameter.
//! \param isOpen [in] indicates whether the resulting tag is opening or
//!        closing.
//! \return requested tag.
asiTestEngine_ReportTag& asiTestEngine_ReportTagFactory::B(const bool isOpen)
{
  setPairedTag(m_tag, "b", isOpen);
  return m_tag;
}

//! Returns SUB tag, opening or closing, depending on the passed
//! parameter.
//! \param isOpen [in] indicates whether the resulting tag is opening or
//!        closing.
//! \return requested tag.
asiTestEngine_ReportTag& asiTestEngine_ReportTagFactory::Sub(const bool isOpen)
{
  setPairedTag(m_tag, "sub", isOpen);
  return m_tag;
}

//! Returns BR tag.
//! \return requested tag.
asiTestEngine_ReportTag& asiTestEngine_ReportTagFactory::Br()
{
  setUnpairedTag(m_tag, "br");
  return m_tag;
}

//! Returns TABLE tag, opening or closing, depending on the passed
//! parameter.
//! \param isOpen [in] indicates whether the resulting tag is opening or
//!        closing.
//! \return requested tag.
asiTestEngine_ReportTag& asiTestEngine_ReportTagFactory::Table(const bool isOpen)
{
  setPairedTag(m_tag, "table", isOpen);
  return m_tag;
}

//! Returns TBODY tag, opening or closing, depending on the passed
//! parameter.
//! \param isOpen [in] indicates whether the resulting tag is opening or
//!        closing.
//! \return requested tag.
asiTestEngine_ReportTag& asiTestEngine_ReportTagFactory::TBody(const bool isOpen)
{
  setPairedTag(m_tag, "tbody", isOpen);
  return m_tag;
}

//! Returns TR tag, opening or closing, depending on the passed
//! parameter.
//! \param isOpen [in] indicates whether the resulting tag is opening or
//!        closing.
//! \return requested tag.
asiTestEngine_ReportTag& asiTestEngine_ReportTagFactory::Tr(const bool isOpen)
{
  setPairedTag(m_tag, "tr", isOpen);
  return m_tag;
}

//! Returns TD tag, opening or closing, depending on the passed
//! parameter.
//! \param isOpen [in] indicates whether the resulting tag is opening or
//!        closing.
//! \return requested tag.
asiTestEngine_ReportTag& asiTestEngine_ReportTagFactory::Td(const bool isOpen)
{
  setPairedTag(m_tag, "td", isOpen);
  return m_tag;
}

//! Returns TH tag, opening or closing, depending on the passed
//! parameter.
//! \param isOpen [in] indicates whether the resulting tag is opening or
//!        closing.
//! \return requested tag.
asiTestEngine_ReportTag& asiTestEngine_ReportTagFactory::Th(const bool isOpen)
{
  setPairedTag(m_tag, "th", isOpen);
  return m_tag;
}

//! Returns FONT tag, opening or closing, depending on the passed
//! parameter.
//! \param isOpen [in] indicates whether the resulting tag is opening or
//!        closing.
//! \return requested tag.
asiTestEngine_ReportTag& asiTestEngine_ReportTagFactory::Font(const bool isOpen)
{
  setPairedTag(m_tag, "font", isOpen);
  return m_tag;
}

//! Returns H1 tag, opening or closing, depending on the passed
//! parameter.
//! \param isOpen [in] indicates whether the resulting tag is opening or
//!        closing.
//! \return requested tag.
asiTestEngine_ReportTag& asiTestEngine_ReportTagFactory::H1(const bool isOpen)
{
  setPairedTag(m_tag, "h1", isOpen);
  return m_tag;
}

//! Returns H2 tag, opening or closing, depending on the passed
//! parameter.
//! \param isOpen [in] indicates whether the resulting tag is opening or
//!        closing.
//! \return requested tag.
asiTestEngine_ReportTag& asiTestEngine_ReportTagFactory::H2(const bool isOpen)
{
  setPairedTag(m_tag, "h2", isOpen);
  return m_tag;
}

//! Returns H3 tag, opening or closing, depending on the passed
//! parameter.
//! \param isOpen [in] indicates whether the resulting tag is opening or
//!        closing.
//! \return requested tag.
asiTestEngine_ReportTag& asiTestEngine_ReportTagFactory::H3(const bool isOpen)
{
  setPairedTag(m_tag, "h3", isOpen);
  return m_tag;
}

//! Returns H4 tag, opening or closing, depending on the passed
//! parameter.
//! \param isOpen [in] indicates whether the resulting tag is opening or
//!        closing.
//! \return requested tag.
asiTestEngine_ReportTag& asiTestEngine_ReportTagFactory::H4(const bool isOpen)
{
  setPairedTag(m_tag, "h4", isOpen);
  return m_tag;
}

//! Returns HR tag.
//! \return requested tag.
asiTestEngine_ReportTag& asiTestEngine_ReportTagFactory::Hr()
{
  setUnpairedTag(m_tag, "hr");
  return m_tag;
}

//! Returns Style tag, opening or closing, depending on the passed
//! parameter.
//! \param isOpen [in] indicates whether the resulting tag is opening or
//!        closing.
//! \return requested tag.
asiTestEngine_ReportTag& asiTestEngine_ReportTagFactory::Style(const bool isOpen)
{
  setPairedTag(m_tag, "style", isOpen);
  return m_tag;
}

//! Returns Center tag, opening or closing, depending on the passed
//! parameter.
//! \param isOpen [in] indicates whether the resulting tag is opening or
//!        closing.
//! \return requested tag.
asiTestEngine_ReportTag& asiTestEngine_ReportTagFactory::Center(const bool isOpen)
{
  setPairedTag(m_tag, "center", isOpen);
  return m_tag;
}

//! Prepares the passed tag as a paired one with the given name and
//! openness indicator.
//! \param tag [in/out] tag to prepare.
//! \param baseName [in] base name to set for the tag being prepared.
//! \param isOpen [in] indicates whether the tag being prepared is opening
//!        or closing.
void asiTestEngine_ReportTagFactory::setPairedTag(asiTestEngine_ReportTag& tag,
                                                  const char* baseName,
                                                  const bool isOpen)
{
  tag.Release();
  tag.SetIsPaired(true);
  tag.SetIsClosed(!isOpen);
  tag.SetBase(baseName);
}

//! Prepares the passed tag as single (non-paired) one with the given name.
//! \param tag [in/out] tag to prepare.
//! \param baseName [in] base name to set for the tag being prepared.
void asiTestEngine_ReportTagFactory::setUnpairedTag(asiTestEngine_ReportTag& tag,
                                                    const char* baseName)
{
  tag.Release();
  tag.SetIsPaired(false);
  tag.SetBase(baseName);
}
