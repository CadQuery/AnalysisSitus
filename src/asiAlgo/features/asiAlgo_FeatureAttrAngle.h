//-----------------------------------------------------------------------------
// Created on: 21 March 2016
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

#ifndef asiAlgo_FeatureAttrAngle_h
#define asiAlgo_FeatureAttrAngle_h

// Feature includes
#include <asiAlgo_FeatureAttrAdjacency.h>
#include <asiAlgo_FeatureAngleType.h>

//-----------------------------------------------------------------------------

//! Attribute storing information about feature angle between faces.
class asiAlgo_FeatureAttrAngle : public asiAlgo_FeatureAttrAdjacency
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_FeatureAttrAngle, asiAlgo_FeatureAttrAdjacency)

public:

  //! Creates feature angle attribute.
  asiAlgo_FeatureAttrAngle()
  : asiAlgo_FeatureAttrAdjacency(), m_angle(FeatureAngleType_Undefined)
  {}

  //! Creates feature angle attribute.
  //! \param[in] angleProp angle property.
  asiAlgo_FeatureAttrAngle(const asiAlgo_FeatureAngleType angleProp)
  : asiAlgo_FeatureAttrAdjacency(), m_angle(angleProp)
  {}

  //! Creates feature angle attribute with common edges.
  //! \param[in] a                 angle property.
  //! \param[in] commonEdgeIndices indices of common edges.
  asiAlgo_FeatureAttrAngle(const asiAlgo_FeatureAngleType    a,
                           const TColStd_PackedMapOfInteger& commonEdgeIndices)
  : asiAlgo_FeatureAttrAdjacency(commonEdgeIndices), m_angle(a)
  {}

public:

  //! \return static GUID associated with this type of attribute.
  static const Standard_GUID& GUID()
  {
    static Standard_GUID guid("ADCC7A78-E062-4CE2-B394-7F25DDFD555E");
    return guid;
  }

  //! \return GUID associated with this type of attribute.
  virtual const Standard_GUID& GetGUID() const
  {
    return GUID();
  }

  //! \return human-readable name of the attribute.
  virtual const char* GetName() const override
  {
    return "angle type";
  }

public:

  //! \return angle between faces.
  asiAlgo_FeatureAngleType GetAngle() const { return m_angle; }

  //! Sets angle between faces.
  //! \param[in] a the angle value.
  void SetAngle(const asiAlgo_FeatureAngleType a) { m_angle = a; }

protected:

  asiAlgo_FeatureAngleType m_angle; //!< Angle type.

};

#endif
