//-----------------------------------------------------------------------------
// Created on: 28 November 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
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

#ifndef asiAlgo_BaseCloud_h
#define asiAlgo_BaseCloud_h

// asiAlgo includes
#include <asiAlgo_CloudRegion.h>

// OCCT includes
#include <gp_XYZ.hxx>
#include <Standard_Type.hxx>
#include <TColStd_HArray1OfReal.hxx>

// STL includes
#include <vector>

//-----------------------------------------------------------------------------

//! Simple cloud of elements (points or vectors).
template <typename TCoordType>
class asiAlgo_BaseCloud : public Standard_Transient
{
public:

  asiAlgo_EXPORT
    asiAlgo_BaseCloud();

  asiAlgo_EXPORT
    asiAlgo_BaseCloud(const std::vector<TCoordType>& coords);

public:

  asiAlgo_EXPORT void
    CopyTo(asiAlgo_BaseCloud<TCoordType>& copy) const;

  asiAlgo_EXPORT void
    Reserve(const int nElems);

  asiAlgo_EXPORT int
    GetNumberOfElements() const;

  asiAlgo_EXPORT bool
    IsEmpty() const;

  asiAlgo_EXPORT void
    AddElement(const TCoordType x, const TCoordType y, const TCoordType z);

  asiAlgo_EXPORT void
    AddElement(const gp_XYZ& xyz);

  asiAlgo_EXPORT void
    AddElement(const gp_Pnt& xyz);

  asiAlgo_EXPORT void
    SetElement(const int elemIndex, const TCoordType x, const TCoordType y, const TCoordType z);

  asiAlgo_EXPORT void
    GetElement(const int elemIndex, TCoordType& x, TCoordType& y, TCoordType& z) const;

  asiAlgo_EXPORT gp_XYZ
    GetElement(const int elemIndex) const;

  asiAlgo_EXPORT const std::vector<TCoordType>&
    GetCoords() const;

  asiAlgo_EXPORT Handle(TColStd_HArray1OfReal)
    GetCoordsArray() const;

  asiAlgo_EXPORT std::vector<TCoordType>&
    ChangeCoords();

  asiAlgo_EXPORT void
    ComputeBoundingBox(TCoordType& xMin, TCoordType& xMax,
                       TCoordType& yMin, TCoordType& yMax,
                       TCoordType& zMin, TCoordType& zMax) const;

  asiAlgo_EXPORT Handle(asiAlgo_BaseCloud<TCoordType>)
    ExtractRegion(const asiAlgo_CloudRegion& region) const;

  asiAlgo_EXPORT void
    Merge(const Handle(asiAlgo_BaseCloud<TCoordType>)& cloud);

  asiAlgo_EXPORT void
    Clear();

  asiAlgo_EXPORT bool
    ComputeInertiaAxes(gp_Ax3& axes) const;

public:

  //! Reads base cloud recorded in the input file with common XYZ format. That
  //! is, the file contains just coordinate triples without any additional
  //! structuring information.
  //! \param filename [in] file to read.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Load(const char* filename);

  //! Writes base cloud to file with given filename.
  //! \param filename [in] file to write into.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    SaveAs(const char* filename) const;

protected:

  std::vector<TCoordType> m_coords; //!< Coordinates of cloud elements.

};

#endif
