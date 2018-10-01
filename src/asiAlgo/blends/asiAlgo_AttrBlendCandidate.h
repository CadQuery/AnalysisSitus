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

#ifndef asiAlgo_AttrBlendCandidate_h
#define asiAlgo_AttrBlendCandidate_h

// asiAlgo includes
#include <asiAlgo_BlendType.h>
#include <asiAlgo_FeatureAttrFace.h>

// Active Data includes
#include <ActAPI_IPlotter.h>

//-----------------------------------------------------------------------------

//! Attribute to mark a face as a blend candidate.
class asiAlgo_AttrBlendCandidate : public asiAlgo_FeatureAttrFace
{
public:

  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_AttrBlendCandidate, asiAlgo_FeatureAttrFace)

public:

  //! Creates attribute with feature ID.
  //! \param[in] featureId 1-based feature ID.
  asiAlgo_AttrBlendCandidate(const int featureId)
  : asiAlgo_FeatureAttrFace (featureId),
    Kind                    (BlendType_Uncertain),
    Confirmed               (false)
  {}

public:

  //! \return static GUID associated with this type of attribute.
  static const Standard_GUID& GUID()
  {
    static Standard_GUID guid("7B7C2DD0-5BD6-429E-9B71-5F935BF1E815");
    return guid;
  }

  //! \return GUID associated with this type of attribute.
  virtual const Standard_GUID& GetGUID() const override
  {
    return GUID();
  }

public:

  //! Dumps this attribute to the passed output stream.
  //! \param[in, out] target stream.
  asiAlgo_EXPORT virtual void
    Dump(Standard_OStream& out) const;

  //! Dumps this attribute to the passed plotter.
  //! \param[in, out] imperative plotter.
  asiAlgo_EXPORT virtual void
    Dump(ActAPI_PlotterEntry plotter) const;

public:

  asiAlgo_BlendType          Kind;                   //!< Blend type.
  bool                       Confirmed;              //!< Confirmed/Unconfirmed blend.
  TColStd_PackedMapOfInteger SmoothEdgeIndices;      //!< Smooth edges.
  TColStd_PackedMapOfInteger SpringEdgeIndices;      //!< Spring edges.
  TColStd_PackedMapOfInteger CrossEdgeIndices;       //!< Cross edges.
  TColStd_PackedMapOfInteger TerminatingEdgeIndices; //!< Sharp terminating edges.

};

#endif
