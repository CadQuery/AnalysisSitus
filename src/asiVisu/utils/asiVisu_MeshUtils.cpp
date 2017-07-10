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

// Own include
#include <asiVisu_MeshUtils.h>

//! Initializes VTK lookup table charged with default color scheme for meshes.
//! \return VTK lookup table.
vtkSmartPointer<vtkLookupTable> asiVisu_MeshUtils::InitLookupTable()
{
  vtkSmartPointer<vtkLookupTable> aLookup = vtkSmartPointer<vtkLookupTable>::New();

  double aRange[2] = {0, MeshItem_FreeNode};
  aLookup->SetRange(aRange);
  aLookup->SetNumberOfColors(MeshItem_Last);

  aLookup->SetTableValue(MeshItem_Triangle,   1.0, 1.0, 1.0);
  aLookup->SetTableValue(MeshItem_Quadrangle, 0.9, 0.9, 0.9);
  aLookup->SetTableValue(MeshItem_FreeNode,   1.0, 0.0, 0.0);
  aLookup->SetTableValue(MeshItem_FreeEdge,   1.0, 0.0, 0.0);

  return aLookup;
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
