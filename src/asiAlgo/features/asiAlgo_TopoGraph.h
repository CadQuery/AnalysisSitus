//-----------------------------------------------------------------------------
// Created on: 08 September 2017 (*)
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

#ifndef asiAlgo_TopoGraph_h
#define asiAlgo_TopoGraph_h

// asiAlgo includes
#include <asiAlgo_TopoAttr.h>

// OCCT includes
#include <NCollection_DataMap.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//-----------------------------------------------------------------------------

//! \brief Topology graph for any B-Rep part.
//!
//! This tool gives clear TopoDS-agnostic interface to the graph representing
//! the topological structure of a shape. A graph is essentially a set of
//! nodes {N} and a set of arcs {A} as defined formally.
//!
//! <pre>
//!   G = <N, A>
//! </pre>
//!
//! Using this tool, you can map B-Rep model to a formal graph structure.
//! All nodes of the graph are indexed on population. These indices are
//! 1-based and are equal to the global IDs of the corresponding sub-shapes
//! in the original CAD part.
//!
//! \note Being TopoDS-agnostic does not mean "OpenCascade-agnostic". Each
//!       node in the graph has a type corresponding to one or another
//!       topological type of OpenCascade. These types and the relations
//!       between them are not the same in different geometric kernels.
class asiAlgo_TopoGraph : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_TopoGraph, Standard_Transient)

public:

  //! \brief Graph iterator.
  class Iterator
  {
  public:

    //! Default ctor.
    Iterator() : m_iCurrentIndex(0) {}

    //! ctor accepting the topology graph to iterate.
    //! \param[in] graph topology graph to iterate.
    Iterator(const Handle(asiAlgo_TopoGraph)& graph)
    {
      this->Init(graph);
    }

  public:

    //! Initializes iterator with topology graph.
    //! \param[in] graph topology graph to iterate.
    void Init(const Handle(asiAlgo_TopoGraph)& graph)
    {
      m_graph         = graph;
      m_iCurrentIndex = 1;
    }

    //! Checks if there are more graph nodes to iterate.
    //! \return true/false.
    bool More() const
    {
      return m_iCurrentIndex <= m_graph->GetNodes().Extent();
    }

    //! \return 1-based ID of the current node.
    int GetCurrentIndex() const
    {
      return m_iCurrentIndex;
    }

    //! \return current node as topological shape.
    const TopoDS_Shape& GetCurrentNode() const
    {
      return m_graph->GetNode(m_iCurrentIndex);
    }

    //! Moves iterator to the next position.
    void Next()
    {
      ++m_iCurrentIndex;
    }

  protected:

    Handle(asiAlgo_TopoGraph) m_graph;         //!< Topology graph to iterate.
    int                       m_iCurrentIndex; //!< Current 1-based node ID.

  };

