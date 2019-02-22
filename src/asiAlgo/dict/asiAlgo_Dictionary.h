//-----------------------------------------------------------------------------
// Created on: 21 February 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Anton Poletaev, Sergey Slyadnev
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

#ifndef asiAlgo_Dictionary_HeaderFile
#define asiAlgo_Dictionary_HeaderFile

// asiAlgo includes
#include <asiAlgo_DictionaryItem.h>
#include <asiAlgo_DictionaryDimension.h>
#include <asiAlgo_DictionaryUnitSystem.h>

// Active Data includes
#include <ActAPI_IProgressNotifier.h>

// SUIT includes
#include <DDS_Dictionary.h>
#include <DDS_DicGroup.h>
#include <DDS_DicItem.h>

//-----------------------------------------------------------------------------

class asiAlgo_DictionaryImpl;

//! Data dictionary is used to provide data definitions specified in an XML
//! file. This file describes different physical quantities and their
//! compositions which are bound to the Data Model parameters using the so
//! called "semantic IDs". Data dictionary is essentially the way how to
//! attach a physical semantics to the numerical values used in the software.
//!
//! This class is an interface for working with data dictionary.
class asiAlgo_Dictionary
{
public:

  asiAlgo_EXPORT static bool
    Load(const TCollection_AsciiString& theFile);

// dictionary item
public:

  asiAlgo_EXPORT static Handle(asiAlgo_DictionaryItem)
    GetDictionaryItem(const TCollection_AsciiString& theId);

  asiAlgo_EXPORT static void 
    GetItemIds(TColStd_SequenceOfAsciiString& theSeqOfIds);

// dimensions definition
public:

  asiAlgo_EXPORT static void
    GetDimensions(asiAlgo_DictionaryDimensions& dimensions);

  asiAlgo_EXPORT static Handle(asiAlgo_DictionaryDimension)
    GetDimension(const TCollection_AsciiString& theName);

  asiAlgo_EXPORT static asiAlgo_DictionaryUnitSystem
    GetLocalUnitSystem();

  asiAlgo_EXPORT static void
    SetLocalUnitSystem(const asiAlgo_DictionaryUnitSystem& theSystem);

// progress notifier for sending log messages
public:

  asiAlgo_EXPORT static void 
    SetNotifier(const Handle(ActAPI_IProgressNotifier)& theNotifier);

  //! Get notifier.
  //! \return notifier instance.
  asiAlgo_EXPORT static Handle(ActAPI_IProgressNotifier) 
    GetNotifier();

// log flags
public:

  asiAlgo_EXPORT static void
    SetConversionErrorLog(const bool theIsLogged);

  asiAlgo_EXPORT static bool
    IsConversionErrorLog();

private:

  static Handle(asiAlgo_DictionaryImpl)
    getImpl();
};

// ----------------------------------------------------------------------------
//                  SUIT Level Implementation
// ----------------------------------------------------------------------------

//! SUIT data dictionary. Implements updated dictionary format.
//! This class is wrapped by asiAlgo_Dictionary class to provide a
//! convenient interface to all of methods that comply with its
//! implementation.
//!
//! \sa asiAlgo_Dictionary.
class asiAlgo_DictionaryImpl : public DDS_Dictionary
{
public:

  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_DictionaryImpl, DDS_Dictionary)

protected:

  asiAlgo_EXPORT
    asiAlgo_DictionaryImpl();

protected:

  asiAlgo_EXPORT virtual void
    FillDataMap(const LDOM_Element& theDicElt);

// dimensions definition
public:
  
  asiAlgo_EXPORT void
    GetDimensions(asiAlgo_DictionaryDimensions& dimensions) const;

  asiAlgo_EXPORT void
    GetDimensions(const TCollection_AsciiString& theComponent,
                  asiAlgo_DictionaryDimensions&  dimensions) const;

  asiAlgo_EXPORT Handle(asiAlgo_DictionaryDimension)
    GetDimension(const TCollection_AsciiString& theName) const;

  asiAlgo_EXPORT Handle(asiAlgo_DictionaryDimension)
    GetDimension(const TCollection_AsciiString& theName,
                 const TCollection_AsciiString& theComponent) const;

public:

  //! Get local unit system.
  //! \return a copy of local unit system instance.
  const asiAlgo_DictionaryUnitSystem& GetLocalUnitSystem() const
  {
    return m_UnitSystem;
  }

  //! Set local unit system.
  //! \param theSystem [in] the new local unit system.
  void SetLocalUnitSystem(const asiAlgo_DictionaryUnitSystem& theSystem)
  {
    m_UnitSystem = theSystem;
  }

// progress notifier for sending log messages
public:

  asiAlgo_EXPORT void
    SetNotifier(const Handle(ActAPI_IProgressNotifier)& theNotifier);

  asiAlgo_EXPORT Handle(ActAPI_IProgressNotifier)
    GetNotifier() const;

public:

  asiAlgo_EXPORT static void
    Init();

  asiAlgo_EXPORT static TCollection_AsciiString
    GetAsiUnitSystem();

  asiAlgo_EXPORT static TCollection_AsciiString
    GetAsiUnitSystemLabel();

// logger flags
public:

  asiAlgo_EXPORT void
    SetConversionErrorLog(const bool theIsLogged);

  asiAlgo_EXPORT bool
    IsConversionErrorLog() const;

private:

  bool m_bLogUnitErr;

  asiAlgo_DictionaryUnitSystem m_UnitSystem;

  Handle(ActAPI_IProgressNotifier) m_Notifier;
};

#endif
