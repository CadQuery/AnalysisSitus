//-----------------------------------------------------------------------------
// Created on: 22 February 2019
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
#include <asiUI_UnitManager.h>

// asiAlgo includes
#include <asiAlgo_Dictionary.h>

// OCC includes
#include <TColStd_SequenceOfAsciiString.hxx>

//! Default constructor.
asiUI_UnitManager::asiUI_UnitManager() 
: QObject()
{
}

//! Set unit for a dimension for performing data model
//! values conversion into customizable system of units.
//! \param theDimension [in] unit dimension name.
//! \param theUnit [in] unit name.
//! \return true if units succesfully set, otherwise - false.
bool asiUI_UnitManager::SetUnit(const TCollection_AsciiString& theDimension,
                                             const TCollection_AsciiString& theUnit)
{
  if ( !m_UnitSystem.SetUnit(theDimension, theUnit) )
    return false;

  UpdateDictionary();

  return true;
}

//! Set unit for a list of dimensions. Unit names and dimension names should
//! correspond to each other by the indexes.
//! \param theDimensions [in] list of dimension names.
//! \param theUnits [in] list of unit names corresponding to a dimension name list.
bool asiUI_UnitManager::SetUnits(const TColStd_SequenceOfAsciiString& theDimensions,
                                              const TColStd_SequenceOfAsciiString& theUnits)
{
  ASSERT_RAISE(theDimensions.Length() == theUnits.Length(),
               "Dimension name and unit name lists have different length");

  for ( int anIdx = 1; anIdx <= theDimensions.Length(); anIdx++ )
  {
    TCollection_AsciiString aDim  = theDimensions(anIdx);
    TCollection_AsciiString aUnit = theUnits(anIdx);

    if ( !m_UnitSystem.SetUnit(aDim, aUnit) )
    {
      ASSERT_RAISE(m_UnitSystem.SetUnit(aDim, aUnit), "Invalid unit or dimension name");
    }
  }

  UpdateDictionary();

  return true;
}

//! Get selected measurement units for a dimension. The units are used in gui controls
//! for performing conversion of data model values from SI unit system to local
//! unit system defined by user.
//! \param theDimension [out] measurement dimension.
//! \return selected unit name.
TCollection_AsciiString
  asiUI_UnitManager::GetUnit(const TCollection_AsciiString& theDimension) const
{
  return m_UnitSystem.GetUnit(theDimension);
}

//! Get system of measurement units.
//! \return unit system.
asiAlgo_DictionaryUnitSystem asiUI_UnitManager::GetUnitSystem() const
{
  return m_UnitSystem;
}

//! Update dictionary's local unit system used when converting values.
void asiUI_UnitManager::UpdateDictionary()
{
  asiAlgo_DictionaryUnitSystem aPrev =
    asiAlgo_Dictionary::GetLocalUnitSystem();

  asiAlgo_Dictionary::SetLocalUnitSystem(m_UnitSystem);

  if ( m_UnitSystem != aPrev )
  {
    emit UnitSystemChanged();
    emit UnitSystemChanged(aPrev, m_UnitSystem);
  }
}
