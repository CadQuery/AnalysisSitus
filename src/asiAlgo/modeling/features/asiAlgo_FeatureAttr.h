//-----------------------------------------------------------------------------
// Created on: 21 March 2016
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

#ifndef asiAlgo_FeatureAttr_h
#define asiAlgo_FeatureAttr_h

// asiAlgo includes
#include <asiAlgo.h>

// OCCT includes
#include <Standard_GUID.hxx>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiAlgo_FeatureAttr, Standard_Transient)

//! Base class for all feature attributes.
class asiAlgo_FeatureAttr : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_FeatureAttr, Standard_Transient)

public:

  virtual ~asiAlgo_FeatureAttr() {}

public:

  virtual const Standard_GUID&
    GetGUID() const = 0;

public:

  virtual void Dump(Standard_OStream&) const {}

public:

  //! Hasher for sets.
  struct t_hasher
  {
    static int HashCode(const Handle(asiAlgo_FeatureAttr)& attr, const int upper)
    {
      return Standard_GUID::HashCode(attr->GetGUID(), upper);
    }

    static bool IsEqual(const Handle(asiAlgo_FeatureAttr)& attr, const Handle(asiAlgo_FeatureAttr)& other)
    {
      return Standard_GUID::IsEqual( attr->GetGUID(), other->GetGUID() );
    }
  };

};

#endif
