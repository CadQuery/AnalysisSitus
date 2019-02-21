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

#ifndef asiAlgo_DictionaryGroup_HeaderFile
#define asiAlgo_DictionaryGroup_HeaderFile

// asiAlgo includes
#include <asiAlgo_DictionaryDimension.h>

// SUIT includes
#include <DDS_DicGroup.h>

// OCC includes
#include <NCollection_IndexedDataMap.hxx>

class LDOM_Element;

//-----------------------------------------------------------------------------

//! Data dictionary group. The data dictionary group can be a "datum" or
//! a "valueList" used by a datum. A datum is a descriptor of a property
//! manipulated by the user. Basically, such properties are represented by
//! Data Model parameters. In a datum, we define the measurement unit of
//! a property together with it user-friendly description, UI widget,
//! formatting, and validity range.
class asiAlgo_DictionaryGroup : public DDS_DicGroup
{
public:

  // Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_DictionaryGroup, DDS_DicGroup)

public:

  //! Constructor.
  asiAlgo_DictionaryGroup() : DDS_DicGroup( ComponentName() ) {}

// Dictionary specific methods
public:

  //! Fills internal data map of component items.
  //! \param[in] groupElt component group element entry.
  //! \param[in] dicElt   data dictionary element entry.
  asiAlgo_EXPORT virtual void
    FillDataMap(const LDOM_Element& groupElt,
                const LDOM_Element& dicElt);

// Dimension definition
public:

  //! Returns the list of defined dimensions.
  //! \param[out] dimensions list of dimensions defined for the dictionary group.
  asiAlgo_EXPORT void
    GetDimensions(asiAlgo_DictionaryDimensions& dimension) const;

  //! Get dimension definition by name.
  //! \param[in] name name of dimension to be found and returneds.
  //! \return handle to a dimension definition specified by name,
  //!         or null handle if the dimension with the passed name is not found.
  asiAlgo_EXPORT Handle(asiAlgo_DictionaryDimension)
    GetDimension(const TCollection_AsciiString& name) const;

public:

  //! This name is used to name the dictionary group and is used
  //! internally by DDS_Dictionray to avoid components overriding
  //! when loading other dictionary groups.
  static const char* ComponentName()
  {
    return "Data Dictionary";
  }

protected:

  typedef NCollection_IndexedDataMap<TCollection_AsciiString,
                                     Handle(asiAlgo_DictionaryDimension)> DimensionMap;

private:

  //! Map of dimensions.
  DimensionMap m_DimensionMap;

};

#endif
