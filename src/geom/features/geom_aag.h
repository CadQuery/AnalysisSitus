//-----------------------------------------------------------------------------
// Created on: 26 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_aag_h
#define geom_aag_h

// A-Situs includes
#include <analysis_situs.h>

// Geometry includes
#include <geom_feature_attribute.h>

// STL includes
#include <vector>

// OCCT includes
#include <TopoDS_Face.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

class geom_aag_random_iterator;

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_aag, Standard_Transient)

//! AAG.
class geom_aag : public Standard_Transient
{
friend class geom_aag_random_iterator;

public:

  //! Type definition for adjacency matrix.
  typedef NCollection_DataMap<int, TColStd_PackedMapOfInteger> t_adjacency;

  //! Arc between two nodes of AAG. The arc is the explicit representation
  //! for adjacency relation.
  struct t_arc
  {
    int F1; //!< First face.
    int F2; //!< Second face.

    inline t_arc()                             : F1(0),   F2(0)   {}
    inline t_arc(const int _F1, const int _F2) : F1(_F1), F2(_F2) {}

    //! \return hash code for the arc.
    static int HashCode(const t_arc& arc, const int upper)
    {
      int key = arc.F1 + arc.F2;
      key += (key << 10);
      key ^= (key >> 6);
      key += (key << 3);
      key ^= (key >> 11);
      return (key & 0x7fffffff) % upper;
    }

    //! \return true if two links are equal.
    static int IsEqual(const t_arc& arc1, const t_arc& arc2)
    {
      return arc1.F1 == arc2.F1 && arc1.F2 == arc2.F2 ||
             arc1.F2 == arc2.F1 && arc1.F1 == arc2.F2;
    }
  };

  //! Attributes.
  typedef NCollection_DataMap<t_arc, Handle(geom_feature_attribute), t_arc> t_attributes;

public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_aag, Standard_Transient)

public:

  ASitus_EXPORT
    geom_aag(const TopoDS_Shape&               masterCAD,
             const TopTools_IndexedMapOfShape& selectedFaces);

  ASitus_EXPORT
    geom_aag(const TopoDS_Shape& masterCAD);

  ASitus_EXPORT
    ~geom_aag();

public:

  ASitus_EXPORT const TopoDS_Face&
    GetFace(const int face_idx) const;

  ASitus_EXPORT int
    GetFaceId(const TopoDS_Shape& face) const;

  ASitus_EXPORT const TColStd_PackedMapOfInteger&
    GetNeighbors(const int face_idx) const;

  ASitus_EXPORT const t_adjacency&
    GetNeighborhood() const;

  ASitus_EXPORT const TColStd_PackedMapOfInteger&
    GetSelectedFaces() const;

  ASitus_EXPORT const TopTools_IndexedMapOfShape&
    GetFaces() const;

  ASitus_EXPORT const t_attributes&
    GetAttributes() const;

  ASitus_EXPORT const Handle(geom_feature_attribute)&
    GetAttribute(const t_arc& arc) const;

  ASitus_EXPORT bool
    FindConvexOnly(TopTools_IndexedMapOfShape& resultFaces) const;

  ASitus_EXPORT void
    Remove(const TopTools_IndexedMapOfShape& faces);

protected:

  void init(const TopoDS_Shape&               masterCAD,
            const TopTools_IndexedMapOfShape& selectedFaces);

  void addMates(const TopTools_ListOfShape& mateFaces);

protected:

  //! Selected faces. Selection is performed externally using any criterion
  //! which we do not care about here. One typical scenario is to select
  //! those faces corresponding to some feature in the model.
  TColStd_PackedMapOfInteger m_selected;

  //! All faces of the master model.
  TopTools_IndexedMapOfShape m_faces;

  //! This data map can be seen as adjacency matrix for graph.
  t_adjacency m_neighbors;

  //! Stores attributes associated with each arc.
  t_attributes m_attributes;

};

#endif
