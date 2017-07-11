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
  MeshPrimitive_SharedNode      = 2,  //!< Cell type for shared node.
  MeshPrimitive_DanglingLink    = 3,  //!< Cell type for dangling link.
  MeshPrimitive_FreeLink        = 4,  //!< Cell type for free link.
  MeshPrimitive_ManifoldLink    = 5,  //!< Cell type for manifold link.
  MeshPrimitive_NonManifoldLink = 6,  //!< Cell type for non-manifold link.
  MeshPrimitive_FacetTriangle   = 7,  //!< Cell type for triangle facet.
  MeshPrimitive_FacetQuad       = 8,  //!< Cell type for quad facet.
  //
  MeshPrimitive_Detected        = 9,  //!< Detected boundary element.
  MeshPrimitive_Selected        = 10, //!< Selected boundary element.
  //
  MeshPrimitive_LAST
};

#endif