public:

  //! Hasher which does not take into account neither locations nor
  //! orientations of shapes. Our killer is extremely cruel in this regard...
  class t_partner_hasher
  {
  public:

    static int HashCode(const TopoDS_Shape& S, const int Upper)
    {
      const int I  = (int) ptrdiff_t( S.TShape().operator->() );
      const int HS = ::HashCode(I, Upper);
      //
      return HS;
    }

    static bool IsEqual(const TopoDS_Shape& S1, const TopoDS_Shape& S2)
    {
      return S1.IsPartner(S2);
    }
  };

  //! Type definition for indexed nodes.
  typedef NCollection_IndexedMap<TopoDS_Shape, t_partner_hasher> t_nodes;

  //---------------------------------------------------------------------------

  //! Type definition for graph adjacency matrix. This is how shape/sub-shape
  //! links are realized in the topology graph.
  typedef NCollection_DataMap<int, TColStd_PackedMapOfInteger> t_adjacency;

  //---------------------------------------------------------------------------

  //! Arc between two nodes of topology graph. The arc is the explicit
  //! representation for containment relation.
  struct t_arc
  {
    int iParent; //!< Parent shape.
    int iChild;  //!< Child shape.

    //! ctor default.
    t_arc() : iParent(0), iChild(0) {}

    //! ctor with parameters.
    t_arc(const int _iParent, const int _iChild) : iParent(_iParent), iChild(_iChild) {}

    //! \return hash code for the arc.
    static int HashCode(const t_arc& arc, const int upper)
    {
      int key = arc.iParent + arc.iChild;
      key += (key << 10);
      key ^= (key >> 6);
      key += (key << 3);
      key ^= (key >> 11);
      return (key & 0x7fffffff) % upper;
    }

    //! \return true if two links are equal.
    static int IsEqual(const t_arc& arc1, const t_arc& arc2)
    {
      return arc1.iParent == arc2.iParent && arc1.iChild == arc2.iChild;
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
    t_attr_set(const Handle(asiAlgo_TopoAttr)& single_attr)
    {
      this->Add(single_attr);
    }

  public:

    //! Finds attribute by its global type ID.
    //! \param[in] id attribute's global ID.
    //! \return attribute instance.
    const Handle(asiAlgo_TopoAttr)& operator()(const Standard_GUID& id) const
    {
      return m_set(id);
    }

    //! Adds the given attribute to the set.
    //! \param[in] attr attribute to add.
    void Add(const Handle(asiAlgo_TopoAttr)& attr)
    {
      m_set.Bind(attr->GetGUID(), attr);
    }

    //! \return internal collection.
    const NCollection_DataMap<Standard_GUID, Handle(asiAlgo_TopoAttr), Standard_GUID>& GetMap() const
    {
      return m_set;
    }

    //! Checks whether the attribute set already contains an attribute with
    //! the given GUID.
    //! \param[in] guid GUID of the attribute to check.
    //! \return true/false.
    bool Contains(const Standard_GUID& guid) const
    {
      return m_set.IsBound(guid);
    }

  private:

    //! Internal set storing attributes in association with their global IDs.
    NCollection_DataMap<Standard_GUID, Handle(asiAlgo_TopoAttr), Standard_GUID> m_set;

  };

  //---------------------------------------------------------------------------

  //! Arc attributes (e.g. to store orientation of sub-shapes).
  typedef NCollection_DataMap<t_arc, t_attr_set, t_arc> t_arc_attributes;

  //! Node attributes.
  typedef NCollection_DataMap<int, t_attr_set> t_node_attributes;

public:

  //! \brief Initializes topology graph from shape.
  //!
  //! Construction of a formal graph will be done immediately at ctor.
  //!
  //! \param[in] shape B-Rep shape to represent as graph.
  asiAlgo_EXPORT
    asiAlgo_TopoGraph(const TopoDS_Shape& shape);

public:

  //! \brief Returns map of indexed sub-shapes of the given type.
  //!
  //! \param[in]  ssType sub-shape type (TopAbs_VERTEX, TopAbs_EDGE or TopAbs_FACE).
  //! \param[out] map    requested map of sub-shapes.
  asiAlgo_EXPORT void
    GetMapOf(const TopAbs_ShapeEnum      ssType,
             TopTools_IndexedMapOfShape& map) const;

  //! \brief Returns map of indexed faces.
  asiAlgo_EXPORT const TopTools_IndexedMapOfShape&
    GetMapOfFaces() const;

  //! \brief Returns map of indexed edges.
  asiAlgo_EXPORT const TopTools_IndexedMapOfShape&
    GetMapOfEdges() const;

  //! \brief Returns map of indexed vertices.
  asiAlgo_EXPORT const TopTools_IndexedMapOfShape&
    GetMapOfVertices() const;

  //! \brief Returns map of all indexed sub-shapes.
  asiAlgo_EXPORT const TopTools_IndexedMapOfShape&
    GetMapOfSubShapes() const;

  //! \brief Dumps graph structure to output stream.
  //!
  //! The output format is DOT. You may use graph rendering tools like
  //! Graphviz to parse the output.
  //!
  //! \param[out] out output stream.
  asiAlgo_EXPORT void
    Dump(Standard_OStream& out) const;

  //! \brief Calculates short summary for the graph.
  //!
  //! Short summary gives you the total number of nodes of a particular
  //! type.
  //!
  //! \param[out] numCompounds  number of compounds.
  //! \param[out] numCompSolids number of compsolids.
  //! \param[out] numSolids     number of solids.
  //! \param[out] numShells     number of shells.
  //! \param[out] numFaces      number of faces.
  //! \param[out] numWires      number of wires.
  //! \param[out] numEdges      number of edges.
  //! \param[out] numVertices   number of vertices.
  asiAlgo_EXPORT void
    CalculateSummary(int& numCompounds,
                     int& numCompSolids,
                     int& numSolids,
                     int& numShells,
                     int& numFaces,
                     int& numWires,
                     int& numEdges,
                     int& numVertices) const;

  //! Constructs a sub-graph containing topological elements of the given
  //! shape only.
  //!
  //! \param[in] shape shape to construct a sub-graph for.
  //!
  //! \return new graph which is a sub-graph corresponding to the passed shape.
  asiAlgo_EXPORT Handle(asiAlgo_TopoGraph)
    SubGraph(const TopoDS_Shape& shape) const;

public:

  //! \return master CAD.
  const TopoDS_Shape& GetMasterCAD() const
  {
    return m_model;
  }

  //! \brief Returns ID of the root node.
  //! \return ID of the root node.
  int GetRoot() const
  {
    return m_iRoot;
  }

  //! \brief Finds all parent nodes for the given node (1-based index).
  //! \param[in]  node        ID of the node to check (one-based is expected).
  //! \param[out] parentNodes found parent nodes.
  //! \return false if not parents found.
  bool FindParents(const int                   node,
                   TColStd_PackedMapOfInteger& parentNodes) const
  {
    if ( !this->HasNode(node) )
      return false;

    // Loop over the adjacency graph to find parents by part-child relations.
    for ( t_adjacency::Iterator ait(m_arcs); ait.More(); ait.Next() )
    {
      const TColStd_PackedMapOfInteger& tuple = ait.Value();

      if ( tuple.Contains(node) )
        parentNodes.Add( ait.Key() );
    }

    return !parentNodes.IsEmpty();
  }

  //! \brief Checks whether the topology graph contains a node with
  //!        the given one-based index.
  //! \param[in] n ID of the node to check (one-based is expected).
  //! \return true/false.
  bool HasNode(const int node) const
  {
    return node >= 1 && node <= m_nodes.Extent();
  }

  //! \brief Returns topological item (TopoDS_Shape) by its index.
  //! \param[in] node 1-based index of the topological node in question.
  //! \return node as shape.
  const TopoDS_Shape& GetNode(const int node) const
  {
    return m_nodes(node);
  }

  //! \brief Returns index of the node which corresponds to the
  //!        passed (sub-)shape.
  //! \param[in] shape (sub-)shape in question.
  //! \return one-based index of the graph node or zero if no such node exists.
  int GetNodeIndex(const TopoDS_Shape& shape) const
  {
    if ( !m_nodes.Contains(shape) )
      return 0;

    return m_nodes.FindIndex(shape);
  }

  //! \brief Checks whether the topology graph contains (n1, n2) directed arc.
  //! \param[in] n1 one-based ID of the first node.
  //! \param[in] n2 one-based ID of the second node.
  //! \return true/false.
  bool HasArc(const int n1, const int n2) const
  {
    if ( !this->HasChildren(n1) )
      return false;

    return this->GetChildren(n1).Contains(n2);
  }

  //! \brief Checks whether children exist for the given node.
  //! \param[in] oneBasedNodeId one-based node ID.
  //! \return true/false.
  bool HasChildren(const int oneBasedNodeId) const
  {
    return m_arcs.IsBound(oneBasedNodeId);
  }

  //! \brief Returns IDs of child nodes for the given node.
  //! \param[in] oneBasedNodeId one-based node ID.
  //! \return set of child IDs.
  const TColStd_PackedMapOfInteger& GetChildren(const int oneBasedNodeId) const
  {
    return m_arcs(oneBasedNodeId);
  }

  //! \brief returns shape by node ID.
  //! \param[in] oneBasedNodeId one-based node ID.
  //! \return shape.
  const TopoDS_Shape& GetShape(const int oneBasedNodeId) const
  {
    return m_nodes(oneBasedNodeId);
  }

  //! \brief returns shape type by node ID.
  //! \param[in] oneBasedNodeId one-based node ID.
  //! \return shape type.
  TopAbs_ShapeEnum GetNodeType(const int oneBasedNodeId) const
  {
    return this->GetShape(oneBasedNodeId).ShapeType();
  }

  //! \brief Returns the unordered set of graph nodes.
  //! \return graph nodes.
  const NCollection_IndexedMap<TopoDS_Shape, t_partner_hasher>& GetNodes() const
  {
    return m_nodes;
  }

  //! \brief Returns the number of graph nodes.
  //! \return number of graph nodes.
  int GetNumberOfNodes() const
  {
    return m_nodes.Extent();
  }

  //! \brief Returns the collection of graph arcs in form of adjacency matrix.
  //! \return graph arcs.
  const t_adjacency& GetArcs() const
  {
    return m_arcs;
  }

  //! \brief Returns the number of graph arcs.
  //! \return number of graph arcs.
  int GetNumberOfArcs() const
  {
    int numArcs = 0;
    //
    for ( t_adjacency::Iterator it(m_arcs); it.More(); it.Next() )
      numArcs += it.Value().Extent();

    return numArcs;
  }

  //! Returns arc attribute.
  //! \param[in] arc  arc in question.
  //! \param[in] guid GUID of the attribute in question.
  //! \return attribute.
  Handle(asiAlgo_TopoAttr) GetArcAttribute(const t_arc&         arc,
                                           const Standard_GUID& guid) const
  {
    if ( !m_arc_attributes.IsBound(arc) )
      return NULL;

    if ( !m_arc_attributes(arc).Contains(guid) )
      return NULL;

    return m_arc_attributes(arc)(guid);
  }

  //! Returns node attribute.
  //! \param[in] node 1-based node in question.
  //! \param[in] guid GUID of the attribute in question.
  //! \return attribute.
  Handle(asiAlgo_TopoAttr) GetNodeAttribute(const int            node,
                                            const Standard_GUID& guid) const
  {
    if ( !m_node_attributes.IsBound(node) )
      return NULL;

    if ( !m_node_attributes(node).Contains(guid) )
      return NULL;

    return m_node_attributes(node)(guid);
  }

  //! Adds the passed attribute to the collection of nodal attributes
  //! associated with the given graph node.
  //! \param[in] node 1-based index of the graph node in question.
  //! \param[in] attr graph attribute to add.
  //! \return true in case of success, false -- otherwise.
  bool AddNodeAttribute(const int                       node,
                        const Handle(asiAlgo_TopoAttr)& attr)
  {
    if ( m_node_attributes.IsBound(node) &&
         m_node_attributes(node).Contains( attr->GetGUID() ) )
      return false; // Such attribute already exists.

    // Add attribute.
    if ( !m_node_attributes.IsBound(node) )
      m_node_attributes.Bind( node, t_attr_set() );
    //
    m_node_attributes(node).Add(attr);

    return true;
  }

protected:

  //! Builds graph out of TopoDS_Shape structure.
  asiAlgo_EXPORT void
    buildGraph();

  //! Adds sub-shapes for the given root in the graph structure.
  //! \param[in] parent    parent object.
  //! \param[in] iParentId ID of the already registered node
  //!                      representing the parent object in the topology
  //!                      graph being populated.
  asiAlgo_EXPORT void
    addSubshapes(const TopoDS_Shape& parent,
                 const int           iParentId);

// INPUTS
protected:

  TopoDS_Shape m_model; //!< Shape to represent as graph.

// OUTPUTS
protected:

  /** @name Core members
   *  Members defining the topology graph with attributes.
   */
  //@{
  int               m_iRoot;           //!< ID of the root node.
  t_nodes           m_nodes;           //!< Graph nodes.
  t_adjacency       m_arcs;            //!< Shape/sub-shape relations.
  t_node_attributes m_node_attributes; //!< Stores attributes associated with each node.
  t_arc_attributes  m_arc_attributes;  //!< Stores attributes associated with each arc.
  //@}

  /** @name Cached sub-shapes
   *  Members storing the cached sub-shapes with indices.
   */
  //@{
  TopTools_IndexedMapOfShape  m_subShapes; //!< All sub-shapes.
  TopTools_IndexedMapOfShape  m_faces;     //!< All faces of the master model.
  TopTools_IndexedMapOfShape  m_edges;     //!< All edges of the master model.
  TopTools_IndexedMapOfShape  m_vertices;  //!< All vertices of the master model.
  //@}

};

#endif
