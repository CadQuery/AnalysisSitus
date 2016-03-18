//-----------------------------------------------------------------------------
// Created on: 10 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_aag_iterator.h>

//-----------------------------------------------------------------------------

//! Initializes iterator with graph.
//! \param graph [in] graph to iterate.
void geom_aag_iterator::Init(const Handle(geom_aag)& graph)
{
  m_graph = graph;
}

//! \return AAG being iterated.
const Handle(geom_aag)& geom_aag_iterator::GetGraph() const
{
  return m_graph;
}

//-----------------------------------------------------------------------------

//! Initializes iterator with graph.
//! \param graph [in] graph to iterate.
void geom_aag_random_iterator::Init(const Handle(geom_aag)& graph)
{
  geom_aag_iterator::Init(graph);
  m_it.Initialize(m_graph->m_neighbors);
}


//! \return true if there are still some faces to iterate.
bool geom_aag_random_iterator::More() const
{
  return m_it.More() > 0;
}

//! Moves iterator to another (not adjacent) face.
void geom_aag_random_iterator::Next()
{
  m_it.Next();
}

//! \return indices of the neighbor faces for the current face.
const TColStd_PackedMapOfInteger& geom_aag_random_iterator::GetNeighbors() const
{
  return m_it.Value();
}

//! \return ID of the current face.
int geom_aag_random_iterator::GetFaceId() const
{
  return m_it.Key();
}
