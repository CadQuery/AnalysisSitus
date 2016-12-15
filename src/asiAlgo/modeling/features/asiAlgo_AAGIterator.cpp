//-----------------------------------------------------------------------------
// Created on: 10 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
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
