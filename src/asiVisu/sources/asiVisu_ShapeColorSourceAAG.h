//-----------------------------------------------------------------------------
// Created on: 28 May 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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

#ifndef asiVisu_ShapeColorSourceAAG_h
#define asiVisu_ShapeColorSourceAAG_h

// asiVisu includes
#include <asiVisu_ShapeColorSource.h>
#include <asiVisu_Utils.h>

// asiAlgo includes
#include <asiAlgo_AAG.h>
#include <asiAlgo_AttrFaceColor.h>

//-----------------------------------------------------------------------------

//! Data provider interface for colors associated with sub-shapes. This
//! implementation of the basic interface takes colors from AAG attributes.
class asiVisu_ShapeColorSourceAAG : public asiVisu_ShapeColorSource
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_ShapeColorSourceAAG, asiVisu_ShapeColorSource)

public:

  //! Ctor accepting AAG which is the source of color attributes.
  //! \param[in] aag AAG instance.
  asiVisu_ShapeColorSourceAAG(const Handle(asiAlgo_AAG)& aag)
  : asiVisu_ShapeColorSource (),
    m_aag                    (aag)
  {}

public:

  //! Returns color (as an integer value) for the given face ID.
  //! \param[in] faceId one-based ID of a face.
  //! \return color.
  virtual int GetFaceColor(const int faceId) const
  {
    if ( m_aag.IsNull() )
      return -1; // nullptr check.

    Handle(asiAlgo_FeatureAttr)
      attrBase = m_aag->GetNodeAttribute( faceId, asiAlgo_AttrFaceColor::GUID() );
    //
    if ( attrBase.IsNull() )
      return -1;

    // Get color.
    unsigned urgb[3];
    Handle(asiAlgo_AttrFaceColor)::DownCast(attrBase)->GetColor(urgb[0], urgb[1], urgb[2]);

    // Convert color to integer.
    const int color = asiVisu_Utils::ColorToInt(urgb);
    return color;
  }

protected:

  Handle(asiAlgo_AAG) m_aag; //!< AAG.

};

#endif
