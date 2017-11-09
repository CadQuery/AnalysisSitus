//-----------------------------------------------------------------------------
// Created on: 28 November 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef asiAlgo_BaseCloud_h
#define asiAlgo_BaseCloud_h

// asiAlgo includes
#include <asiAlgo_CloudRegion.h>

// OCCT includes
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

  asiAlgo_EXPORT void
    AddElement(const TCoordType x, const TCoordType y, const TCoordType z);

  asiAlgo_EXPORT void
    SetElement(const int elemIndex, const TCoordType x, const TCoordType y, const TCoordType z);

  asiAlgo_EXPORT void
    GetElement(const int elemIndex, TCoordType& x, TCoordType& y, TCoordType& z) const;

  asiAlgo_EXPORT const std::vector<TCoordType>&
    GetCoords() const;

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
