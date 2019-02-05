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

#ifndef asiAlgo_ExtractFeatures_h
#define asiAlgo_ExtractFeatures_h

// asiAlgo includes
#include <asiAlgo_AAG.h>
#include <asiAlgo_ExtractFeaturesResult.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

//-----------------------------------------------------------------------------

//! \brief Extracts feature faces from AAG using the hooked attributes.
//!        This tool is designed to finalize feature recognition process.
//!
//! Most recognition utilities populate AAG with specific attributes to
//! mark faces as belonging to one or another feature. The recognized
//! faces differ by their type and also by global IDs of the features
//! comprised of these faces. In order to get the feature faces out of
//! AAG structure, it is necessary to iterate the graph and put faces
//! belonging to the same feature in one place. This is what this tool
//! really does.
//!
//! This tool iterates AAG searching for node attributes which
//! subclass \ref asiAlgo_FeatureAttrFace. For every such attribute,
//! there is a corresponding feature type encoded by an integer value.
//! The extracted faces are distributed by buckets corresponding
//! to different feature types. In each bucket, the faces belonging to
//! the same features are stored within a packed map of indices.
//!
//! NOTE: Feature type is encoded as integer value to allow for extensions.
//!       For example, you may want to introduce your own sub-classes of
//!       asiAlgo_FeatureAttrFace attribute.
//!
//! \sa asiAlgo_ExtractFeaturesResult, asiAlgo_FeatureAttrFace, asiAlgo_AAG
class asiAlgo_ExtractFeatures : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_ExtractFeatures, ActAPI_IAlgorithm)

public:

  //! \brief Ctor.
  //!
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_ExtractFeatures(ActAPI_ProgressEntry progress,
                            ActAPI_PlotterEntry  plotter);

public:

  //! Use this method to make extractor aware of your feature types.
  //! \param[in] type feature type encoded as integer.
  //! \param[in] guid GUID of the corresponding AAG attributes.
  //! \return false if the feature type cannot be registered (such type
  //!         already exists).
  asiAlgo_EXPORT bool
    RegisterFeatureType(const int            type,
                        const Standard_GUID& guid);

  //! \brief Performs extraction from the passed AAG.
  //!
  //! If the passed <result> is not NULL, the algorithm will add features
  //! to this result without cleaning.
  //!
  //! \param[in]  aag    AAG to extract features from.
  //! \param[out] result extracted features.
  //!
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Perform(const Handle(asiAlgo_AAG)&             aag,
            Handle(asiAlgo_ExtractFeaturesResult)& result) const;

  //! \brief Performs extraction for the given feature type only.
  //!
  //! If the passed <result> is not NULL, the algorithm will add features
  //! to this result without cleaning.
  //!
  //! \param[in]      aag    AAG to extract features from.
  //! \param[in]      type   feature type of interest.
  //! \param[in, out] result extracted features.
  //!
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    PerformForType(const Handle(asiAlgo_AAG)&             aag,
                   const int                              type,
                   Handle(asiAlgo_ExtractFeaturesResult)& result) const;

protected:

  //! \brief Converts feature type to the corresponding attribute GUID.
  //!
  //! \param[in] type feature type to convert.
  //! \return attribute GUID.
  asiAlgo_EXPORT Standard_GUID
    guidByType(const int type) const;

protected:

  NCollection_DataMap<int, Standard_GUID> m_types; //!< Registered feature types.

};

#endif
