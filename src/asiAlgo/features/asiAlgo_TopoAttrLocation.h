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

#ifndef asiAlgo_TopoAttrLocation_h
#define asiAlgo_TopoAttrLocation_h

// asiAlo includes
#include <asiAlgo_TopoAttr.h>

// OCCT includes
#include <TopLoc_Location.hxx>

//-----------------------------------------------------------------------------

//! Attribute storing locations of sub-shapes.
class asiAlgo_TopoAttrLocation : public asiAlgo_TopoAttr
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_TopoAttrLocation, asiAlgo_TopoAttr)

public:

  //! Creates non-initialized attribute.
  asiAlgo_TopoAttrLocation() 
  : asiAlgo_TopoAttr ( ),
    m_location       ( TopLoc_Location() )
  {}

  //! Creates attribute.
  //! \param[in] loc location to set.
  asiAlgo_TopoAttrLocation(const TopLoc_Location& loc)
  : asiAlgo_TopoAttr (),
    m_location       (loc)
  {}

public:

  //! \return static GUID associated with this type of attribute.
  static const Standard_GUID& GUID()
  {
    static Standard_GUID guid("1670B986-AD0B-42C0-85AF-19BEE0B83110");
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
    Handle(asiAlgo_TopoAttr) res = new asiAlgo_TopoAttrLocation(m_location);
    return res;
  }

public:

  //! \return location.
  const TopLoc_Location& GetLocation() const { return m_location; }

  //! Sets location.
  //! \param[in] loc location to set.
  void SetLocation(const TopLoc_Location& loc) { m_location = loc; }

protected:

  TopLoc_Location m_location; //!< Location.

};

#endif
