//-----------------------------------------------------------------------------
// Created on: 21 March 2017
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

#ifndef asiVisu_ShapePrimitive_h
#define asiVisu_ShapePrimitive_h

// asiVisu includes
#include <asiVisu.h>

//! Possible cell types for shape boundary elements.
enum asiVisu_ShapePrimitive
{
  ShapePrimitive_Undefined       = 0, //!< Undefined cell type.
  //
  ShapePrimitive_FreeVertex      = 1, //!< Cell type for free vertex.
  ShapePrimitive_SharedVertex    = 2, //!< Cell type for shared vertex.
  ShapePrimitive_DanglingEdge    = 3, //!< Cell type for dangling edge.
  ShapePrimitive_FreeEdge        = 4, //!< Cell type for free edge.
  ShapePrimitive_ManifoldEdge    = 5, //!< Cell type for manifold edge.
  ShapePrimitive_NonManifoldEdge = 6, //!< Cell type for non-manifold edge.
  ShapePrimitive_Facet           = 7, //!< Cell type for facet.
  //
  ShapePrimitive_Detected        = 8, //!< Detected boundary element.
  ShapePrimitive_Selected        = 9, //!< Selected boundary element.
  //
  ShapePrimitive_LAST
};

#endif
