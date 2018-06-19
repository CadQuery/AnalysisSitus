//-----------------------------------------------------------------------------
// Created on: 19 June 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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
//    * Neither the name of Sergey Slyadnev nor the
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

#ifndef asiAlgo_JSON_HeaderFile
#define asiAlgo_JSON_HeaderFile

// asiAlgo includes
#include <asiAlgo.h>

// OCCT includes
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>

// Standard includes
#include <string>
#include <vector>

//! Utility class to process JSON objects.
class asiAlgo_JSON
{
public:

  asiAlgo_EXPORT
    asiAlgo_JSON(const std::string& json);

  asiAlgo_EXPORT
    ~asiAlgo_JSON();

// Serializers:
public:

  asiAlgo_EXPORT static void
    DumpCurve(const Handle(Geom_Curve)& curve,
              Standard_OStream&         out);

  asiAlgo_EXPORT static void
    DumpSurface(const Handle(Geom_Surface)& surface,
                Standard_OStream&           out);

// Extractors:
public:

  asiAlgo_EXPORT bool
    ExtractBCurve(Handle(Geom_BSplineCurve)& curve) const;

  asiAlgo_EXPORT bool
    ExtractBSurface(Handle(Geom_BSplineSurface)& surface) const;

  asiAlgo_EXPORT bool
    ExtractBlockForKey(const std::string& key,
                       std::string&       block) const;

  asiAlgo_EXPORT bool
    ExtractVector1d(const std::string&   keyword,
                    std::vector<double>& vector) const;

  asiAlgo_EXPORT bool
    ExtractVector3d(const std::string&   keyword,
                    std::vector<gp_XYZ>& vector) const;

  asiAlgo_EXPORT bool
    ExtractGrid3d(const std::string&                  keyword,
                  std::vector< std::vector<gp_XYZ> >& vector) const;

// Services:
public:

  asiAlgo_EXPORT bool
    IsCurve() const;

  asiAlgo_EXPORT bool
    IsSurface() const;

public:

  template <typename T>
  bool ExtractNumericBlockForKey(const std::string& key,
                                  T&                 result,
                                  const T            default_value = 0) const
  {
    std::string block;
    if ( !this->ExtractBlockForKey(key, block) )
      return false;

    // Check if the block represents a number.
    if ( !asiAlgo_Utils::Str::IsNumber(block) )
      return false;

    // Extract number.
    result = asiAlgo_Utils::Str::ToNumber<T>(block, default_value);
    return true;
  }

protected:

  std::string m_json; //!< JSON string to process.

};

#endif
