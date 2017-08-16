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

#ifndef asiVisu_MeshDisplayMode_h
#define asiVisu_MeshDisplayMode_h

// asiVisu includes
#include <asiVisu_MeshPrimitive.h>

// OCCT includes
#include <TColStd_PackedMapOfInteger.hxx>

//-----------------------------------------------------------------------------

//! Enumerates display modes.
enum asiVisu_MeshDisplayMode
{
  MeshDisplayMode_Undefined  = 0x0000, //!< Undefined display mode.
  MeshDisplayMode_Shaded     = 0x0001, //!< Shaded.
  MeshDisplayMode_Shrunk     = 0x0002, //!< Shrunk.
  MeshDisplayMode_Wireframe  = 0x0004  //!< Wireframe.
};

//-----------------------------------------------------------------------------

//! Provides display modes for a display mode filter.
class asiVisu_MeshDisplayModeProvider
{
public:

  //! \return collection of shape primitives employed in SHADED mode.
  static TColStd_PackedMapOfInteger SHADED()
  {
    TColStd_PackedMapOfInteger mode;

    // Add anomaly node types
    mode.Add(MeshPrimitive_FreeNode);
    mode.Add(MeshPrimitive_BorderNode);

    // Add anomaly link types
    mode.Add(MeshPrimitive_FreeLink);
    mode.Add(MeshPrimitive_DanglingLink);
    mode.Add(MeshPrimitive_BorderLink);
    mode.Add(MeshPrimitive_NonManifoldLink);

    // Add facets
    mode.Add(MeshPrimitive_FacetTriangle);
    mode.Add(MeshPrimitive_FacetQuad);

    return mode;
  }

  //! \return collection of shape primitives employed in SHRUNK mode.
  static TColStd_PackedMapOfInteger SHRUNK()
  {
    TColStd_PackedMapOfInteger mode = WIREFRAME();

    // TODO: NYI
    mode.Add(MeshPrimitive_SharedNode);

    return mode;
  }

  //! \return collection of shape primitives employed in WIREFRAME mode.
  static TColStd_PackedMapOfInteger WIREFRAME()
  {
    TColStd_PackedMapOfInteger mode;

    // Add anomaly node types
    mode.Add(MeshPrimitive_FreeNode);
    mode.Add(MeshPrimitive_BorderNode);

    // Add all types of links
    mode.Add(MeshPrimitive_FreeLink);
    mode.Add(MeshPrimitive_DanglingLink);
    mode.Add(MeshPrimitive_BorderLink);
    mode.Add(MeshPrimitive_ManifoldLink);
    mode.Add(MeshPrimitive_NonManifoldLink);

    return mode;
  }

  //! Returns shape primitives employed in the given display mode.
  //! \param mode [in] display mode of interest.
  //! \return collection of primitive types.
  static TColStd_PackedMapOfInteger GetPrimitivesForMode(const asiVisu_MeshDisplayMode mode)
  {
    switch ( mode )
    {
      case MeshDisplayMode_Shaded:    return SHADED();
      case MeshDisplayMode_Shrunk:    return SHRUNK();
      case MeshDisplayMode_Wireframe: return WIREFRAME();
      //
      default: break;
    }
    return TColStd_PackedMapOfInteger();
  }
};

#endif
