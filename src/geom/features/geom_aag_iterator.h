//-----------------------------------------------------------------------------
// Created on: 10 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_aag_iterator_h
#define geom_aag_iterator_h

// A-Situs includes
#include <analysis_situs.h>

// Geometry includes
#include <geom_aag.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_aag_iterator, Standard_Transient)

//! AAG iterator.
class geom_aag_iterator : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_aag_iterator, Standard_Transient);

public:

  //! Default constructor.
  geom_aag_iterator() : Standard_Transient()
  {}

  //! Creates and initializes iterator for AAG.
  //! \param graph [in] graph to iterate.
  geom_aag_iterator(const Handle(geom_aag)& graph) : Standard_Transient()
  {
    this->Init(graph);
  }

public:

  ASitus_EXPORT inline const Handle(geom_aag)&
    GetGraph() const;

  ASitus_EXPORT virtual void
    Init(const Handle(geom_aag)& graph);

public:

  virtual bool                              More         () const = 0;
  virtual void                              Next         () = 0;
  virtual const TColStd_PackedMapOfInteger& GetNeighbors () const = 0;
  virtual int                               GetFaceId    () const = 0;

protected:

  Handle(geom_aag) m_graph; //!< AAG being iterated.

};

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_aag_random_iterator, geom_aag_iterator)

//! Random-order AAG iterator.
class geom_aag_random_iterator : public geom_aag_iterator
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_aag_random_iterator, geom_aag_iterator);

public:

  //! Creates and initializes iterator for AAG.
  //! \param graph [in] graph to iterate.
  geom_aag_random_iterator(const Handle(geom_aag)& graph) : geom_aag_iterator()
  {
    this->Init(graph);
  }

public:

  ASitus_EXPORT virtual void                              Init         (const Handle(geom_aag)& graph);
  ASitus_EXPORT virtual bool                              More         () const;
  ASitus_EXPORT virtual void                              Next         ();
  ASitus_EXPORT virtual const TColStd_PackedMapOfInteger& GetNeighbors () const;
  ASitus_EXPORT virtual int                               GetFaceId    () const;

protected:

  geom_aag::t_adjacency::Iterator m_it; //!< Data map iterator.

};

#endif
