//-----------------------------------------------------------------------------
// Created on: 06 April 2017
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

