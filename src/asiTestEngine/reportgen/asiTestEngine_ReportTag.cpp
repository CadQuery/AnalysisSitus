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

#include <asiTestEngine_ReportTag.h>

//! Default constructor.
asiTestEngine_ReportTag::asiTestEngine_ReportTag()
{
  this->Release();
}

//! Destructor.
asiTestEngine_ReportTag::~asiTestEngine_ReportTag()
{
}

//! Cleans up the tag's internal data (base name, attributes and styles).
void asiTestEngine_ReportTag::Release()
{
  m_bIsInitialized = m_bIsStyled = m_bIsPaired = m_bIsClosed = false;

  this->eraseString(m_base);
  this->eraseString(m_style);
  this->eraseStrings(m_attributes);
}

//! Sets a flag indicating whether this tag is paired or not.
//! \param isPaired [in] value to set.
void asiTestEngine_ReportTag::SetIsPaired(const bool isPaired)
{
  m_bIsPaired = isPaired;

  if ( !m_bIsPaired && !m_bIsClosed )
    m_bIsClosed = true;
}

//! Returns a Boolean value indicating whether this tag is paired or not.
//! \return true/false.
bool asiTestEngine_ReportTag::IsPaired() const
{
  return m_bIsPaired;
}

//! Sets a flag indicating whether this tag is closed or not.
//! \param isClosed [in] value to set.
void asiTestEngine_ReportTag::SetIsClosed(const bool isClosed)
{
  m_bIsClosed = isClosed;
}

//! Returns a Boolean value indicating whether this tag is closed or not.
//! \return true/false.
bool asiTestEngine_ReportTag::IsClosed() const
{
  return m_bIsClosed;
}

//! Sets a base name for the tag instance.
//! \param base [in] value to set.
void asiTestEngine_ReportTag::SetBase(const char* base)
{
  m_base = base;
  m_bIsInitialized = true;
}

//! Sets a base name for the tag instance.
//! \param base [in] value to set.
void asiTestEngine_ReportTag::SetBase(const std::string& base)
{
  this->SetBase( base.c_str() );
}

//! Assigns a style (name, value) pair to the tag.
//! \param styleName [in] style name.
//! \param styleValue [in] style value.
void asiTestEngine_ReportTag::AddStyle(const char* styleName,
                                       const char* styleValue)
{
  if ( !m_bIsStyled )
    m_style = "style=''";

  std::string stStart(m_bIsStyled ? " " : ""), stName(styleName), stValue(styleValue);
  std::string stRecord = stStart + stName + ": " + stValue + ";";

  const size_t pos = m_style.find("'") + 1;
  m_style.insert(pos, stRecord);

  m_bIsStyled = true;
}

//! Assigns a style (name, value) pair to the tag.
//! \param styleName [in] style name.
//! \param styleValue [in] style value.
void asiTestEngine_ReportTag::AddStyle(const std::string& styleName,
                                       const std::string& styleValue)
{
  this->AddStyle( styleName.c_str(), styleValue.c_str() );
}

//! Assigns an attribute (name, value) pair with the tag.
//! \param attrName [in] attribute name.
//! \param attrValue [in] attribute value.
void asiTestEngine_ReportTag::AddAttribute(const char* attrName,
                                           const char* attrValue)
{
  std::string attrRecord = std::string(attrName) + "='" + std::string(attrValue) + "'";
  m_attributes.push_back(attrRecord);
}

//! Assigns an attribute (name, value) pair with the tag.
//! \param attrName [in] attribute name.
//! \param attrValue [in] attribute value.
void asiTestEngine_ReportTag::AddAttribute(const std::string& attrName,
                                           const std::string& attrValue)
{
  this->AddAttribute( attrName.c_str(), attrValue.c_str() );
}

//! Returns resulting tag as a string.
//! \return string representing the tag.
std::string asiTestEngine_ReportTag::Result() const
{
  if ( !m_bIsInitialized )
    return std::string();

  // Start the tag
  std::string result = (m_bIsPaired && m_bIsClosed ? "</" : "<");

  // Add base
  result += m_base;

  if ( !m_bIsClosed || (m_bIsClosed && !m_bIsPaired) )
  {
    // Add attributes from the stored list
    for ( int i = 0; i < (int) m_attributes.size(); ++i )
      result += std::string(" ").append( m_attributes.at(i) );

    // Add styles
    if ( m_bIsStyled )
      result += std::string(" ").append(m_style);
  }

  // End of the tag
  result += std::string(m_bIsPaired ? ">" : "/>");

  return result;
}

//! Cleans up the passed string.
//! \param string [in/out] string to clean up.
void asiTestEngine_ReportTag::eraseString(std::string& string)
{
  string.clear();
}

//! Cleans up the passed list of strings.
//! \param stringList [in/out] list to clean up
void asiTestEngine_ReportTag::eraseStrings(std::vector<std::string>& stringList)
{
  stringList.clear();
}
