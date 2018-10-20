//-----------------------------------------------------------------------------
// Created on: 26 February 2016
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

//! AAG.
class asiAlgo_AAG : public Standard_Transient
{
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

    //! Convenience iterator for the set of attributes associated with
    //! node or arc in AAG.
    class Iterator
    {
    public:

      //! Ctor accepting the set of attributes to iterate.
      //! \param[in] attributes set of attributes to iterate.
      Iterator(const t_attr_set& attributes) : m_attrs(attributes)
      {
        m_it.Initialize( m_attrs.GetMap() );
      }

      //! \return true if there is something more to iterate starting from
      //!         current position, false -- otherwise.
      bool More() const
      {
        return m_it.More();
      }

      //! Moves iterator to the next position.
      void Next()
      {
        m_it.Next();
      }

      //! \return GUID of the currently iterated attribute.
      const Standard_GUID& GetGUID() const
      {
        return m_it.Key();
      }

      //! \return currently iterated attribute.
      const Handle(asiAlgo_FeatureAttr)& GetAttr() const
      {
        return m_it.Value();
      }

    protected:

      //! Attributes to iterate over.
      const t_attr_set& m_attrs;

      //! Internal iterator.
      NCollection_DataMap<Standard_GUID, Handle(asiAlgo_FeatureAttr)>::Iterator m_it;

    private:

      // To avoid C4512 "assignment operator could not be generated".
      Iterator& operator=(const Iterator&) { return *this; }
    };

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

  //! Initializes AAG from the    given master model and selected faces.
  //! \param[in] masterCAD        master model (full CAD).
  //! \param[in] selectedFaces    selected faces.
  //! \param[in] allowSmooth      indicates whether to allow "smooth" value for
  //!                             arc attribute. This value means that the
  //!                             joint between faces is at least G1, so in
  //!                             order to calculate dihedral angle, the neighborhood
  //!                             of transition has to be analyzed. The latter
  //!                             analysis introduces additional cost, so you
  //!                             may disable it if you are Ok to attribute G0
  //!                             joints only.
  //! \param[in] smoothAngularTol angular tolerance used for recognition
  //!                             of smooth dihedral angles. A smooth angle
  //!                             may appear to be imperfect by construction,
  //!                             but still smooth by the design intent. With
  //!                             this parameter you're able to control it.
  asiAlgo_EXPORT
    asiAlgo_AAG(const TopoDS_Shape&               masterCAD,
                const TopTools_IndexedMapOfShape& selectedFaces,
                const bool                        allowSmooth = false,
                const double                      smoothAngularTol = 1.e-4);

  //! Constructor accepting master CAD only.
  //! \param[in] masterCAD        master CAD.
  //! \param[in] allowSmooth      indicates whether "smooth" attribution for arcs
  //!                             is allowed (true) or not (false).
  //! \param[in] smoothAngularTol angular tolerance used for recognition
  //!                             of smooth dihedral angles. A smooth angle
  //!                             may appear to be imperfect by construction,
  //!                             but still smooth by the design intent. With
  //!                             this parameter you're able to control it.
  asiAlgo_EXPORT
    asiAlgo_AAG(const TopoDS_Shape& masterCAD,
                const bool          allowSmooth = false,
                const double        smoothAngularTol = 1.e-4);

  //! Destructor.
  asiAlgo_EXPORT
    ~asiAlgo_AAG();

public:

  //! \return copy of this AAG.
  asiAlgo_EXPORT Handle(asiAlgo_AAG)
    Copy() const;

public:

  //! \return master CAD.
  asiAlgo_EXPORT const TopoDS_Shape&
    GetMasterCAD() const;

  //! \brief Returns map of indexed sub-shapes of the given type.
  //!
  //! \param[in]  ssType sub-shape type (TopAbs_VERTEX, TopAbs_EDGE or TopAbs_FACE).
  //! \param[out] map    requested map of sub-shapes.
  asiAlgo_EXPORT void
    GetMapOf(const TopAbs_ShapeEnum      ssType,
             TopTools_IndexedMapOfShape& map) const;

  //! \return map of all faces.
  asiAlgo_EXPORT const TopTools_IndexedMapOfShape&
    GetMapOfFaces() const;

  //! \return map of all edges.
  asiAlgo_EXPORT const TopTools_IndexedMapOfShape&
    GetMapOfEdges() const;

  //! \return map of all vertices.
  asiAlgo_EXPORT const TopTools_IndexedMapOfShape&
    GetMapOfVertices() const;

