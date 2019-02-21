//-----------------------------------------------------------------------------
// Created on: 06 April 2017
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

#ifndef asiVisu_ShapeDisplayModeFilter_h
#define asiVisu_ShapeDisplayModeFilter_h

// asiVisu includes
#include <asiVisu_ShapeDisplayMode.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>

//! Filters shape polygonal data extracting only those cells which correspond
//! to the desired type of shape visualization primitive. The cells remaining
//! after such extraction compose a specific display mode. There is a predefined
//! set of display modes, e.g. shaded, wireframe, vertices, etc.
class asiVisu_ShapeDisplayModeFilter : public vtkPolyDataAlgorithm
{
public:

  vtkTypeMacro(asiVisu_ShapeDisplayModeFilter, vtkPolyDataAlgorithm)

  asiVisu_EXPORT static asiVisu_ShapeDisplayModeFilter*
    New();

public:

  //! Sets display mode.
  //! \param mode [in] display mode to set.
  asiVisu_EXPORT void
    SetDisplayMode(const asiVisu_ShapeDisplayMode mode);

  //! \return display mode.
  asiVisu_EXPORT asiVisu_ShapeDisplayMode
    GetDisplayMode() const;

  //! Adds primitive to the current list of shape primitives.
  //! This method is used for customization of predefined display modes.
  //! \param prim [in] primitive type to add.
  asiVisu_EXPORT void
    AddPrimitive(const asiVisu_ShapePrimitive prim);

  //! Removes primitive from the current list of shape primitives.
  //! This method is used for customization of predefined display modes.
  //! \param prim [in] primitive type to remove.
  asiVisu_EXPORT void
    RemovePrimitive(const asiVisu_ShapePrimitive prim);

protected:

  asiVisu_EXPORT virtual int
    RequestData(vtkInformation*        pInfo,
                vtkInformationVector** pInputVector,
                vtkInformationVector*  pOutputVector);

protected:

  asiVisu_EXPORT
    asiVisu_ShapeDisplayModeFilter();

protected:

  asiVisu_ShapeDisplayMode   m_displayMode;        //!< Display mode.
  TColStd_PackedMapOfInteger m_modePrimitiveTypes; //!< Primitive types corresponding to the display mode.

};

#endif

