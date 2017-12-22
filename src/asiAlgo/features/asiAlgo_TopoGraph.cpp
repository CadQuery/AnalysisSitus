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

// Own include
#include <asiAlgo_TopoGraph.h>

// asiAlgo includes
#include <asiAlgo_TopoAttrLocation.h>
#include <asiAlgo_TopoAttrOrientation.h>
#include <asiAlgo_Utils.h>

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

void asiAlgo_TopoGraph::GetMapOf(const TopAbs_ShapeEnum      ssType,
                                 TopTools_IndexedMapOfShape& map) const
{
  switch ( ssType )
  {
    case TopAbs_VERTEX:
      map = this->GetMapOfVertices();
      break;
    case TopAbs_EDGE:
      map = this->GetMapOfEdges();
      break;
    case TopAbs_FACE:
      map = this->GetMapOfFaces();
      break;
    default: break;
  }
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
  const NCollection_IndexedMap<TopoDS_Shape, t_partner_hasher>& nodes = this->GetNodes();
  //
  for ( int n = 1; n <= nodes.Extent(); ++n )
  {
    // Generate label
    TCollection_AsciiString label;
    label += nodes(n).TShape()->DynamicType()->Name();

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

      // Get orientation attribute
      Handle(asiAlgo_TopoAttr)
        oriAttrBase = this->GetArcAttribute( t_arc(parentId, childId),
                                             asiAlgo_TopoAttrOrientation::GUID() );
      //
      Handle(asiAlgo_TopoAttrOrientation)
        oriAttr = Handle(asiAlgo_TopoAttrOrientation)::DownCast(oriAttrBase);

      // Generate label
      TCollection_AsciiString label;
      label += asiAlgo_Utils::OrientationToString( oriAttr->GetOrientation() );

      out << Whitespace
          << NodeLetter << parentId
          << " -> "
          << NodeLetter << childId
          << " [label=\"" << label.ToCString() << "\"]"
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

    // Add or take the existing node. The nodes in the graph correspond to
    // topological elements with identical TShapes.
    int iChildId;
    //
    if ( !m_nodes.Contains(subShape) )
      iChildId = m_nodes.Add(subShape);
    else
      iChildId = m_nodes.FindIndex(subShape);

    // Add arc
    if ( !m_arcs.IsBound(iParentId) )
      m_arcs.Bind( iParentId, TColStd_PackedMapOfInteger() );
    //
    m_arcs(iParentId).Add(iChildId);

    // Bind orientation & location attributes
    t_attr_set attrSet;
    {
      Handle(asiAlgo_TopoAttrOrientation)
        oriAttr = new asiAlgo_TopoAttrOrientation( subShape.Orientation() );
      //
      attrSet.Add(oriAttr);
    }
    //
    {
      if ( !subShape.Location().IsIdentity() )
      {
        Handle(asiAlgo_TopoAttrLocation)
          locAttr = new asiAlgo_TopoAttrLocation( subShape.Location() );
        //
        attrSet.Add(locAttr);
      }
    }
    m_arc_attributes.Bind(t_arc(iParentId, iChildId), attrSet);

    // Process children: add sub-shapes recursively
    if ( !subShape.IsNull() )
      this->addSubshapes(subShape, iChildId);
  }
}
