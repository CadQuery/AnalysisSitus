//-----------------------------------------------------------------------------
// Created on: 26 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiAlgo_AAG_h
#define asiAlgo_AAG_h

// asiAlgo includes
#include <asiAlgo_FeatureAttr.h>

// STL includes
#include <vector>

// OCCT includes
#include <NCollection_DataMap.hxx>
#include <Standard_OStream.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

class asiAlgo_AAGRandomIterator;

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiAlgo_AAG, Standard_Transient)

//! AAG.
class asiAlgo_AAG : public Standard_Transient
{
friend class asiAlgo_AAGRandomIterator;

public:

  //---------------------------------------------------------------------------

  //! Type definition for adjacency matrix.
  typedef NCollection_DataMap<int, TColStd_PackedMapOfInteger> t_adjacency;

  //---------------------------------------------------------------------------

  //! Arc between two nodes of AAG. The arc is the explicit representation
  //! for adjacency relation.
  struct t_arc
  {
    int F1; //!< First face.
    int F2; //!< Second face.

    //! ctor default.
    t_arc() : F1(0), F2(0) {}

    //! ctor with parameters.
    t_arc(const int _F1, const int _F2) : F1(_F1), F2(_F2) {}

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

  //---------------------------------------------------------------------------

  //! Collection of attributes.
  class t_attr_set
  {
  public:

    t_attr_set() {} //!< Default ctor.

    //! Constructor accepting a single attribute to populate the internal set.
    //! \param[in] single_attr single attribute to populate the set with.
    t_attr_set(const Handle(asiAlgo_FeatureAttr)& single_attr)
    {
      this->Add(single_attr);
    }

  public:

    //! Finds attribute by its global type ID.
    //! \param[in] id attribute's global ID.
    //! \return attribute instance.
    const Handle(asiAlgo_FeatureAttr)& operator()(const Standard_GUID& id) const
    {
      return m_set(id);
    }

    //! Adds the given attribute to the set.
    //! \param[in] attr attribute to add.
    void Add(const Handle(asiAlgo_FeatureAttr)& attr)
    {
      m_set.Bind(attr->GetGUID(), attr);
    }

    //! \return internal collection.
    const NCollection_DataMap<Standard_GUID, Handle(asiAlgo_FeatureAttr), Standard_GUID>& GetMap() const
    {
      return m_set;
    }

  private:

    //! Internal set storing attributes in association with their global IDs.
    NCollection_DataMap<Standard_GUID, Handle(asiAlgo_FeatureAttr), Standard_GUID> m_set;

  };

  //---------------------------------------------------------------------------

  //! Arc attributes.
  typedef NCollection_DataMap<t_arc, Handle(asiAlgo_FeatureAttr), t_arc> t_arc_attributes;

  //! Node attributes.
  typedef NCollection_DataMap<int, t_attr_set> t_node_attributes;

  //---------------------------------------------------------------------------

public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_AAG, Standard_Transient)

public:

  asiAlgo_EXPORT
    asiAlgo_AAG(const TopoDS_Shape&               masterCAD,
                const TopTools_IndexedMapOfShape& selectedFaces,
                const bool                        allowSmooth = false);

  asiAlgo_EXPORT
    asiAlgo_AAG(const TopoDS_Shape& masterCAD,
                const bool          allowSmooth = false);

  asiAlgo_EXPORT
    ~asiAlgo_AAG();

public:

  asiAlgo_EXPORT Handle(asiAlgo_AAG)
    Copy() const;

public:

  asiAlgo_EXPORT const TopoDS_Shape&
    GetMasterCAD() const;

  asiAlgo_EXPORT bool
    HasFace(const int face_idx) const;

  asiAlgo_EXPORT const TopoDS_Face&
    GetFace(const int face_idx) const;

  asiAlgo_EXPORT int
    GetFaceId(const TopoDS_Shape& face) const;

  asiAlgo_EXPORT bool
    HasNeighbors(const int face_idx) const;

  asiAlgo_EXPORT const TColStd_PackedMapOfInteger&
    GetNeighbors(const int face_idx) const;

  asiAlgo_EXPORT TColStd_PackedMapOfInteger
    GetNeighborsThru(const int face_idx, const TopoDS_Edge& edge) const;

  asiAlgo_EXPORT TColStd_PackedMapOfInteger
    GetNeighbors(const int                         face_idx,
                 const TColStd_PackedMapOfInteger& edge_ids) const;

  asiAlgo_EXPORT const t_adjacency&
    GetNeighborhood() const;

  asiAlgo_EXPORT const TColStd_PackedMapOfInteger&
    GetSelectedFaces() const;

  asiAlgo_EXPORT const TopTools_IndexedMapOfShape&
    GetFaces() const;

  asiAlgo_EXPORT const TopTools_IndexedMapOfShape&
    GetEdges() const;

  asiAlgo_EXPORT const t_arc_attributes&
    GetArcAttributes() const;

  asiAlgo_EXPORT const Handle(asiAlgo_FeatureAttr)&
    GetArcAttribute(const t_arc& arc) const;

  asiAlgo_EXPORT bool
    HasNodeAttributes(const int node) const;

  asiAlgo_EXPORT const t_attr_set&
    GetNodeAttributes(const int node) const;

  asiAlgo_EXPORT Handle(asiAlgo_FeatureAttr)
    GetNodeAttribute(const int            node,
                     const Standard_GUID& attr_id) const;

  asiAlgo_EXPORT bool
    SetNodeAttribute(const int                   node,
                     const Handle(asiAlgo_FeatureAttr)& attr);

  asiAlgo_EXPORT bool
    FindConvexOnly(TopTools_IndexedMapOfShape& resultFaces) const;

  asiAlgo_EXPORT bool
    FindConcaveOnly(TopTools_IndexedMapOfShape& resultFaces) const;

  asiAlgo_EXPORT void
    Remove(const TopTools_IndexedMapOfShape& faces);

  asiAlgo_EXPORT void
    Dump(Standard_OStream& out) const;

protected:

  asiAlgo_EXPORT void
    init(const TopoDS_Shape&               masterCAD,
         const TopTools_IndexedMapOfShape& selectedFaces,
         const bool                        allowSmooth);

  asiAlgo_EXPORT void
    addMates(const TopTools_ListOfShape& mateFaces);

protected:

  asiAlgo_AAG() {}

protected:

  //! Master CAD model.
  TopoDS_Shape m_master;

  //! Selected faces. Selection is performed externally using any criterion
  //! which we do not care about here. One typical scenario is to select
  //! those faces corresponding to some feature in the model.
  TColStd_PackedMapOfInteger m_selected;

  //! All faces of the master model.
  TopTools_IndexedMapOfShape m_faces;

  //! All edges of the master model.
  TopTools_IndexedMapOfShape m_edges;

  //! This data map can be seen as adjacency matrix for graph.
  t_adjacency m_neighbors;

  //! Stores attributes associated with each arc.
  t_arc_attributes m_arc_attributes;

  //! Stores attributes associated with nodes.
  t_node_attributes m_node_attributes;

  //! Indicates whether to allow smooth transitions or not.
  bool m_bAllowSmooth;

};

#endif