  //! \return map of all sub-shapes.
  asiAlgo_EXPORT const TopTools_IndexedMapOfShape&
    GetMapOfSubShapes() const;

  //! Returns true if the index is in range.
  //! \param[in] face_idx face index.
  //! \return true/false.
  asiAlgo_EXPORT bool
    HasFace(const int face_idx) const;

  //! Returns topological face by its internal index (e.g. coming from iterator).
  //! \param[in] face_idx face index.
  //! \return topological face.
  asiAlgo_EXPORT const TopoDS_Face&
    GetFace(const int face_idx) const;

  //! Returns face ID.
  //! \param[in] face face of interest.
  //! \return face ID.
  asiAlgo_EXPORT int
    GetFaceId(const TopoDS_Shape& face) const;

  //! Checks whether the given face has any neighbors recorded in the AAG.
  //! Normally it has, but in some abnormal situations no neighbors could
  //! be there.
  //! \param[in] face_idx face index.
  //! \return true in case if at least one neighbor presents, false -- otherwise.
  asiAlgo_EXPORT bool
    HasNeighbors(const int face_idx) const;

  //! Returns neighbors for the face having the given internal index.
  //! \param[in] face_idx face index.
  //! \return indices of the neighbor faces.
  asiAlgo_EXPORT const TColStd_PackedMapOfInteger&
    GetNeighbors(const int face_idx) const;

  //! Returns only those neighbor faces which share the given edge with the
  //! passed face of interest.
  //! \param[in] face_idx ID of the face of interest.
  //! \param[in] edge     common edge.
  //! \return indices of the neighbor faces sharing the given edge.
  asiAlgo_EXPORT TColStd_PackedMapOfInteger
    GetNeighborsThru(const int face_idx, const TopoDS_Edge& edge) const;

  //! Returns neighbor faces for the given face of interest with additional
  //! filter on edges realizing the neighborhood.
  //! \param[in] face_idx index of the face of interest.
  //! \param[in] edge_ids indices of edges of interest.
  //! \return indices of the neighbor faces.
  asiAlgo_EXPORT TColStd_PackedMapOfInteger
    GetNeighbors(const int                         face_idx,
                 const TColStd_PackedMapOfInteger& edge_ids) const;

  //! Returns full collection of neighbor faces.
  //! \return neighborhood data.
  asiAlgo_EXPORT const t_adjacency&
    GetNeighborhood() const;

  //! Returns all selected faces.
  //! \return collection of 1-based indices of the selected faces.
  asiAlgo_EXPORT const TColStd_PackedMapOfInteger&
    GetSelectedFaces() const;

  //! \return attributes associated with all graph arcs.
  asiAlgo_EXPORT const t_arc_attributes&
    GetArcAttributes() const;

  //! Accessor for an arc attribute.
  //! \param[in] arc graph arc in question.
  //! \return attribute associated with the given arc.
  asiAlgo_EXPORT const Handle(asiAlgo_FeatureAttr)&
    GetArcAttribute(const t_arc& arc) const;

  //! Checks whether the given node has any attributes or not.
  //! \param[in] node ID of the graph node to check.
  //! \return true/false.
  asiAlgo_EXPORT bool
    HasNodeAttributes(const int node) const;

  //! Accessor for the collection of nodal attributes.
  //! \param[in] node ID of the graph node of interest.
  //! \return attributes associated with the given graph node.
  asiAlgo_EXPORT const t_attr_set&
    GetNodeAttributes(const int node) const;

  //! Returns attribute associated with the given graph node.
  //! \param[in] node    ID of the graph node of interest.
  //! \param[in] attr_id ID of the attribute to access.
  //! \return attribute associated with the given node.
  asiAlgo_EXPORT Handle(asiAlgo_FeatureAttr)
    GetNodeAttribute(const int            node,
                     const Standard_GUID& attr_id) const;

  //! Sets the given attribute for a node in AAG. If an attribute of this type
  //! is already there, this method does nothing and returns false.
  //! \param[in] node ID of the graph node of interest.
  //! \param[in] attr attribute to set.
  asiAlgo_EXPORT bool
    SetNodeAttribute(const int                   node,
                     const Handle(asiAlgo_FeatureAttr)& attr);

  //! Searches for those faces having ALL neighbors attributed with convex link.
  //! \param[out] resultFaces found faces (if any).
  //! \return true if anything has been found, false -- otherwise.
  asiAlgo_EXPORT bool
    FindConvexOnly(TopTools_IndexedMapOfShape& resultFaces) const;

