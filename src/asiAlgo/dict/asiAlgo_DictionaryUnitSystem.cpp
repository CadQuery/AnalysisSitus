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

// Own include
#include <asiAlgo_DictionaryUnitSystem.h>

// asiAlgo includes
#include <asiAlgo_DictionaryDimension.h>
#include <asiAlgo_DictionaryUnit.h>
#include <asiAlgo_Dictionary.h>

// SUIT includes
#include <DDS_Dictionary.h>

//! Construct default unit system.
//! By default, SI units set as current for all dimension.
asiAlgo_DictionaryUnitSystem::asiAlgo_DictionaryUnitSystem()
{}

//! Set current unit by name for a dimension.
//! \param theDimensionName [in] dimension name string.
//! \param theUnitName [in] unit name string.
//! \return false if the dimension name or unit name is invalid.
bool asiAlgo_DictionaryUnitSystem::SetUnit(const TCollection_AsciiString& theDimensionName,
                                           const TCollection_AsciiString& theUnitName)
{
  Handle(asiAlgo_DictionaryDimension) aDim = asiAlgo_Dictionary::GetDimension(theDimensionName);
  if ( aDim.IsNull() )
    return false;

  if ( aDim->GetUnit(theUnitName).IsNull() )
    return false;

  if ( m_UnitMap.IsBound(theDimensionName) )
    m_UnitMap.UnBind(theDimensionName);

  m_UnitMap.Bind(theDimensionName, theUnitName);

  return true;
}

//! Get current unit name for a dimension.
//! \param theDimensionName [in] name of the dimension.
//! \return unit name string for a dimension, or empty string
//!         if dimension name is invalid.
TCollection_AsciiString
  asiAlgo_DictionaryUnitSystem::GetUnit(const TCollection_AsciiString& theDimensionName) const
{
  // if the units has been already set
  if ( m_UnitMap.IsBound(theDimensionName) )
    return m_UnitMap(theDimensionName);

  Handle(asiAlgo_DictionaryDimension) aDim = asiAlgo_Dictionary::GetDimension(theDimensionName);
  if ( aDim.IsNull() )
    return TCollection_AsciiString();

  Handle(asiAlgo_DictionaryUnit) aUnit = aDim->GetSIUnit();
  return !aUnit.IsNull() ? aUnit->GetName() : TCollection_AsciiString();
}

//! Compare unit and dimensions choise in two units systems.
//! \param theOther [in] the another unit system to compare.
bool asiAlgo_DictionaryUnitSystem::operator==(const asiAlgo_DictionaryUnitSystem& theOther) const
{
  DimensionUnitMap::Iterator aMyIt (m_UnitMap);
  DimensionUnitMap::Iterator anOtherIt (theOther.m_UnitMap);

  // cross-comparsion of unit choise
  for ( ; aMyIt.More(); aMyIt.Next() )
  {
    const TCollection_AsciiString& aDimName = aMyIt.Key();
    const TCollection_AsciiString& aUnitName = aMyIt.Value();

    if ( theOther.GetUnit(aDimName) != aUnitName )
      return false;
  }

  for ( ; anOtherIt.More(); anOtherIt.Next() )
  {
    const TCollection_AsciiString& aDimName = anOtherIt.Key();
    const TCollection_AsciiString& aUnitName = anOtherIt.Value();

    if ( this->GetUnit(aDimName) != aUnitName )
      return false;
  }

  return true;
}

//! Compare unit and dimensions choise in two units systems.
//! \param theOther [in] the another unit system to compare.
bool asiAlgo_DictionaryUnitSystem::operator!=(const asiAlgo_DictionaryUnitSystem& theOther) const
{
  return !this->operator== (theOther);
}
