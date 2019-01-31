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

// Own include
#include <asiVisu_MeshUtils.h>

// asiVisu includes
#include <asiVisu_MeshPrimitive.h>

//! Initializes VTK lookup table charged with default color scheme for meshes.
//! \return VTK lookup table.
vtkSmartPointer<vtkLookupTable> asiVisu_MeshUtils::InitLookupTable()
{
  vtkSmartPointer<vtkLookupTable>
    colorTable = vtkSmartPointer<vtkLookupTable>::New();

  // Set colors table for 3D shapes
  double range[2];
  range[0] = MeshPrimitive_Undefined;
  range[1] = MeshPrimitive_LAST - 1;
  //
  colorTable->Allocate(MeshPrimitive_LAST);
  colorTable->SetNumberOfTableValues(MeshPrimitive_LAST);
  colorTable->SetTableRange(range);
  colorTable->SetValueRange(0, 1);
  //
  colorTable->SetTableValue(MeshPrimitive_Undefined,       0.0,  0.0, 0.0);
  //
  colorTable->SetTableValue(MeshPrimitive_FreeNode,        0.75, 0.0, 0.0);
  colorTable->SetTableValue(MeshPrimitive_BorderNode,      1.0,  0.0, 0.0);
  colorTable->SetTableValue(MeshPrimitive_SharedNode,      0.0,  0.0, 0.0);
  //
  colorTable->SetTableValue(MeshPrimitive_FreeLink,        0.75, 0.0, 0.0);
  colorTable->SetTableValue(MeshPrimitive_DanglingLink,    1.0,  0.0, 0.0);
  colorTable->SetTableValue(MeshPrimitive_BorderLink,      1.0,  0.0, 0.0);
  colorTable->SetTableValue(MeshPrimitive_ManifoldLink,    0.1,  0.1, 0.1);
  colorTable->SetTableValue(MeshPrimitive_NonManifoldLink, 1.0,  1.0, 0.0);
  //
  colorTable->SetTableValue(MeshPrimitive_CellTriangle,   0.9,  0.9, 0.9);
  colorTable->SetTableValue(MeshPrimitive_CellQuad,       0.7,  0.7, 0.7);
  colorTable->SetTableValue(MeshPrimitive_CellTetra,      0.9,  0.9, 0.9);
  colorTable->SetTableValue(MeshPrimitive_CellHexa,       0.7,  0.7, 0.7);
  //
  colorTable->SetTableValue(MeshPrimitive_Detected,        0.0, 1.0, 1.0);
  colorTable->SetTableValue(MeshPrimitive_Selected,        1.0, 1.0, 0.0);

  return colorTable;
}

//! Initializes the passed VTK mapper with the given Lookup Table.
//! \param theMapper         [in/out] mapper to initialize.
//! \param theLookup         [in]     Lookup Table to initialize the mapper with.
//! \param theScalarsArrName [in]     name of the array storing the scalars
//!                                   for colorization.
void asiVisu_MeshUtils::InitMapper(vtkMapper*      theMapper,
                                   vtkLookupTable* theLookup,
                                   const char*     theScalarsArrName)
{
  theMapper->ScalarVisibilityOn();
  theMapper->SetScalarModeToUseCellFieldData();
  theMapper->SelectColorArray(theScalarsArrName);
  theMapper->SetColorModeToMapScalars();
  theMapper->SetScalarRange( theLookup->GetRange() );
  theMapper->SetLookupTable(theLookup);
  theMapper->Update();
}

//! Initializes the passed VTK mapper with the default Lookup Table.
//! \param theMapper         [in/out] mapper to initialize.
//! \param theScalarsArrName [in]     name of the array storing the scalars
//!                                   for colorization.
void asiVisu_MeshUtils::InitMapper(vtkMapper*  theMapper,
                                   const char* theScalarsArrName)
{
  vtkSmartPointer<vtkLookupTable> aLookup = InitLookupTable();
  InitMapper(theMapper, aLookup, theScalarsArrName);
}

//! Returns default shrink factor for meshes.
//! \return shrink factor.
double asiVisu_MeshUtils::DefaultShrinkFactor()
{
  return 0.8;
}

//! Returns default point size used for representation of mesh nodes.
//! \return point size in pixels.
double asiVisu_MeshUtils::DefaultPointSize()
{
  return 5;
}

//! Returns default color for mesh contour.
//! \param fR [out] red component [0;1].
//! \param fG [out] green component [0;1].
//! \param fB [out] blue component [0;1].
void asiVisu_MeshUtils::DefaultContourColor(double& fR,
                                            double& fG,
                                            double& fB)
{
  fR = fG = fB = 0.0; // Everything is BLACK
}

//! Returns default opacity for mesh contour.
//! \return opacity factor.
double asiVisu_MeshUtils::DefaultContourOpacity()
{
  return 0.2;
}
