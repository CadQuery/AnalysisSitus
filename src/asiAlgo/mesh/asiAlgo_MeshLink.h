//-----------------------------------------------------------------------------
// Created on: 11 July 2017
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

#ifndef asiAlgo_MeshLink_h
#define asiAlgo_MeshLink_h

// asiAlgo includes
#include <asiAlgo.h>

// OCCT includes
#include <NCollection_IndexedMap.hxx>

//! Auxiliary unoriented link structure.
struct asiAlgo_MeshLink
{
  int N1; //!< First node.
  int N2; //!< Second node.

  //! ctor default.
  asiAlgo_MeshLink() : N1(0), N2(0) {}

  //! ctor with parameters.
  asiAlgo_MeshLink(const int _N1, const int _N2) : N1(_N1), N2(_N2) {}

  //! \return hash code for the arc.
  static int HashCode(const asiAlgo_MeshLink& arc, const int upper)
  {
    int key = arc.N1 + arc.N2;
    key += (key << 10);
    key ^= (key >> 6);
    key += (key << 3);
    key ^= (key >> 11);
    return (key & 0x7fffffff) % upper;
  }

  //! \return true if two links are equal.
  static int IsEqual(const asiAlgo_MeshLink& arc1,
                     const asiAlgo_MeshLink& arc2)
  {
    return arc1.N1 == arc2.N1 && arc1.N2 == arc2.N2 ||
           arc1.N2 == arc2.N1 && arc1.N1 == arc2.N2;
  }
};

typedef NCollection_IndexedMap<asiAlgo_MeshLink, asiAlgo_MeshLink> asiAlgo_MeshLinkSet;

#endif
