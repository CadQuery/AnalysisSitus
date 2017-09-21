//-----------------------------------------------------------------------------
// Created on: 08 September 2017 (*)
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

#ifndef asiAlgo_TopoGraph_h
#define asiAlgo_TopoGraph_h

// asiAlgo includes
#include <asiAlgo.h>

// OCCT includes
#include <NCollection_DataMap.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

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
    int GetCurrentNode() const
    {
      return m_iCurrentIndex;
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

  //! Arc attributes to store orientation of sub-shape in its parent shape.
  typedef NCollection_DataMap<t_arc, TopAbs_Orientation, t_arc> t_arc_attributes;

public:

  //! \brief Initializes topology graph from shape.
  //!
  //! Construction of a formal graph will be done immediately at ctor.
  //!
  //! \param[in] shape B-Rep shape to represent as graph.
  asiAlgo_EXPORT
    asiAlgo_TopoGraph(const TopoDS_Shape& shape);

public:

  asiAlgo_EXPORT const TopTools_IndexedMapOfShape&
    GetMapOfFaces() const;

  asiAlgo_EXPORT const TopTools_IndexedMapOfShape&
    GetMapOfEdges() const;

  asiAlgo_EXPORT const TopTools_IndexedMapOfShape&
    GetMapOfVertices() const;

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

public:

  //! \brief Returns ID of the root node.
  //! \return ID of the root node.
  int GetRoot() const
  {
    return m_iRoot;
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

  //! \brief Returns the unordered set of graph nodes.
  //! \return graph nodes.
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& GetNodes() const
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

  //! Returns arc attribute (orientation).
  //! \param[in] arc arc in question.
  //! \return attribute.
  TopAbs_Orientation GetArcAttribute(const t_arc& arc) const
  {
    return m_arc_attributes(arc);
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

  int                                                           m_iRoot;          //!< ID of the root node.
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> m_nodes;          //!< Graph nodes.
  t_adjacency                                                   m_arcs;           //!< Shape/sub-shape relations.
  t_arc_attributes                                              m_arc_attributes; //!< Stores attributes associated with each arc.
  TopTools_IndexedMapOfShape                                    m_subShapes;      //!< All sub-shapes.
  TopTools_IndexedMapOfShape                                    m_faces;          //!< All faces of the master model.
  TopTools_IndexedMapOfShape                                    m_edges;          //!< All edges of the master model.
  TopTools_IndexedMapOfShape                                    m_vertices;       //!< All vertices of the master model.

};

#endif
