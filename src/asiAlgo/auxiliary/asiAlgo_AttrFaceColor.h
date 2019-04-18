//-----------------------------------------------------------------------------
// Created on: 18 April 2019
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

#ifndef asiAlgo_AttrFaceColor_h
#define asiAlgo_AttrFaceColor_h

// asiAlgo includes
#include <asiAlgo_FeatureAttrFace.h>

// Active Data includes
#include <ActAPI_IPlotter.h>

//-----------------------------------------------------------------------------

//! Attribute to colorize a face.
class asiAlgo_AttrFaceColor : public asiAlgo_FeatureAttrFace
{
public:

  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_AttrFaceColor, asiAlgo_FeatureAttrFace)

public:

  //! Creates attribute with feature ID.
  //! \param[in] featureId 1-based feature ID.
  asiAlgo_AttrFaceColor(const int featureId = 0) : asiAlgo_FeatureAttrFace(featureId)
  {
    this->SetColor(0, 0, 0);
  }

  //! Creates attribute with feature ID.
  //! \param[in] ured      red color component as unsigned integer.
  //! \param[in] ugreen    green color component as unsigned integer.
  //! \param[in] ublue     blue color component as unsigned integer.
  //! \param[in] featureId 1-based feature ID.
  asiAlgo_AttrFaceColor(const unsigned ured,
                        const unsigned ugreen,
                        const unsigned ublue,
                        const int      featureId = 0)
  : asiAlgo_FeatureAttrFace(featureId)
  {
    this->SetColor(ured, ugreen, ublue);
  }

public:

  //! \return static GUID associated with this type of attribute.
  static const Standard_GUID& GUID()
  {
    static Standard_GUID guid("C62AD5E2-819B-4B1E-9935-5BBFFFAFD3A8");
    return guid;
  }

  //! \return GUID associated with this type of attribute.
  virtual const Standard_GUID& GetGUID() const override
  {
    return GUID();
  }

  //! \return human-readable name of the attribute.
  virtual const char* GetName() const override
  {
    return "face color";
  }

public:

  //! Sets color for the attribute.
  //! \param[in] ured   red color component as unsigned integer.
  //! \param[in] ugreen green color component as unsigned integer.
  //! \param[in] ublue  blue color component as unsigned integer.
  void SetColor(const unsigned ured,
                const unsigned ugreen,
                const unsigned ublue)
  {
    m_color[0] = ured;
    m_color[1] = ugreen;
    m_color[2] = ublue;
  }

  //! Returns color stored in the attribute.
  //! \param[out] ured   red color component as unsigned integer.
  //! \param[out] ugreen green color component as unsigned integer.
  //! \param[out] ublue  blue color component as unsigned integer.
  void GetColor(unsigned& ured,
                unsigned& ugreen,
                unsigned& ublue) const
  {
    ured   = m_color[0];
    ugreen = m_color[1];
    ublue  = m_color[2];
  }

private:

  unsigned int m_color[3]; //!< Color components.

};

#endif
