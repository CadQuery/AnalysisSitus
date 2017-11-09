//-----------------------------------------------------------------------------
// Created on: 11 July 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
