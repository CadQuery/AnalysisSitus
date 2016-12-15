//-----------------------------------------------------------------------------
// Created on: 15 August 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
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
