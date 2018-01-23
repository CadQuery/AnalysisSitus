//-----------------------------------------------------------------------------
// Created on: 11 July 2017
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
  MeshPrimitive_CellTriangle    = 9,  //!< Cell type for triangle facet.
  MeshPrimitive_CellQuad        = 10, //!< Cell type for quad facet.
  MeshPrimitive_CellTetra       = 11, //!< Cell type for tetrahedron.
  MeshPrimitive_CellHexa        = 12, //!< Cell type for hexahedron.
  //
  MeshPrimitive_Detected        = 13, //!< Detected boundary element.
  MeshPrimitive_Selected        = 14, //!< Selected boundary element.
  //
  MeshPrimitive_LAST
};

#endif
