//-----------------------------------------------------------------------------
// Created on: 10 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
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
