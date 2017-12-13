//-----------------------------------------------------------------------------
// Created on: 28 November 2017
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

#ifndef asiAlgo_TopoAttrName_h
#define asiAlgo_TopoAttrName_h

// asiAlo includes
#include <asiAlgo_TopoAttr.h>

// OCCT includes
#include <TCollection_AsciiString.hxx>

//-----------------------------------------------------------------------------

//! Attribute storing names of sub-shapes. This attribute was originally
//! developed to support topological naming services.
class asiAlgo_TopoAttrName : public asiAlgo_TopoAttr
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_TopoAttrName, asiAlgo_TopoAttr)

public:

  //! Creates non-initialized attribute.
  asiAlgo_TopoAttrName() : asiAlgo_TopoAttr() {}

  //! Creates attribute with name.
  //! \param[in] name sub-shape name to set.
  asiAlgo_TopoAttrName(const TCollection_AsciiString& name)
  : asiAlgo_TopoAttr (),
    m_name           (name)
  {}

public:

  //! \return static GUID associated with this type of attribute.
  static const Standard_GUID& GUID()
  {
    static Standard_GUID guid("1C4C10E9-58CC-49F3-B3C6-BF786E73469E");
    return guid;
  }

  //! \return GUID associated with this type of attribute.
  virtual const Standard_GUID& GetGUID() const
  {
    return GUID();
  }

  //! \return copy of this attribute.
  virtual Handle(asiAlgo_TopoAttr) Copy() const
  {
    Handle(asiAlgo_TopoAttr) res = new asiAlgo_TopoAttrName(m_name);
    return res;
  }

public:

  //! \return name of a sub-shape.
  const TCollection_AsciiString& GetName() const { return m_name; }

  //! Sets name.
  //! \param[in] name ASCII string to set as a sub-shape name.
  void SetName(const TCollection_AsciiString& name) { m_name = name; }

protected:

  TCollection_AsciiString m_name; //!< Sub-shape name as ASCII string.

};

#endif
