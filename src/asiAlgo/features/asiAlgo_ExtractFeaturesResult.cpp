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

// Own include
#include <asiAlgo_ExtractFeaturesResult.h>

//-----------------------------------------------------------------------------

void
  asiAlgo_ExtractFeaturesResult::GetFaceIndices(TColStd_PackedMapOfInteger& faceIds) const
{
  for ( t_data::Iterator dit(m_data); dit.More(); dit.Next() )
    for ( t_features::Iterator fit( dit.Value() ); fit.More(); fit.Next() )
      faceIds.Unite( fit.Value() );
}

//-----------------------------------------------------------------------------

TColStd_PackedMapOfInteger&
  asiAlgo_ExtractFeaturesResult::FindFeature(const int type,
                                             const int featId)
{
  // Check if the type of interest is already settled. If not, do it now
  t_features* featuresPtr = m_data.ChangeSeek(type);
  if ( featuresPtr == NULL )
    featuresPtr = m_data.Bound( type, t_features() );

  TColStd_PackedMapOfInteger* mapPtr = (*featuresPtr).ChangeSeek(featId);

  // Check if the feature id of interest is already settled. If not, do it now
  if ( mapPtr == NULL )
    mapPtr = (*featuresPtr).Bound( featId, TColStd_PackedMapOfInteger() );
  //
  return (*mapPtr);
}

//-----------------------------------------------------------------------------

bool asiAlgo_ExtractFeaturesResult::ContainsFeaturesOfType(const int type) const
{
  return m_data.IsBound(type);
}

//-----------------------------------------------------------------------------

bool asiAlgo_ExtractFeaturesResult::ContainsFeature(const int type,
                                                    const int featId) const
{
  const t_features* featuresPtr = m_data.Seek(type);

  if ( featuresPtr == NULL )
    return false;

  return (*featuresPtr).IsBound(featId);
}
