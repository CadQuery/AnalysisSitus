//-----------------------------------------------------------------------------
// Created on: 15 August 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
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
