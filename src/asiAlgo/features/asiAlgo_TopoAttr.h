//-----------------------------------------------------------------------------
// Created on: 19 November 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
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

#ifndef asiAlgo_TopoAttr_h
#define asiAlgo_TopoAttr_h

// asiAlgo includes
#include <asiAlgo.h>

// OCCT includes
#include <Standard_GUID.hxx>

//-----------------------------------------------------------------------------

//! Base class for all topological attributes.
class asiAlgo_TopoAttr : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_TopoAttr, Standard_Transient)

public:

  virtual ~asiAlgo_TopoAttr() {}

public:

  virtual const Standard_GUID&
    GetGUID() const = 0;

  virtual Handle(asiAlgo_TopoAttr)
    Copy() const = 0;

public:

  virtual void Dump(Standard_OStream&) const {}

public:

  //! Hasher for sets.
  struct t_hasher
  {
    static int HashCode(const Handle(asiAlgo_TopoAttr)& attr, const int upper)
    {
      return Standard_GUID::HashCode(attr->GetGUID(), upper);
    }

    static bool IsEqual(const Handle(asiAlgo_TopoAttr)& attr, const Handle(asiAlgo_TopoAttr)& other)
    {
      return Standard_GUID::IsEqual( attr->GetGUID(), other->GetGUID() );
    }
  };

};

#endif
