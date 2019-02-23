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

#ifndef asiAlgo_DictionaryDimension_HeaderFile
#define asiAlgo_DictionaryDimension_HeaderFile

// asiAlgo includes
#include <asiAlgo_DictionaryUnit.h>

// OCCT includes
#include <TCollection_AsciiString.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_Shared.hxx>

class LDOM_Element;

//-----------------------------------------------------------------------------

//! Data dictionary definition of a dimension as described in the XML
//! defintion file. This class provides information on the employed units.
class asiAlgo_DictionaryDimension : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_DictionaryDimension, Standard_Transient)

public:

  //! Ctor.
  //! \param[in] name dimension name.
  asiAlgo_EXPORT
    asiAlgo_DictionaryDimension(const TCollection_AsciiString& name);

public:

  //! Get name of the dimension.
  //! \return name string for the dimension.
  const TCollection_AsciiString& GetName() const
  {
    return m_name;
  }

public:

  //! Returns SI unit of the dimension.
  //! \return SI unit for the dimension or null handle if no SI unit is
  //!         defined in the dictionary.
  asiAlgo_EXPORT Handle(asiAlgo_DictionaryUnit)
    GetSIUnit() const;

  //! Returns unit of the dimension by name.
  //! \param[in] name name of the unit.
  //! \return unit definition or null handle in case if the unit specified
  //!         by the passed name is not found.
  asiAlgo_EXPORT Handle(asiAlgo_DictionaryUnit)
    GetUnit(const TCollection_AsciiString& name);

  //! Returns the list of units defined for the dimension.
  //! \param[out] units vector of units defined for the dimension.
  asiAlgo_EXPORT void
    GetUnits(asiAlgo_DictionaryUnits& units) const;

// Dictionary-specific methods
public:

  //! Fills internal map of dimension item.
  //! \param[in] dimElt XML node for the dimension tag.
  asiAlgo_EXPORT virtual void
    FillDataMap(const LDOM_Element& dimElt);

protected:

  typedef NCollection_IndexedDataMap<TCollection_AsciiString,
                                     Handle(asiAlgo_DictionaryUnit)> UnitMap;

private:

  //! Dimension name.
  TCollection_AsciiString m_name;

  //! Indexed map of units.
  UnitMap m_unitMap;

  //! SI unit index.
  int m_iSI;
};

//! Short-cut for a collection of dimensions.
typedef std::vector<Handle(asiAlgo_DictionaryDimension)> asiAlgo_DictionaryDimensions;

#endif
