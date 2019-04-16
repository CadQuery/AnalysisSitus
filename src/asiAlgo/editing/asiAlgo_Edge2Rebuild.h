//-----------------------------------------------------------------------------
// Created on: 13 November 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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

#ifndef asiAlgo_Edge2Rebuild_h
#define asiAlgo_Edge2Rebuild_h

// asiAlgo includes
#include <asiAlgo_FrozenVertices.h>
#include <asiAlgo_History.h>

// OCCT includes
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//-----------------------------------------------------------------------------

//! Descriptor of an edge which should be rebuilt at resolution stage for a
//! certain topological condition. Generally, one can rebuild an edge entirely.
//! However, in some conditions, it may not be necessary to reconstruct some
//! vertices and hence one can avoid running computationally expensive and
//! unreliable geometric algorithms (e.g. curve-surface intersection).
struct asiAlgo_Edge2Rebuild
{
  TopoDS_Edge            edge;           //!< Edge to rebuild.
  asiAlgo_FrozenVertices frozenVertices; //!< Vertices to keep intact.

  // Ctor.
  explicit asiAlgo_Edge2Rebuild(const TopoDS_Edge& _edge) : edge(_edge) {}

  // Ctor.
  asiAlgo_Edge2Rebuild(const TopoDS_Edge&   _edge,
                       const TopoDS_Vertex& _vertex,
                       const bool           _frozen)
  : edge(_edge)
  {
    if ( _frozen )
    {
      frozenVertices.Add(_vertex);
    }
    else
    {
      TopoDS_Vertex vf, vl;
      TopExp::Vertices(_edge, vf, vl);

      // Add opposite vertex to be frozen.
      if ( _vertex.IsPartner(vf) )
        frozenVertices.Add(vl);
      else if ( _vertex.IsPartner(vl) )
        frozenVertices.Add(vf);
    }
  }

  //! Actualizes the stored boundary elements w.r.t. the passed history.
  //! \param[in] history editing history for actualization.
  void Actualize(const Handle(asiAlgo_History)& history)
  {
    // Update edge.
    if ( history->IsDeleted(this->edge) )
    {
      this->edge.Nullify();
      this->frozenVertices.vertices.Clear();
    }
    else
    {
      this->edge = TopoDS::Edge( history->GetLastModifiedOrArg(this->edge) );

      // Update frozen vertices.
      asiAlgo_FrozenVertices updatedVertices;
      //
      for ( int vidx = 1; vidx <= this->frozenVertices.GetNumOfVertices(); ++vidx )
      {
        TopoDS_Shape vertex = history->GetLastModifiedOrArg( this->frozenVertices(vidx) );
        updatedVertices.Add(vertex);
      }
      //
      this->frozenVertices = updatedVertices;
    }
  }

  //! Hasher for data maps.
  struct Hasher
  {
    static int HashCode(const asiAlgo_Edge2Rebuild& e, const int upper)
    {
      return ::HashCode(e.edge, upper);
    }

    static bool IsEqual(const asiAlgo_Edge2Rebuild& e1, const asiAlgo_Edge2Rebuild& e2)
    {
      return ::IsEqual(e1.edge, e2.edge);
    }
  };
};

//-----------------------------------------------------------------------------

//! Collection of edges to rebuild at geometry normalization stage. This
//! structure not only wraps the underlying map of edges, but also controls
//! frozen vertices. I.e., if one edge is added several times, this structure
//! will check that the edge occurrences all have identical sets of vertices
//! declared as "frozen". Only those vertices which belong to all occurrences
//! will remain.
struct asiAlgo_Edges2Rebuild
{
  NCollection_IndexedMap<asiAlgo_Edge2Rebuild,
                         asiAlgo_Edge2Rebuild::Hasher> edges;

  //! Adds another edge while controlling the frozen vertices.
  //! \param[in] newEdge2Rebuild element to add.
  void Add(const asiAlgo_Edge2Rebuild& newEdge2Rebuild)
  {
    if ( !this->edges.Contains(newEdge2Rebuild) )
    {
      this->edges.Add(newEdge2Rebuild);
      return;
    }

    // If we are here, it means that there is already such edge in the map.
    // We now check that the maps of frozen vertices stored in both edge
    // occurrences intersect. Only intersection will be stored.

    const int storedEdgeIdx = this->edges.FindIndex(newEdge2Rebuild);

    // Get stored occurrence.
    const asiAlgo_Edge2Rebuild& storedEdge2Rebuild = this->edges.FindKey(storedEdgeIdx);

    // Narrow down the collection of frozen vertices.
    asiAlgo_FrozenVertices
      frozenVerts = storedEdge2Rebuild.frozenVertices.GetIntersection(newEdge2Rebuild.frozenVertices);

    // Prepare another edge2Rebuild.
    asiAlgo_Edge2Rebuild edge2Rebuild(newEdge2Rebuild.edge);
    edge2Rebuild.frozenVertices = frozenVerts;

    // Store.
    edges.Substitute(storedEdgeIdx, edge2Rebuild);
  }
};

#endif
