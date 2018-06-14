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
#include <asiTestEngine_ReportStyle.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

//! Default constructor preparing empty style.
asiTestEngine_ReportStyle::asiTestEngine_ReportStyle()
{
  this->nullify(false);
}

//! Dummy constructor for conversion from integer.
asiTestEngine_ReportStyle::asiTestEngine_ReportStyle(int)
{
  this->nullify(false);
}

//! Copy constructor.
//! \param copy [in] style to copy.
asiTestEngine_ReportStyle::asiTestEngine_ReportStyle(const asiTestEngine_ReportStyle& copy)
{
  this->nullify(false);
  this->deepCopy(copy);
}

//! Destructor. Releases allocated memory.
asiTestEngine_ReportStyle::~asiTestEngine_ReportStyle()
{
  this->nullify(true);
}

//! Assignment operator.
//! \param other [in] style to copy data from.
//! \return this.
asiTestEngine_ReportStyle& asiTestEngine_ReportStyle::operator=(const asiTestEngine_ReportStyle& other)
{
  this->nullify(true);
  this->deepCopy(other);
  return *this;
}

//! Returns true if all properties of this style descriptor are nulls.
//! \return true/false.
bool asiTestEngine_ReportStyle::IsNull() const
{
  return m_props.IsNull();
}

//! Sets border property.
//! \param px [in] border width in pixels.
void asiTestEngine_ReportStyle::SetBorder(const int px)
{
  if ( !m_props.IsBorder() )
    m_props.pPxBorder = new int;

  (*m_props.pPxBorder) = px;
}

//! Sets background color property.
//! \param tcolor [in] background color to set.
void asiTestEngine_ReportStyle::SetBgColor(const Color& color)
{
  if ( !m_props.IsBgColor() )
    m_props.pBgColor = new Color;

  (*m_props.pBgColor) = color;
}

//! Sets text color property.
//! \param color [in] font color to set.
void asiTestEngine_ReportStyle::SetColor(const Color& color)
{
  if ( !m_props.IsColor() )
    m_props.pColor = new Color;

  (*m_props.pColor) = color;
}

//! Sets font family.
//! \param font [in] font family to set.
void asiTestEngine_ReportStyle::SetFontFamily(const std::string& family)
{
  if ( !m_props.IsFontFamily() )
    m_props.pFontFamily = new std::string;

  (*m_props.pFontFamily) = family;
}

//! Sets font size property.
//! \param px [in] font size in pixels.
void asiTestEngine_ReportStyle::SetFontSize(const int px)
{
  if ( !m_props.IsFontSize() )
    m_props.pPxFont = new int;

  (*m_props.pPxFont) = px;
}

//! Sets font weight property.
//! \param weight [in] font weight to set.
void asiTestEngine_ReportStyle::SetFontWeight(const FontWeight weight)
{
  if ( !m_props.IsFontWeight() )
    m_props.pFontWeight = new FontWeight;

  (*m_props.pFontWeight) = weight;
}

//! Sets font style property.
//! \param fontStyle [in] font style to set.
void asiTestEngine_ReportStyle::SetFontStyle(const FontStyle fontStyle)
{
  if ( !m_props.IsFontStyle() )
    m_props.pFontStyle = new FontStyle;

  (*m_props.pFontStyle) = fontStyle;
}

//! Sets text alignment property.
//! \param align [in] text alignment to set.
void asiTestEngine_ReportStyle::SetTextAlignment(const TextAlign align)
{
  if ( !m_props.IsTextAlign() )
    m_props.pTextAlign = new TextAlign;

  (*m_props.pTextAlign) = align;
}

//! Sets vertical alignment property.
//! \param align [in] vertical alignment to set.
void asiTestEngine_ReportStyle::SetVerticalAlignment(const VertAlign align)
{
  if ( !m_props.IsVertAlign() )
    m_props.pVertAlign = new VertAlign;

  (*m_props.pVertAlign) = align;
}

