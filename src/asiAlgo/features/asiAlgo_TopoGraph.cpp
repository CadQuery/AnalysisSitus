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

// Own include
#include <asiAlgo_TopoGraph.h>

// OCCT includes
#include <TCollection_AsciiString.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopoDS_Iterator.hxx>

//-----------------------------------------------------------------------------

#define NodeLetter "N"
#define Whitespace "    "

//-----------------------------------------------------------------------------

asiAlgo_TopoGraph::asiAlgo_TopoGraph(const TopoDS_Shape& shape)
: Standard_Transient (),
  m_model            (shape)
{
  this->buildGraph();
}

//-----------------------------------------------------------------------------

//! \return map of faces.
const TopTools_IndexedMapOfShape& asiAlgo_TopoGraph::GetMapOfFaces() const
{
  return m_faces;
}

//-----------------------------------------------------------------------------

//! \return map of edges.
const TopTools_IndexedMapOfShape& asiAlgo_TopoGraph::GetMapOfEdges() const
{
  return m_edges;
}

//-----------------------------------------------------------------------------

//! \return map of vertices.
const TopTools_IndexedMapOfShape& asiAlgo_TopoGraph::GetMapOfVertices() const
{
  return m_vertices;
}

//-----------------------------------------------------------------------------

//! \return map of sub-shapes.
const TopTools_IndexedMapOfShape& asiAlgo_TopoGraph::GetMapOfSubShapes() const
{
  return m_subShapes;
}

//-----------------------------------------------------------------------------

void asiAlgo_TopoGraph::Dump(Standard_OStream& out) const
{
  // Directed graph header
  out << "digraph asiAlgo_TopoGraph {\n";
  out << "\n";

  // Dump nodes with attributes
  const NCollection_IndexedMap<TopoDS_Shape>& nodes = this->GetNodes();
  //
  for ( int n = 1; n <= nodes.Extent(); ++n )
  {
    // Generate label
    TCollection_AsciiString label;
    label += "\\n"; label += nodes(n).TShape()->DynamicType()->Name();

    // Dump node with label
    out << Whitespace << NodeLetter << n << " [label=\"" << label.ToCString() << "\"];\n";
  }
  out << "\n";

  // Dump arcs
  for ( t_adjacency::Iterator it(m_arcs); it.More(); it.Next() )
  {
    const int                         parentId = it.Key();
    const TColStd_PackedMapOfInteger& children = it.Value();

    // Loop over the children
    for ( TColStd_MapIteratorOfPackedMapOfInteger cit(children); cit.More(); cit.Next() )
    {
      const int childId = cit.Key();

      out << Whitespace
          << NodeLetter << parentId
          << " -> "
          << NodeLetter << childId
          << ";\n";
    }
  }

  out << "\n";
  out << "}\n";
}

//-----------------------------------------------------------------------------

void asiAlgo_TopoGraph::CalculateSummary(int& numCompounds,
                                         int& numCompSolids,
                                         int& numSolids,
                                         int& numShells,
                                         int& numFaces,
                                         int& numWires,
                                         int& numEdges,
                                         int& numVertices) const
{
  numCompounds  = 0;
  numCompSolids = 0;
  numSolids     = 0;
  numShells     = 0;
  numFaces      = 0;
  numWires      = 0;
  numEdges      = 0;
  numVertices   = 0;

  // Loop over the nodes
  for ( int n = 1; n <= this->GetNodes().Extent(); ++n )
  {
    switch ( m_nodes(n).ShapeType() )
    {
      case TopAbs_COMPOUND:  ++numCompounds;  break;
      case TopAbs_COMPSOLID: ++numCompSolids; break;
      case TopAbs_SOLID:     ++numSolids;     break;
      case TopAbs_SHELL:     ++numShells;     break;
      case TopAbs_FACE:      ++numFaces;      break;
      case TopAbs_WIRE:      ++numWires;      break;
      case TopAbs_EDGE:      ++numEdges;      break;
      case TopAbs_VERTEX:    ++numVertices;   break;
      default: break;
    }
  }
}

//-----------------------------------------------------------------------------

void asiAlgo_TopoGraph::buildGraph()
{
  // Extract all sub-shapes with unique indices from the master CAD
  TopExp::MapShapes(m_model, m_subShapes);

  // Extract all faces with unique indices from the master CAD
  TopExp::MapShapes(m_model, TopAbs_FACE, m_faces);

  // Extract all edges with unique indices from the master CAD
  TopExp::MapShapes(m_model, TopAbs_EDGE, m_edges);

  // Extract all vertices with unique indices from the master CAD
  TopExp::MapShapes(m_model, TopAbs_VERTEX, m_vertices);

  // Add root node
  const int iObjectId = m_nodes.Add(m_model);

  // Mark as root
  m_iRoot = iObjectId;

  // Add sub-shapes (the objects nested into the current one)
  this->addSubshapes(m_model, iObjectId);
}

//-----------------------------------------------------------------------------

void asiAlgo_TopoGraph::addSubshapes(const TopoDS_Shape& parent,
                                     const int           iParentId)
{
  // Iterate over the sub-shapes
  for ( TopoDS_Iterator it(parent, false, false); it.More(); it.Next() )
  {
    const TopoDS_Shape& subShape = it.Value();

    // Add node
    const int iChildId = m_nodes.Add(subShape);

    // Add arc
    if ( !m_arcs.IsBound(iParentId) )
      m_arcs.Bind( iParentId, TColStd_PackedMapOfInteger() );
    //
    m_arcs(iParentId).Add(iChildId);

    // Process children: add sub-shapes recursively
    if ( !subShape.IsNull() )
      this->addSubshapes(subShape, iChildId);
  }
}