  //! Searches for those faces having ALL neighbors attributed with concave link.
  //! \param[out] resultFaces found faces (if any).
  //! \return true if anything has been found, false -- otherwise.
  asiAlgo_EXPORT bool
    FindConcaveOnly(TopTools_IndexedMapOfShape& resultFaces) const;

  //! Removes the passed faces with all corresponding arcs from AAG.
  //! \param[in] faces faces to remove.
  asiAlgo_EXPORT void
    Remove(const TopTools_IndexedMapOfShape& faces);

  //! Dumps AAG structure to the passed output stream. The format of dump
  //! is not well-structured. See `DumpJSON()` method for a more structured
  //! output.
  //! \param[in,out] out target stream.
  asiAlgo_EXPORT void
    Dump(Standard_OStream& out) const;

  //! Dumps AAG structure as JSON.
  //! \param[in,out] out target stream.
  asiAlgo_EXPORT void
    DumpJSON(Standard_OStream& out) const;

protected:

  //! Initializes graph tool with master CAD and selected faces.
  //! \param[in] masterCAD        master model (full CAD).
  //! \param[in] selectedFaces    selected faces.
  //! \param[in] allowSmooth      indicates whether "smooth" attribution for arcs
  //!                             is allowed (true) or not (false).
  //! \param[in] smoothAngularTol angular tolerance used for recognition
  //!                             of smooth dihedral angles. A smooth angle
  //!                             may appear to be imperfect by construction,
  //!                             but still smooth by the design intent. With
  //!                             this parameter you're able to control it.
  asiAlgo_EXPORT void
    init(const TopoDS_Shape&               masterCAD,
         const TopTools_IndexedMapOfShape& selectedFaces,
         const bool                        allowSmooth,
         const double                      smoothAngularTol);

  //! Fills graph with nodes for mate faces.
  //! \param[in] mateFaces faces to add (if not yet added).
  asiAlgo_EXPORT void
    addMates(const TopTools_ListOfShape& mateFaces);

protected:

  //! Dumps all graph nodes with their attributes to JSON.
  //! \param[in,out] out target output stream.
  asiAlgo_EXPORT void
    dumpNodesJSON(Standard_OStream& out) const;

  //! Dumps single graph node with its attributes to JSON.
  //! \param[in]     node    ID of the graph node to dump.
  //! \param[in]     isFirst indicates whether the currently dumped node is the
  //!                        first one to correctly put commas.
  //! \param[in,out] out     target output stream.
  asiAlgo_EXPORT void
    dumpNodeJSON(const int         node,
                 const bool        isFirst,
                 Standard_OStream& out) const;

  //! Dumps all graph arcs with their attributes to JSON.
  //! \param[in,out] out target output stream.
  asiAlgo_EXPORT void
    dumpArcsJSON(Standard_OStream& out) const;

  //! Dumps the given graph arc with its attributes to JSON.
  //! \param[in]     arc     graph arc in question.
  //! \param[in]     isFirst indicates whether the currently dumped arc is the
  //!                        first one to correctly put commas.
  //! \param[in,out] out     target output stream.
  asiAlgo_EXPORT void
    dumpArcJSON(const t_arc&      arc,
                const bool        isFirst,
                Standard_OStream& out) const;

protected:

  asiAlgo_AAG() {} //!< Default ctor.

protected:

  //! Master CAD model.
  TopoDS_Shape m_master;

  //! Selected faces. Selection is performed externally using any criterion
  //! which we do not care about here. One typical scenario is to select
  //! those faces corresponding to some feature in the model.
  TColStd_PackedMapOfInteger m_selected;

  //! All sub-shapes.
  TopTools_IndexedMapOfShape m_subShapes;

  //! All faces of the master model.
  TopTools_IndexedMapOfShape m_faces;

  //! All edges of the master model.
  TopTools_IndexedMapOfShape m_edges;

  //! All vertices of the master model.
  TopTools_IndexedMapOfShape m_vertices;

  //! This data map can be seen as adjacency matrix for graph.
  t_adjacency m_neighbors;

  //! Stores attributes associated with each arc.
  t_arc_attributes m_arc_attributes;

  //! Stores attributes associated with nodes.
  t_node_attributes m_node_attributes;

  //! Indicates whether to allow smooth transitions or not.
  bool m_bAllowSmooth;

  //! Angular tolerance to use for attribution of "smooth" dihedral edges.
  double m_fSmoothAngularTol;

};

#endif
