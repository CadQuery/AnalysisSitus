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

#ifndef asiVisu_MeshPrimitive_h
#define asiVisu_MeshPrimitive_h

// asiVisu includes
#include <asiVisu.h>

//! Possible cell types for mesh elements.
enum asiVisu_MeshPrimitive
{
  MeshPrimitive_Undefined       = 0,  //!< Undefined cell type.
  //
  MeshPrimitive_FreeNode        = 1,  //!< Cell type for free node.
  MeshPrimitive_BorderNode      = 2,  //!< Cell type for border node.
  MeshPrimitive_SharedNode      = 3,  //!< Cell type for shared node.
  //
  MeshPrimitive_FreeLink        = 4,  //!< Cell type for free link.
  MeshPrimitive_DanglingLink    = 5,  //!< Cell type for dangling link.
  MeshPrimitive_BorderLink      = 6,  //!< Cell type for border link.
  MeshPrimitive_ManifoldLink    = 7,  //!< Cell type for manifold link.
  MeshPrimitive_NonManifoldLink = 8,  //!< Cell type for non-manifold link.
  //
  MeshPrimitive_FacetTriangle   = 9,  //!< Cell type for triangle facet.
  MeshPrimitive_FacetQuad       = 10, //!< Cell type for quad facet.
  //
  MeshPrimitive_Detected        = 11, //!< Detected boundary element.
  MeshPrimitive_Selected        = 12, //!< Selected boundary element.
  //
  MeshPrimitive_LAST
};

#endif
