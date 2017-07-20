//-----------------------------------------------------------------------------
// Created on: 10 March 2016
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
  m_it.Initialize(m_graph->m_neighbors);
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
