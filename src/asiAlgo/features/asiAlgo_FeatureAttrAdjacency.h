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

#ifndef asiAlgo_FeatureAttrAdjacency_h
#define asiAlgo_FeatureAttrAdjacency_h

// asiAlgo includes
#include <asiAlgo_FeatureAttr.h>

// OCCT includes
#include <TColStd_PackedMapOfInteger.hxx>

//-----------------------------------------------------------------------------

//! Attribute to store actual realization of adjacency between faces.
class asiAlgo_FeatureAttrAdjacency : public asiAlgo_FeatureAttr
{
public:

  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_FeatureAttrAdjacency, asiAlgo_FeatureAttr)

public:

  //! Creates adjacency attribute with empty collection of edge indices.
  //! \sa SetEdgeIndices() method to populate the attribute.
  asiAlgo_FeatureAttrAdjacency() : asiAlgo_FeatureAttr() {}

  //! Creates adjacency attribute and initializes it with the indices of
  //! edges connecting the adjacent faces.
  //! \param[in] edgeIndices unordered collection of edge indices to set.
  asiAlgo_FeatureAttrAdjacency(const TColStd_PackedMapOfInteger& edgeIndices)
  : asiAlgo_FeatureAttr (),
    m_edgeIndices       (edgeIndices)
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

  //! \return human-readable name of the attribute.
  virtual const char* GetName() const override
  {
    return "adjacency";
  }

public:

  //! \return unordered collection of edge indices.
  const TColStd_PackedMapOfInteger& GetEdgeIndices() const
  {
    return m_edgeIndices;
  }

  //! Sets collection of edge indices.
  //! \param[in] edgeIndices collection of indices of common edges.
  void SetEdgeIndices(const TColStd_PackedMapOfInteger& edgeIndices)
  {
    m_edgeIndices = edgeIndices;
  }

  //! Gathers edges by the stored indices.
  //! \param[out] edges output collection of transient edges as queried
  //!                   from AAG.
  void GetEdges(TopTools_IndexedMapOfShape& edges)
  {
    const TopTools_IndexedMapOfShape& edgeMap = this->getAAG()->RequestMapOfEdges();

    for ( TColStd_MapIteratorOfPackedMapOfInteger eit(m_edgeIndices); eit.More(); eit.Next() )
    {
      const int eid = eit.Key();
      edges.Add( edgeMap(eid) );
    }
  }

protected:

  TColStd_PackedMapOfInteger m_edgeIndices; //!< Indices of common edges.

};

#endif
