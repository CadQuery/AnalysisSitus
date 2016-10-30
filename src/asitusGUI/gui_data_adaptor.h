//-----------------------------------------------------------------------------
// Created on: 10 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_data_adaptor_h
#define gui_data_adaptor_h

// A-Situs includes
#include <analysis_situs.h>

DEFINE_STANDARD_HANDLE(gui_data_adaptor, Standard_Transient)

//! Base class for Data Adaptors representing scalar or tabulated data in
//! Data-View connectors.
class gui_data_adaptor : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(gui_data_adaptor, Standard_Transient)

public:

  //! Returns unique ID of the data represented by Adaptor. The uniqueness
  //! must be ensured in a scope of a single Node only (e.g. it can be just
  //! a PID for some the involved Parameters).
  //! \return unique ID.
  virtual int ID() const = 0;

public:

  //! Hasher for Data Adaptors.
  struct Hasher
  {
    //! Returns hash-code for the given Data Adaptor.
    //! \param Adt      [in] Data Adaptor to calculate hash code for.
    //! \param nBuckets [in] number of buckets.
    //! \return hash code.
    static int
      HashCode(const Handle(gui_data_adaptor)& Adt,
               const int                       nBuckets = 100)
    {
      return ::HashCode(Adt->ID(), nBuckets);
    }

    //! Checks whether two Data Adaptors represent the same data. It is
    //! enough to compare their IDs.
    //! \param Adt1 [in] first adaptor.
    //! \param Adt2 [in] second adaptor.
    //! \return true in case of equality, false -- otherwise.
    static int
      IsEqual(const Handle(gui_data_adaptor)& Adt1,
              const Handle(gui_data_adaptor)& Adt2)
    {
      return ( Adt1->ID() == Adt2->ID() );
    }
  };

};

#endif
