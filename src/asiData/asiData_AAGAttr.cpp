//-----------------------------------------------------------------------------
// Created on: 21 December 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

// Own include
#include <asiData_AAGAttr.h>

// OCCT includes
#include <Standard_GUID.hxx>

//-----------------------------------------------------------------------------
// Construction & settling-down routines
//-----------------------------------------------------------------------------

//! Default constructor.
asiData_AAGAttr::asiData_AAGAttr() : TDF_Attribute()
{}

//! Settles down new AAG Attribute to the given OCAF Label.
//! \param[in] label TDF Label to settle down the new Attribute to.
//! \return newly created Attribute settled down onto the target Label.
Handle(asiData_AAGAttr) asiData_AAGAttr::Set(const TDF_Label& label)
{
  Handle(asiData_AAGAttr) A;
  //
  if ( !label.FindAttribute(GUID(), A) )
  {
    A = new asiData_AAGAttr();
    label.AddAttribute(A);
  }
  return A;
}

//-----------------------------------------------------------------------------
// Accessors for Attribute's GUID
//-----------------------------------------------------------------------------

//! Returns statically defined GUID for AAG Attribute.
//! \return statically defined GUID.
const Standard_GUID& asiData_AAGAttr::GUID()
{
  static Standard_GUID AttrGUID("A8DC530F-16E8-4591-8D7E-1D2C19E8B7EB");
  return AttrGUID;
}

//! Accessor for GUID associated with this kind of OCAF Attribute.
//! \return GUID of the OCAF Attribute.
const Standard_GUID& asiData_AAGAttr::ID() const
{
  return GUID();
}

//-----------------------------------------------------------------------------
// Attribute's kernel methods:
//-----------------------------------------------------------------------------

//! Creates new instance of AAG Attribute which is not initially populated
//! with any data structures.
//! \return new instance of AAG Attribute.
Handle(TDF_Attribute) asiData_AAGAttr::NewEmpty() const
{
  return new asiData_AAGAttr();
}

//! Performs data transferring from the given OCAF Attribute to this one.
//! This method is mainly used by OCAF Undo/Redo kernel as a part of
//! backup functionality.
//! \param[in] mainAttr OCAF Attribute to copy data from.
void asiData_AAGAttr::Restore(const Handle(TDF_Attribute)& asiData_NotUsed(mainAttr))
{
  // Nothing is here
}

//! Supporting method for Copy/Paste functionality. Performs full copying of
//! the underlying data.
//! \param[in] into       where to paste.
//! \param[in] relocTable relocation table.
void asiData_AAGAttr::Paste(const Handle(TDF_Attribute)&       asiData_NotUsed(into),
                            const Handle(TDF_RelocationTable)& asiData_NotUsed(relocTable)) const
{
  // Nothing is here
}

//-----------------------------------------------------------------------------
// Accessors for domain-specific data
//-----------------------------------------------------------------------------

//! Sets AAG to store.
//! \param[in] aag AAG to store.
void asiData_AAGAttr::SetAAG(const Handle(asiAlgo_AAG)& aag)
{
  m_aag = aag;
}

//! Returns the stored AAG.
//! \return stored AAG.
const Handle(asiAlgo_AAG)& asiData_AAGAttr::GetAAG() const
{
  return m_aag;
}
