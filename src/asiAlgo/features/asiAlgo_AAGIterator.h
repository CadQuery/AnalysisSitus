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

#ifndef asiAlgo_AAGIterator_h
#define asiAlgo_AAGIterator_h

// asiAlgo includes
#include <asiAlgo_AAG.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

// Standard includes
#include <stack>

//-----------------------------------------------------------------------------

//! AAG iterator.
class asiAlgo_AAGIterator : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_AAGIterator, Standard_Transient)

public:

  //! Default constructor.
  asiAlgo_AAGIterator() : Standard_Transient()
  {}

  //! Creates and initializes iterator for AAG.
  //! \param[in] graph graph to iterate.
  asiAlgo_AAGIterator(const Handle(asiAlgo_AAG)& graph) : Standard_Transient()
  {
    this->SetGraph(graph);
  }

public:

  asiAlgo_EXPORT const Handle(asiAlgo_AAG)&
    GetGraph() const;

  asiAlgo_EXPORT void
    SetGraph(const Handle(asiAlgo_AAG)& graph);

public:

  //! \return true if current AAG exist.
  virtual bool
    More() const = 0;

  //! Selects next AAG.
  virtual void
    Next() = 0;

  //! Returns the neighbors of the current face.
  //! \param[out] neighbors neighbors.
  //! \return false is no neighbors available.
  virtual bool
    GetNeighbors(TColStd_PackedMapOfInteger& neighbors) const = 0;

  //! \return face ID of current AAG.
  virtual int
    GetFaceId() const = 0;

protected:

  Handle(asiAlgo_AAG) m_graph; //!< AAG being iterated.

};

//-----------------------------------------------------------------------------

//! Random-order AAG iterator.
class asiAlgo_AAGRandomIterator : public asiAlgo_AAGIterator
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_AAGRandomIterator, asiAlgo_AAGIterator)

public:

  //! Creates and initializes iterator for AAG.
  //! \param[in] graph graph to iterate.
  asiAlgo_AAGRandomIterator(const Handle(asiAlgo_AAG)& graph) : asiAlgo_AAGIterator()
  {
    this->Init(graph);
  }

public:

  asiAlgo_EXPORT virtual void
    Init(const Handle(asiAlgo_AAG)& graph);

  asiAlgo_EXPORT virtual bool
    More() const;

  asiAlgo_EXPORT virtual void
    Next();

  asiAlgo_EXPORT virtual bool
    GetNeighbors(TColStd_PackedMapOfInteger& neighbors) const;

  asiAlgo_EXPORT virtual int
    GetFaceId() const;

protected:

  asiAlgo_AAG::t_adjacency::Iterator m_it; //!< Data map iterator.

};

//-----------------------------------------------------------------------------

//! Random-order AAG iterator limited by a set of node indices.
class asiAlgo_AAGSetIterator : public asiAlgo_AAGIterator
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_AAGSetIterator, asiAlgo_AAGIterator)

public:

  //! Creates and initializes iterator for AAG.
  //! \param[in] graph graph to iterate.
  //! \param[in] set   set of node indices which are the only indices allowed
  //!                  for iteration.
  asiAlgo_AAGSetIterator(const Handle(asiAlgo_AAG)&        graph,
                         const TColStd_PackedMapOfInteger& set) : asiAlgo_AAGIterator()
  {
    this->Init(graph, set);
  }

public:

  asiAlgo_EXPORT virtual void 
    Init(const Handle(asiAlgo_AAG)&        graph,
         const TColStd_PackedMapOfInteger& set);

  asiAlgo_EXPORT virtual bool
    More() const;

  asiAlgo_EXPORT virtual void
    Next();

  asiAlgo_EXPORT virtual bool
    GetNeighbors(TColStd_PackedMapOfInteger& neighbors) const;

  asiAlgo_EXPORT virtual int
    GetFaceId() const;

protected:

  TColStd_MapIteratorOfPackedMapOfInteger m_it; //!< Data map iterator.

};

//-----------------------------------------------------------------------------