//! Sets padding property.
//! \param px [in] padding in pixels.
void asiTestEngine_ReportStyle::SetPadding(const int px)
{
  if ( !m_props.IsPadding() )
    m_props.pPxPadding = new int;

  (*m_props.pPxPadding) = px;
}

//! Sets width property.
//! \param width [in] width property to set.
void asiTestEngine_ReportStyle::SetWidth(const std::string& width)
{
  if ( !m_props.IsWidth() )
    m_props.pWidth = new std::string;

  (*m_props.pWidth) = width;
}

//! Applies CSS style to the passed HTML tag.
//! \param tag [in/out] HTML tag to apply CSS style to.
void asiTestEngine_ReportStyle::ApplyStyles(asiTestEngine_ReportTag& tag) const
{
  tag.AddAttribute("style", this->MakeDescriptor().c_str() );
}

//! Returns CSS descriptor for this style. Normally, the returned string
//! should contains the list of properties without opening and closing
//! brackets.
//! \return CSS style definition.
std::string asiTestEngine_ReportStyle::MakeDescriptor() const
{
  std::string result;

  // Background color
  if ( m_props.IsBgColor() )
  {
    result += "background-color: ";
    result += std::string("rgb(") + asiAlgo_Utils::Str::ToString(m_props.pBgColor->R) +
              std::string(", ")   + asiAlgo_Utils::Str::ToString(m_props.pBgColor->G) +
              std::string(", ")   + asiAlgo_Utils::Str::ToString(m_props.pBgColor->B) +
              std::string(");");
  }

  // Border
  if ( m_props.IsBorder() )
  {
    result += std::string("border-width: ") + asiAlgo_Utils::Str::ToString(*m_props.pPxBorder) + "px;";
    result += std::string("border-style: solid;");
    result += std::string("border-color: rgb(220, 220, 220);");
  }

  // Text color
  if ( m_props.IsColor() )
  {
    result += "color: ";
    result += std::string("rgb(") + asiAlgo_Utils::Str::ToString(m_props.pColor->R) +
              std::string(", ")   + asiAlgo_Utils::Str::ToString(m_props.pColor->G) +
              std::string(", ")   + asiAlgo_Utils::Str::ToString(m_props.pColor->B) +
              std::string(");");
  }

  // Font family
  if ( m_props.IsFontFamily() )
  {
    result += std::string("font-family: ") + *m_props.pFontFamily + std::string(";");
  }

  // Font size
  if ( m_props.IsFontSize() )
  {
    result += std::string("font-size: ") + asiAlgo_Utils::Str::ToString(*m_props.pPxFont) + std::string("px;");
  }

  // Font style
  if ( m_props.IsFontStyle() )
  {
    result += std::string("font-style: ") + this->getFontStyle(*m_props.pFontStyle) + std::string(";");
  }

  // Font weight
  if ( m_props.IsFontWeight() )
  {
    result += std::string("font-weight: ") + this->getFontWeight(*m_props.pFontWeight) + std::string(";");
  }

  // Padding
  if ( m_props.IsPadding() )
  {
    result += std::string("padding: ") + asiAlgo_Utils::Str::ToString(*m_props.pPxPadding) + std::string("px;");
  }

  // Text alignment
  if ( m_props.IsTextAlign() )
  {
    result += std::string("text-align: ") + this->getTextAlignCSS(*m_props.pTextAlign) + std::string(";");
  }

  // Vertical alignment
  if ( m_props.IsVertAlign() )
  {
    result += std::string("vertical-align: ") + this->getVertAlignCSS(*m_props.pVertAlign) + std::string(";");
  }

  // Width
  if ( m_props.IsWidth() )
  {
    result += std::string("width: ") + asiAlgo_Utils::Str::ToString(*m_props.pWidth) + std::string(";");
  }

  return result;
}

