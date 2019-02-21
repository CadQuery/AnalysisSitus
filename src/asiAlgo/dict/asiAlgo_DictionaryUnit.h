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

#ifndef asiAlgo_DictionaryUnit_HeaderFile
#define asiAlgo_DictionaryUnit_HeaderFile

// asiAlgo includes
#include <asiAlgo.h>

// OCCT includes
#include <TCollection_AsciiString.hxx>

// Standard includes
#include <vector>

//! Data dictionary definition of a measurement unit described in an XML file.
//!
//! This class provides information on:
//! - unit name;
//! - conversion coefficients.
//!
//! The unit definition provides conversion coefficients `k` and `b` for
//! "to SI" conversion: `SI_value = k*non_SI_value + b`.
class asiAlgo_DictionaryUnit : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_DictionaryUnit, Standard_Transient)

public:

  //! Ctor.
  //! \param[in] name   name of the unit.
  //! \param[in] scale  conversion scaling factor.
  //! \param[in] offset conversion offset factor.
  asiAlgo_EXPORT
    asiAlgo_DictionaryUnit(const TCollection_AsciiString& name,
                           const double                   scale,
                           const double                   offset);

public:

  //! Get unit name.
  //! \return unit name string.
  const TCollection_AsciiString& GetName() const
  {
    return m_name;
  }

  //! Get scale conversion factor for conversion of 
  //! the units to SI unit system (coefficient "k").
  //! The scale factor determines how much the 
  //! non-SI value should be scaled to convert it to
  //! a corresponding SI value (in pair with offset).
  double GetScaleFactor() const
  {
    return m_fScale;
  }

  //! Get offset for conversion of the
  //! units to SI unit system (coefficient "b").
  //! The offset determines the value to
  //! be added to non-SI value to convert it to
  //! a corresponding SI value (in pair with scaling).
  double GetOffset() const
  {
    return m_fOffset;
  }

private:

  TCollection_AsciiString m_name;    //!< Name of the measurement unit.
  double                  m_fScale;  //!< Conversion scale.
  double                  m_fOffset; //!< Conversion offset.
};

//! Short-cut for a collection of units.
typedef std::vector<Handle(asiAlgo_DictionaryUnit)> asiAlgo_DictionaryUnits;

#endif
