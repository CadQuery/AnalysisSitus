//-----------------------------------------------------------------------------
// Created on: 26 November 2015
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

#ifndef asiVisu_DisplayMode_h
#define asiVisu_DisplayMode_h

// asiVisu includes
#include <asiVisu_ShapePrimitive.h>

// OCCT includes
#include <TColStd_PackedMapOfInteger.hxx>

//-----------------------------------------------------------------------------

//! Enumerates display modes.
enum asiVisu_DisplayMode
{
  DisplayMode_Undefined            = 0x0000, //!< Undefined display mode.
  DisplayMode_Shaded               = 0x0001, //!< Shaded.
  DisplayMode_Wireframe            = 0x0002, //!< Wireframe.
  DisplayMode_Vertices             = 0x0004, //!< Vertices
  DisplayMode_WireframeAndVertices = 0x0008  //!< Wireframe with vertices.
};

//-----------------------------------------------------------------------------

//! Provides display modes for a display mode filter.
class asiVisu_DisplayModeProvider
{
public:

  //! \return collection of shape primitives employed in SHADED mode.
  static TColStd_PackedMapOfInteger SHADED()
  {
    TColStd_PackedMapOfInteger mode;
    //
    mode.Add(ShapePrimitive_FreeVertex);
    mode.Add(ShapePrimitive_DanglingEdge);
    mode.Add(ShapePrimitive_NonManifoldEdge);
    mode.Add(ShapePrimitive_Facet);
    //
    return mode;
  }

  //! \return collection of shape primitives employed in WIREFRAME mode.
  static TColStd_PackedMapOfInteger WIREFRAME()
  {
    TColStd_PackedMapOfInteger mode;
    //
    mode.Add(ShapePrimitive_FreeVertex);
    mode.Add(ShapePrimitive_DanglingEdge);
    mode.Add(ShapePrimitive_FreeEdge);
    mode.Add(ShapePrimitive_ManifoldEdge);
    mode.Add(ShapePrimitive_NonManifoldEdge);
    //
    return mode;
  }

  //! \return collection of shape primitives employed in WIREFRAME and VERTICES mode.
  static TColStd_PackedMapOfInteger WIREFRAME_AND_VERTICES()
  {
    TColStd_PackedMapOfInteger mode = WIREFRAME();
    //
    mode.Add(ShapePrimitive_SharedVertex);
    //
    return mode;
  }

  //! \return collection of shape primitives employed in VERTICES mode.
  static TColStd_PackedMapOfInteger VERTICES()
  {
    TColStd_PackedMapOfInteger mode;
    //
    mode.Add(ShapePrimitive_FreeVertex);
    mode.Add(ShapePrimitive_SharedVertex);
    //
    return mode;
  }

  //! Returns shape primitives employed in the given display mode.
  //! \param mode [in] display mode of interest.
  //! \return collection of primitive types.
  static TColStd_PackedMapOfInteger GetPrimitivesForMode(const asiVisu_DisplayMode mode)
  {
    switch ( mode )
    {
      case DisplayMode_Shaded:               return SHADED();
      case DisplayMode_Wireframe:            return WIREFRAME();
      case DisplayMode_Vertices:             return VERTICES();
      case DisplayMode_WireframeAndVertices: return WIREFRAME_AND_VERTICES();
      //
      default: break;
    }
    return TColStd_PackedMapOfInteger();
  }
};

#endif
