//-----------------------------------------------------------------------------
// Created on: 11 June 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2016-present, Sergey Slyadnev
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

#ifndef asiAlgo_FeatureFaces_h
#define asiAlgo_FeatureFaces_h

// asiAlgo includes
#include <asiAlgo_FeatureType.h>

// OCCT includes
#include <NCollection_DataMap.hxx>
#include <Standard_GUID.hxx>
#include <TColStd_PackedMapOfInteger.hxx>

//-----------------------------------------------------------------------------

//! Feature ID.
typedef int asiAlgo_FeatureId;

//-----------------------------------------------------------------------------

//! Feature as a set of indices of faces.
typedef TColStd_PackedMapOfInteger asiAlgo_Feature;

//-----------------------------------------------------------------------------

//! Features by indices.
typedef NCollection_DataMap<asiAlgo_FeatureId, asiAlgo_Feature> asiAlgo_Features;

//-----------------------------------------------------------------------------

//! Handy typedef for indices of feature faces organized by feature types.
typedef NCollection_DataMap<asiAlgo_FeatureType, asiAlgo_Features> asiAlgo_FeaturesByType;

//-----------------------------------------------------------------------------

//! Undefined GUID.
typedef Standard_GUID asiAlgo_BadGuid;

#endif
