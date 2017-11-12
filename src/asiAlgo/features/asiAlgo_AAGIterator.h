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

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiAlgo_AAGIterator, Standard_Transient)

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

DEFINE_STANDARD_HANDLE(asiAlgo_AAGRandomIterator, asiAlgo_AAGIterator)

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

DEFINE_STANDARD_HANDLE(asiAlgo_AAGSetIterator, asiAlgo_AAGIterator)

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

#endif
