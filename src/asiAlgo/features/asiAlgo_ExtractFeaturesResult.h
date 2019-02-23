//-----------------------------------------------------------------------------
// Created on: 04 February 2019
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

#ifndef asiAlgo_ExtractFeaturesResult_h
#define asiAlgo_ExtractFeaturesResult_h

// asiAlgo includes
#include <asiAlgo.h>

// OCCT includes
#include <NCollection_DataMap.hxx>
#include <TColStd_PackedMapOfInteger.hxx>

//-----------------------------------------------------------------------------

//! This class represents feature extraction result.
//!
//! \sa asiAlgo_AAG, asiAlgo_ExtractFeatures
class asiAlgo_ExtractFeaturesResult : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_ExtractFeaturesResult, Standard_Transient)

public:

  //! Yet another collection to represent features with IDs.
  typedef NCollection_DataMap<int, TColStd_PackedMapOfInteger> t_features;

  //! Convenience alias for indices of feature faces distributed by types.
  //! The face indices of a fixed type are grouped in a map to represent
  //! features recognized separately.
  typedef NCollection_DataMap<int, t_features> t_data;

public:

  //! Collects all face indices into one map and returns it. Use this method
  //! if you are not specific about feature types and IDs.
  //! \param[out] faceIds IDs of the faces.
  asiAlgo_EXPORT void
    GetFaceIndices(TColStd_PackedMapOfInteger& faceIds) const;

  //! Returns a group of face indices (actually, this is exactly a feature
  //! itself) corresponding to the given feature type and feature index.
  //!
  //! \param[in] type   feature type of interest.
  //! \param[in] featId feature id of interest.
  //!
  //! \return reference to the target group of faces. If not found, this
  //!         group is created on-fly and returned.
  asiAlgo_EXPORT TColStd_PackedMapOfInteger&
    FindFeature(const int type,
                const int featId);

  //! Checks whether any features of the type in question are registered
  //! in the result.
  //!
  //! \param[in] type feature type of interest.
  //!
  //! \return true/false.
  asiAlgo_EXPORT bool
    ContainsFeaturesOfType(const int type) const;

  //! Checks whether the result contains a feature of the given type with
  //! the passed ID.
  //!
  //! \param[in] type   feature type of interest.
  //! \param[in] featId ID of the feature in question.
  //!
  //! \return true/false.
  asiAlgo_EXPORT bool
    ContainsFeature(const int type,
                    const int featId) const;

protected:

  //! Internal data structure for feature faces distributed by types.
  t_data m_data;

};

#endif