//! Collection of rules for parameterized neighborhood iterator.
//! \sa asiAlgo_AAGNeighborsIterator
namespace asiAlgo_AAGIterationRule
{
  //! This rule is fully permissive. Using this rule you can iterate the
  //! entire connected component of AAG graph.
  class AllowAny : public Standard_Transient
  {
  public:

    // OCCT RTTI
    DEFINE_STANDARD_RTTI_INLINE(AllowAny, Standard_Transient)

  public:

    AllowAny() {} //!< Default ctor.

  public:

    bool IsBlocking(const int) { return false; }
  };
};

//! AAG iterator visiting neighbor faces for the given seed in depth-first order.
//! Seed faces are also traversed by this iterator. Use this iterator to visit
//! all nodes constituting a connected component in AAG.
template <typename t_blockRule>
class asiAlgo_AAGNeighborsIterator : public asiAlgo_AAGIterator
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_AAGNeighborsIterator, asiAlgo_AAGIterator)

public:

  //! Creates and initializes iterator for AAG.
  //! \param[in] graph graph to iterate.
  //! \param[in] seed  seed face.
  //! \param[in] rule  rule to block traversal of neighbor nodes.
  asiAlgo_AAGNeighborsIterator(const Handle(asiAlgo_AAG)& graph,
                               const int                  seed,
                               const Handle(t_blockRule)& rule) : asiAlgo_AAGIterator()
  {
    this->SetBlockRule(rule);
    this->Init(graph, seed);
  }

public:

  //! Sets block rule to prevent further iterations on the current node.
  //! \param[in] rule blocking rule to set.
  void SetBlockRule(const Handle(t_blockRule)& rule)
  {
    m_blockRule = rule;
  }

public:

  //! Initializes iterator.
  //! \param[in] graph graph to iterate.
  //! \param[in] seed  1-based ID of the seed face to start iteration from.
  void Init(const Handle(asiAlgo_AAG)& graph,
            const int                  seed)
  {
    asiAlgo_AAGIterator::SetGraph(graph);
    //
    m_iSeed = seed;
    m_visited.Clear();

    // Check whether the seed face should be added for traversal.
    if ( !m_blockRule->IsBlocking(m_iSeed) )
      m_fringe.push(m_iSeed);
    //
    m_visited.Add(m_iSeed);
  }

  //! \return true if there are still some faces to iterate.
  bool More() const
  {
    return !m_fringe.empty();
  }

  //! Moves iterator to another (adjacent) face.
  void Next()
  {
    // Let's throw an exception if there is nothing else to iterate.
    if ( !this->More() )
      Standard_ProgramError::Raise("No next item");

    // Take current.
    const int iCurrent = this->GetFaceId();
    m_fringe.pop(); // Top item is done.

    // Put all nodes pending for iteration to the fringe.
    const TColStd_PackedMapOfInteger& neighbors = m_graph->GetNeighbors(iCurrent);
    //
    for ( TColStd_MapIteratorOfPackedMapOfInteger nit(neighbors); nit.More(); nit.Next() )
    {
      const int iNext = nit.Key();
      //
      if ( m_visited.Contains(iNext) )
        continue;

      // Set as visited.
      m_visited.Add(iNext);

      // Check whether the current neighbor should be added for traversal.
      if ( m_blockRule->IsBlocking(iNext) )
        continue;

      // Set node to iterate.
      m_fringe.push(iNext);
    }
  }

  //! Returns the neighbors of the current face.
  //! \param[out] neighbors neighbors.
  //! \return false is no neighbors available.
  bool GetNeighbors(TColStd_PackedMapOfInteger& neighbors) const
  {
    const int face_id = this->GetFaceId();
    if ( !m_graph->HasNeighbors(face_id) )
      return false;

    neighbors = m_graph->GetNeighbors(face_id);
    return true;
  }

  //! \return ID of the current face.
  int GetFaceId() const
  {
    return m_fringe.top();
  }

protected:

  int                        m_iSeed;     //!< Seed node.
  TColStd_PackedMapOfInteger m_visited;   //!< Visited nodes.
  std::stack<int>            m_fringe;    //!< Where to return.
  Handle(t_blockRule)        m_blockRule; //!< Rule to block further iterations.

};

#endif
