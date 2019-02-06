//-----------------------------------------------------------------------------
// Created on: 05 February 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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

#ifndef asiAlgo_FrozenVertices_h
#define asiAlgo_FrozenVertices_h

// asiAlgo includes
#include <asiAlgo.h>

// OCCT includes
#include <TopTools_IndexedMapOfShape.hxx>

//-----------------------------------------------------------------------------

//! Vertices which are not meant to be recomputed when geometry is normalized.
//! This structure is a part of knowledge-guided computation principle
//! used in defeaturing tools.
struct asiAlgo_FrozenVertices
{
  TopTools_IndexedMapOfShape vertices; //!< Vertices to keep intact.

  //! Default ctor.
  asiAlgo_FrozenVertices() {}

  //! Ctor.
  asiAlgo_FrozenVertices(const TopTools_IndexedMapOfShape& _vertices) : vertices(_vertices) {}

  //! Adds another vertex to the collection of frozen ones.
  void Add(const TopoDS_Shape& vertex)
  {
    vertices.Add(vertex);
  }

  //! Accessor for a vertex by its index.
  //! \param[in] oneBasedIdx 1-based ID of the vertex to access.
  //! \return reference to the vertex.
  const TopoDS_Shape& operator()(const int oneBasedIdx) const
  {
    return vertices(oneBasedIdx);
  }

  //! \return number of stored vertices.
  int GetNumOfVertices() const
  {
    return vertices.Extent();
  }

  //! Creates another map of frozen vertices as an intersection between `this`
  //! map and the passed `op` map.
  //! \param[in] op operand map.
  //! \return new collection of frozen vertices.
  asiAlgo_FrozenVertices GetIntersection(const asiAlgo_FrozenVertices& op) const
  {
    asiAlgo_FrozenVertices result;

    for ( int j = 1; j <= op.vertices.Extent(); ++j )
      if ( this->vertices.Contains( op.vertices(j) ) )
        result.Add( op.vertices(j) );

    return result;
  }
};

#endif
