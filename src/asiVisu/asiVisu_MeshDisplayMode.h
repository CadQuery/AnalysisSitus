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

    // Add 2D and 3D cells
    mode.Add(MeshPrimitive_CellTriangle);
    mode.Add(MeshPrimitive_CellQuad);
    mode.Add(MeshPrimitive_CellTetra);
    mode.Add(MeshPrimitive_CellHexa);

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
