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

#ifndef asiAlgo_DictionaryUnitSystem_HeaderFile
#define asiAlgo_DictionaryUnitSystem_HeaderFile

// asiAlgo includes
#include <asiAlgo.h>

// OCCT includes
#include <NCollection_DataMap.hxx>
#include <TCollection_AsciiString.hxx>

//-----------------------------------------------------------------------------

//! Unit system is a helper class for defining preferred unit
//! system for dictionary items.
//! The class can be used to define a local unit system for
//! data dictionary, or to specify a desired system of units
//! when converting data with corresponding dictionary item's
//! methods.
class asiAlgo_DictionaryUnitSystem
{
public:

  asiAlgo_EXPORT
    asiAlgo_DictionaryUnitSystem();

public:

  asiAlgo_EXPORT bool
    SetUnit(const TCollection_AsciiString& theDimensionName,
            const TCollection_AsciiString& theUnitName);

  asiAlgo_EXPORT TCollection_AsciiString
    GetUnit(const TCollection_AsciiString& theDimensionName) const;

  asiAlgo_EXPORT bool 
    operator== (const asiAlgo_DictionaryUnitSystem& theOther) const;

  asiAlgo_EXPORT bool
    operator!= (const asiAlgo_DictionaryUnitSystem& theOther) const;

private:

  typedef NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString> DimensionUnitMap;

private:

  DimensionUnitMap m_UnitMap;
};

#endif
