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

#ifndef asiAlgo_TopoAttrOrientation_h
#define asiAlgo_TopoAttrOrientation_h

// asiAlgo includes
#include <asiAlgo_TopoAttr.h>

// OCCT includes
#include <TopAbs_Orientation.hxx>

//-----------------------------------------------------------------------------

//! Attribute storing sub-shape orientation.
class asiAlgo_TopoAttrOrientation : public asiAlgo_TopoAttr
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_TopoAttrOrientation, asiAlgo_TopoAttr)

public:

  //! Creates non-initialized attribute.
  asiAlgo_TopoAttrOrientation() 
  : asiAlgo_TopoAttr (),
    m_orientation    (TopAbs_EXTERNAL)
  {}

  //! Creates attribute.
  //! \param[in] ori orientation to set.
  asiAlgo_TopoAttrOrientation(const TopAbs_Orientation ori)
  : asiAlgo_TopoAttr (),
    m_orientation    (ori)
  {}

public:

  //! \return static GUID associated with this type of attribute.
  static const Standard_GUID& GUID()
  {
    static Standard_GUID guid("1F222763-4C0C-417A-AC24-9654E1BAFBBF");
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
    Handle(asiAlgo_TopoAttr) res = new asiAlgo_TopoAttrOrientation(m_orientation);
    return res;
  }

public:

  //! \return orientation.
  TopAbs_Orientation GetOrientation() const { return m_orientation; }

  //! Sets orientation.
  //! \param[in] ori orientation to set.
  void SetOrientation(const TopAbs_Orientation ori) { m_orientation = ori; }

protected:

  TopAbs_Orientation m_orientation; //!< Orientation.

};

#endif
