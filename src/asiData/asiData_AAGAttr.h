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

#ifndef asiData_AAGAttr_h
#define asiData_AAGAttr_h

// asiData includes
#include <asiData.h>

// asiAlgo includes
#include <asiAlgo_AAG.h>

// OCCT includes
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>

DEFINE_STANDARD_HANDLE(asiData_AAGAttr, TDF_Attribute)

//! OCAF Attribute representing Attributed Adjacency Graph (AAG).
class asiData_AAGAttr : public TDF_Attribute
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_AAGAttr, TDF_Attribute)

// Construction & settling-down routines:
public:

  asiData_EXPORT
    asiData_AAGAttr();

  asiData_EXPORT static Handle(asiData_AAGAttr)
    Set(const TDF_Label& Label);

// GUID accessors:
public:

  asiData_EXPORT static const Standard_GUID&
    GUID();

  asiData_EXPORT virtual const Standard_GUID&
    ID() const;

// Attribute's kernel methods:
public:

  asiData_EXPORT virtual Handle(TDF_Attribute)
    NewEmpty() const;

  asiData_EXPORT virtual void
    Restore(const Handle(TDF_Attribute)& mainAttr);

  asiData_EXPORT virtual void
    Paste(const Handle(TDF_Attribute)&       into,
          const Handle(TDF_RelocationTable)& relocTable) const;

// Accessors for domain-specific data:
public:

  asiData_EXPORT void
    SetAAG(const Handle(asiAlgo_AAG)& aag);

  asiData_EXPORT const Handle(asiAlgo_AAG)&
    GetAAG() const;

// Members:
private:

  //! Stored AAG.
  Handle(asiAlgo_AAG) m_aag;

};

#endif
