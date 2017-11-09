//-----------------------------------------------------------------------------
// Created on: 10 December 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef asiUI_DataAdaptor_h
#define asiUI_DataAdaptor_h

// A-Situs includes
#include <asiUI.h>

// OCCT includes
#include <Standard_Type.hxx>

DEFINE_STANDARD_HANDLE(asiUI_DataAdaptor, Standard_Transient)

//! Base class for Data Adaptors representing scalar or tabulated data in
//! Data-View connectors.
class asiUI_DataAdaptor : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiUI_DataAdaptor, Standard_Transient)

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
      HashCode(const Handle(asiUI_DataAdaptor)& Adt,
               const int                        nBuckets = 100)
    {
      return ::HashCode(Adt->ID(), nBuckets);
    }

    //! Checks whether two Data Adaptors represent the same data. It is
    //! enough to compare their IDs.
    //! \param Adt1 [in] first adaptor.
    //! \param Adt2 [in] second adaptor.
    //! \return true in case of equality, false -- otherwise.
    static int
      IsEqual(const Handle(asiUI_DataAdaptor)& Adt1,
              const Handle(asiUI_DataAdaptor)& Adt2)
    {
      return ( Adt1->ID() == Adt2->ID() );
    }
  };

};

#endif
