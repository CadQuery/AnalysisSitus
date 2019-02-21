//-----------------------------------------------------------------------------
// Created on: 01 October 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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

#ifndef asiAlgo_FeatureAttrFace_h
#define asiAlgo_FeatureAttrFace_h

// asiAlgo includes
#include <asiAlgo_FeatureAttr.h>

//-----------------------------------------------------------------------------

//! Attribute for feature faces. This attribute brings feature ID which
//! allows marking faces as belonging to the same group.
class asiAlgo_FeatureAttrFace : public asiAlgo_FeatureAttr
{
friend class asiAlgo_AAG;

public:

  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_FeatureAttrFace, asiAlgo_FeatureAttr)

public:

  //! Creates attribute.
  //! \param[in] featureId feature ID.
  asiAlgo_FeatureAttrFace(const int featureId)
  : asiAlgo_FeatureAttr (),
    m_iFeatureId        (featureId)
  {}

public:

  //! \return feature index.
  int GetFeatureId() const { return m_iFeatureId; }

  //! Sets face ID.
  //! \param faceId [in] 1-based face ID.
  void SetFaceId(const int faceId)
  {
    m_iFaceId = faceId;
  }

  //! \return face ID.
  int GetFaceId() const
  {
    return m_iFaceId;
  }

private:

  int m_iFaceId;    //!< 1-based face ID.
  int m_iFeatureId; //!< 1-based feature ID.

};

#endif
