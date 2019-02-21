//-----------------------------------------------------------------------------
// Created on: 20 November 2015
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