//! Returns string representation for the passed font style specifier.
//! \param style [in] font style specifier.
//! \return corresponding string.
const char* asiTestEngine_ReportStyle::getFontStyle(const FontStyle style) const
{
  switch ( style )
  {
    case FS_Italic : return "italic";
    default        : break;
  }
  return "normal";
}

//! Returns string representation for the passed font weight specifier.
//! \param weight [in] font weight specifier.
//! \return corresponding string.
const char* asiTestEngine_ReportStyle::getFontWeight(const FontWeight weight) const
{
  switch ( weight )
  {
    case FW_Bold : return "bold";
    default      : break;
  }
  return "normal";
}

//! Returns string representation for the passed text alignment specifier.
//! \param align [in] text alignment specifier.
//! \return corresponding string.
const char* asiTestEngine_ReportStyle::getTextAlignCSS(const TextAlign align) const
{
  switch ( align )
  {
    case TA_Center  : return "center";
    case TA_Justify : return "justify";
    case TA_Left    : return "left";
    case TA_Right   : return "right";
    default         : break;
  }
  return "";
}

//! Returns string representation for the passed vertical alignment specifier.
//! \param align [in] vertical alignment specifier.
//! \return corresponding string.
const char* asiTestEngine_ReportStyle::getVertAlignCSS(const VertAlign align) const
{
  switch ( align )
  {
    case VA_Bottom : return "bottom";
    case VA_Middle : return "middle";
    case VA_Top    : return "top";
    default        : break;
  }
  return "";
}

//! Nullifies properties.
//! \param release [in] indicates whether to release memory.
void asiTestEngine_ReportStyle::nullify(const bool release)
{
  if ( release )
  {
    delete m_props.pBgColor;
    delete m_props.pColor;
    delete m_props.pFontFamily;
    delete m_props.pFontStyle;
    delete m_props.pFontWeight;
    delete m_props.pPxBorder;
    delete m_props.pPxFont;
    delete m_props.pPxPadding;
    delete m_props.pTextAlign;
    delete m_props.pVertAlign;
    delete m_props.pWidth;
  }

  m_props.pBgColor    = NULL;
  m_props.pColor      = NULL;
  m_props.pFontFamily = NULL;
  m_props.pFontStyle  = NULL;
  m_props.pFontWeight = NULL;
  m_props.pPxBorder   = NULL;
  m_props.pPxFont     = NULL;
  m_props.pPxPadding  = NULL;
  m_props.pTextAlign  = NULL;
  m_props.pVertAlign  = NULL;
  m_props.pWidth      = NULL;
}

//! Copies styling properties.
//! \param other [in] style to copy from.
void asiTestEngine_ReportStyle::deepCopy(const asiTestEngine_ReportStyle& other)
{
  if ( other.m_props.IsBgColor() )    this->SetBgColor( *other.m_props.pBgColor );
  if ( other.m_props.IsColor() )      this->SetColor( *other.m_props.pColor );
  if ( other.m_props.IsFontFamily() ) this->SetFontFamily( *other.m_props.pFontFamily );
  if ( other.m_props.IsFontStyle() )  this->SetFontStyle( *other.m_props.pFontStyle );
  if ( other.m_props.IsFontWeight() ) this->SetFontWeight( *other.m_props.pFontWeight );
  if ( other.m_props.IsBorder() )     this->SetBorder( *other.m_props.pPxBorder );
  if ( other.m_props.IsFontSize() )   this->SetFontSize( *other.m_props.pPxFont );
  if ( other.m_props.IsPadding() )    this->SetPadding( *other.m_props.pPxPadding );
  if ( other.m_props.IsTextAlign() )  this->SetTextAlignment( *other.m_props.pTextAlign );
  if ( other.m_props.IsVertAlign() )  this->SetVerticalAlignment( *other.m_props.pVertAlign );
  if ( other.m_props.IsWidth() )      this->SetWidth( *other.m_props.pWidth );
}
