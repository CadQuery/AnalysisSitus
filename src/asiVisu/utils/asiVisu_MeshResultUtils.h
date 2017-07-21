//-----------------------------------------------------------------------------
// Created on: 20 November 2015
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

#ifndef asiVisu_MeshResultUtils_h
#define asiVisu_MeshResultUtils_h

// Visualization includes
#include <asiVisu.h>

// VTK includes
#include <vtkActor.h>
#include <vtkLookupTable.h>
#include <vtkMapper.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkScalarBarActor.h>
#include <vtkScalarBarWidget.h>
#include <vtkSmartPointer.h>
#include <vtkTextActor.h>
#include <vtkTransform.h>

// OCCT includes
#include <Standard_Type.hxx>

//! Auxiliary functions facilitating representation of calculation
//! results on meshes.
namespace asiVisu_MeshResultUtils
{
  asiVisu_EXPORT vtkSmartPointer<vtkLookupTable>
    InitLookupTable(const double theRangeMin,
                    const double theRangeMax);

  asiVisu_EXPORT void
    InitCellScalarMapper(vtkMapper* theMapper,
                         vtkLookupTable* theLookup,
                         const char* theScalarsArrName,
                         const bool doInterpolation = true);

  asiVisu_EXPORT void
    InitCellScalarMapper(vtkMapper* theMapper,
                         const char* theScalarsArrName,
                         const double theRangeMin,
                         const double theRangeMax,
                         const bool doInterpolation = true);

  asiVisu_EXPORT void
    InitPointScalarMapper(vtkMapper* theMapper,
                          vtkLookupTable* theLookup,
                          const char* theScalarsArrName,
                          const bool doInterpolation = true);

  asiVisu_EXPORT void
    InitPointScalarMapper(vtkMapper* theMapper,
                          const char* theScalarsArrName,
                          const double theRangeMin,
                          const double theRangeMax,
                          const bool doInterpolation = true);

  asiVisu_EXPORT void
    InitScalarBarWidget(vtkScalarBarWidget* theScalarBarWidget);

  asiVisu_EXPORT vtkSmartPointer<vtkPolyDataAlgorithm>
    GetVectorGlyph();

  asiVisu_EXPORT vtkSmartPointer<vtkTransform>
    GetVectorGlyphTransform();

  asiVisu_EXPORT void
    ApplySoftLightingRules(vtkActor* actor);
};

#endif
