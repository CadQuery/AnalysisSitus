//-----------------------------------------------------------------------------
// Created on: 15 August 2016
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

#ifndef asiAlgo_FeatureAttrAdjacency_h
#define asiAlgo_FeatureAttrAdjacency_h

// asiAlgo includes
#include <asiAlgo_FeatureAttr.h>

// OCCT includes
#include <TopTools_IndexedMapOfShape.hxx>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiAlgo_FeatureAttrAdjacency, asiAlgo_FeatureAttr)

//! Attribute storing information related to realization of adjacency
//! between faces.
class asiAlgo_FeatureAttrAdjacency : public asiAlgo_FeatureAttr
{
public:

  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_FeatureAttrAdjacency, asiAlgo_FeatureAttr)

public:

  //! Creates feature adjacency attribute.
  asiAlgo_FeatureAttrAdjacency() : asiAlgo_FeatureAttr() {}

  //! Creates feature adjacency attribute and initializes it with edges.
  //! \param[in] edges edges to set.
  asiAlgo_FeatureAttrAdjacency(const TopTools_IndexedMapOfShape& edges)
  : asiAlgo_FeatureAttr(),
    m_edges(edges)
  {}

public:

  //! \return static GUID associated with this type of attribute.
  static const Standard_GUID& GUID()
  {
    static Standard_GUID guid("2C9F5B79-4D0A-4824-8878-A1E8CF404E31");
    return guid;
  }

  //! \return GUID associated with this type of attribute.
  virtual const Standard_GUID& GetGUID() const
  {
    return GUID();
  }

public:

  //! \return edges.
  const TopTools_IndexedMapOfShape& GetEdges() const { return m_edges; }

  //! Sets edges.
  //! \param[in] edges edges in common.
  void SetEdges(const TopTools_IndexedMapOfShape& edges) { m_edges = edges; }

protected:

  TopTools_IndexedMapOfShape m_edges; //!< Edges in common.

};

#endif
