//-----------------------------------------------------------------------------
// Created on: 26 November 2015
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

#ifndef asiVisu_ShapeDisplayMode_h
#define asiVisu_ShapeDisplayMode_h

// asiVisu includes
#include <asiVisu_ShapePrimitive.h>

// OCCT includes
#include <TColStd_PackedMapOfInteger.hxx>

//-----------------------------------------------------------------------------

//! Enumerates display modes.
enum asiVisu_ShapeDisplayMode
{
  ShapeDisplayMode_Undefined            = 0x0000, //!< Undefined display mode.
  ShapeDisplayMode_Shaded               = 0x0001, //!< Shaded.
  ShapeDisplayMode_Wireframe            = 0x0002, //!< Wireframe.
  ShapeDisplayMode_Vertices             = 0x0004, //!< Vertices.
  ShapeDisplayMode_WireframeAndVertices = 0x0008, //!< Wireframe with vertices.
  ShapeDisplayMode_ShadedAndWireframe   = 0x0010  //!< Shaded and wireframe.
};

//-----------------------------------------------------------------------------

//! Provides display modes for a display mode filter.
class asiVisu_ShapeDisplayModeProvider
{
public:

  //! \return collection of shape primitives employed in SHADED mode.
  static TColStd_PackedMapOfInteger SHADED()
  {
    TColStd_PackedMapOfInteger mode;

    // Add facets
    mode.Add(ShapePrimitive_Facet);

    // Add isolines for bad faces
    mode.Add(ShapePrimitive_Isoline);

    return mode;
  }

  //! \return collection of shape primitives employed in WIREFRAME mode.
  static TColStd_PackedMapOfInteger WIREFRAME()
  {
    TColStd_PackedMapOfInteger mode;

    // Add anomaly vertex types
    mode.Add(ShapePrimitive_FreeVertex);
    mode.Add(ShapePrimitive_BorderVertex);

    // Add all types of edges
    mode.Add(ShapePrimitive_FreeEdge);
    mode.Add(ShapePrimitive_DanglingEdge);
    mode.Add(ShapePrimitive_BorderEdge);
    mode.Add(ShapePrimitive_ManifoldEdge);
    mode.Add(ShapePrimitive_NonManifoldEdge);

    return mode;
  }

  //! \return collection of shape primitives employed in VERTICES mode.
  static TColStd_PackedMapOfInteger VERTICES()
  {
    TColStd_PackedMapOfInteger mode;
    //
    mode.Add(ShapePrimitive_FreeVertex);
    mode.Add(ShapePrimitive_BorderVertex);
    mode.Add(ShapePrimitive_SharedVertex);
    //
    return mode;
  }

  //! \return collection of shape primitives employed in WIREFRAME and VERTICES mode.
  static TColStd_PackedMapOfInteger WIREFRAME_AND_VERTICES()
  {
    TColStd_PackedMapOfInteger mode = WIREFRAME();
    //
    mode.Unite( VERTICES() );
    //
    return mode;
  }

  //! \return collection of shape primitives employed in SHADED and WIREFRAME mode.
  static TColStd_PackedMapOfInteger SHADED_AND_WIREFRAME()
  {
    TColStd_PackedMapOfInteger mode = SHADED();
    //
    mode.Unite( WIREFRAME() );
    //
    return mode;
  }

  //! Returns shape primitives employed in the given display mode.
  //! \param mode [in] display mode of interest.
  //! \return collection of primitive types.
  static TColStd_PackedMapOfInteger GetPrimitivesForMode(const asiVisu_ShapeDisplayMode mode)
  {
    switch ( mode )
    {
      case ShapeDisplayMode_Shaded:               return SHADED();
      case ShapeDisplayMode_Wireframe:            return WIREFRAME();
      case ShapeDisplayMode_Vertices:             return VERTICES();
      case ShapeDisplayMode_WireframeAndVertices: return WIREFRAME_AND_VERTICES();
      case ShapeDisplayMode_ShadedAndWireframe:   return SHADED_AND_WIREFRAME();
      //
      default: break;
    }
    return TColStd_PackedMapOfInteger();
  }
};

#endif
