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

#ifndef asiVisu_MeshUtils_h
#define asiVisu_MeshUtils_h

// Visualization includes
#include <asiVisu.h>

// VTK includes
#include <vtkLookupTable.h>
#include <vtkMapper.h>
#include <vtkSmartPointer.h>

// OCCT includes
#include <Standard_Type.hxx>

//! Mesh items.
enum visu_mesh_items
{
  MeshItem_Triangle   = 0,
  MeshItem_Quadrangle = 1,
  MeshItem_FreeNode   = 2,
  MeshItem_Last
};

//! Auxiliary functions supporting mesh presentations in VTK viewer.
namespace asiVisu_MeshUtils
{
  vtkSmartPointer<vtkLookupTable>
    InitLookupTable();

  void
    InitMapper(vtkMapper* theMapper,
               vtkLookupTable* theLookup,
               const char* theScalarsArrName);

  void
    InitMapper(vtkMapper* theMapper,
               const char* theScalarsArrName);

  double
    DefaultShrinkFactor();

  double
    DefaultPointSize();

  void
    DefaultContourColor(double& fR,
                        double& fG,
                        double& fB);

  double
    DefaultContourLineWidth();

  double
    DefaultContourOpacity();
};

#endif
