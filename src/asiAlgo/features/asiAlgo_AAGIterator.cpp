//-----------------------------------------------------------------------------
// Created on: 10 March 2016
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

// Own include
#include <asiAlgo_AAGIterator.h>

//-----------------------------------------------------------------------------

//! \return AAG being iterated.
const Handle(asiAlgo_AAG)& asiAlgo_AAGIterator::GetGraph() const
{
  return m_graph;
}

//! Initializes iterator with graph.
//! \param[in] graph graph to iterate.
void asiAlgo_AAGIterator::SetGraph(const Handle(asiAlgo_AAG)& graph)
{
  m_graph = graph;
}

//-----------------------------------------------------------------------------

//! Initializes iterator with graph.
//! \param[in] graph graph to iterate.
void asiAlgo_AAGRandomIterator::Init(const Handle(asiAlgo_AAG)& graph)
{
  asiAlgo_AAGIterator::SetGraph(graph);
  //
  m_it.Initialize( m_graph->GetNeighborhood() );
}


//! \return true if there are still some faces to iterate.
bool asiAlgo_AAGRandomIterator::More() const
{
  return m_it.More();
}

//! Moves iterator to another (not adjacent) face.
void asiAlgo_AAGRandomIterator::Next()
{
  m_it.Next();
}

//! Returns the neighbors of the current face.
//! \param[out] neighbors neighbors.
//! \return false is no neighbors available.
bool asiAlgo_AAGRandomIterator::GetNeighbors(TColStd_PackedMapOfInteger& neighbors) const
{
  neighbors = m_it.Value();
  return true;
}

//! \return ID of the current face.
int asiAlgo_AAGRandomIterator::GetFaceId() const
{
  return m_it.Key();
}

//-----------------------------------------------------------------------------

//! Initializes iterator with graph.
//! \param[in] graph graph to iterate.
//! \param[in] set   indices to iterate.
void asiAlgo_AAGSetIterator::Init(const Handle(asiAlgo_AAG)&        graph,
                                  const TColStd_PackedMapOfInteger& set)
{
  asiAlgo_AAGIterator::SetGraph(graph);
  //
  m_it.Initialize(set);
}


//! \return true if there are still some faces to iterate.
bool asiAlgo_AAGSetIterator::More() const
{
  return m_it.More();
}

//! Moves iterator to another (not adjacent) face.
void asiAlgo_AAGSetIterator::Next()
{
  m_it.Next();
}

//! Returns the neighbors of the current face.
//! \param[out] neighbors neighbors.
//! \return false is no neighbors available.
bool asiAlgo_AAGSetIterator::GetNeighbors(TColStd_PackedMapOfInteger& neighbors) const
{
  const int face_id = m_it.Key();
  if ( !m_graph->HasNeighbors(face_id) )
    return false;

  neighbors = m_graph->GetNeighbors(face_id);
  return true;
}

//! \return ID of the current face.
int asiAlgo_AAGSetIterator::GetFaceId() const
{
  return m_it.Key();
}
