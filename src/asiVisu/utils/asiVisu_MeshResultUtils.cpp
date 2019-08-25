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

// Own include
#include <asiVisu_MeshResultUtils.h>

// VTK includes
#include <vtkCoordinate.h>
#include <vtkGlyphSource2D.h>
#include <vtkProperty.h>
#include <vtkScalarBarRepresentation.h>
#include <vtkTextProperty.h>
#include <vtkTextRepresentation.h>

//! Initializes VTK lookup table charged with default color scheme for
//! scalar mapping. This default color scheme is built so that to
//! cover the given range of scalar values.
//! \param theRangeMin [in] minimal scalar value.
//! \param theRangeMax [in] maximal scalar value.
//! \return VTK lookup table.
vtkSmartPointer<vtkLookupTable>
  asiVisu_MeshResultUtils::InitLookupTable(const double theRangeMin,
                                           const double theRangeMax)
{
  vtkSmartPointer<vtkLookupTable> lookup = vtkSmartPointer<vtkLookupTable>::New();
  lookup->SetTableRange(theRangeMin, theRangeMax);

  // Rainbow colors.
  lookup->SetHueRange(0.66667, 0.0);

  // Grayscale (for monochrome printing).
  /*
  lookup->SetHueRange(0.0, 0.1);
  lookup->SetValueRange(1.0, 0);
  lookup->SetSaturationRange(0.0, 0.0);
  */
  return lookup;
}

//! Initializes the passed VTK mapper with the given Lookup Table.
//! \param theMapper         [in/out] mapper to initialize.
//! \param theLookup         [in]     Lookup Table to initialize the mapper with.
//! \param theScalarsArrName [in]     name of the array storing the scalars
//!                                   for colorization.
//! \param doInterpolation   [in]     indicates whether to ask mapper to interpolate
//!                                   scalars before actual mapping.
void asiVisu_MeshResultUtils::InitCellScalarMapper(vtkMapper*      theMapper,
                                                   vtkLookupTable* theLookup,
                                                   const char*     theScalarsArrName,
                                                   const bool      doInterpolation)
{
  theMapper->ScalarVisibilityOn();
  theMapper->SetScalarModeToUseCellData();
  theMapper->SelectColorArray(theScalarsArrName);
  theMapper->SetColorModeToMapScalars();
  theMapper->SetScalarRange( theLookup->GetRange() );
  theMapper->SetLookupTable(theLookup);

  if ( doInterpolation )
    theMapper->InterpolateScalarsBeforeMappingOn();
  else
    theMapper->InterpolateScalarsBeforeMappingOff();

  theMapper->Update();
}

//! Initializes the passed VTK mapper with the default Lookup Table.
//! \param theMapper         [in/out] mapper to initialize.
//! \param theScalarsArrName [in]     name of the array storing the scalars
//!                                   for colorization.
//! \param theRangeMin       [in]     minimal scalar value.
//! \param theRangeMax       [in]     maximal scalar value.
//! \param doInterpolation   [in]     indicates whether to ask mapper to interpolate
//!                                   scalars before actual mapping.
void asiVisu_MeshResultUtils::InitCellScalarMapper(vtkMapper*   theMapper,
                                                  const char*  theScalarsArrName,
                                                  const double theRangeMin,
                                                  const double theRangeMax,
                                                  const bool   doInterpolation)
{
  vtkSmartPointer<vtkLookupTable> aLookup = InitLookupTable(theRangeMin, theRangeMax);
  InitCellScalarMapper(theMapper, aLookup, theScalarsArrName, doInterpolation);
}

//! Initializes the passed VTK mapper with the given Lookup Table.
//! \param theMapper         [in/out] mapper to initialize.
//! \param theLookup         [in]     Lookup Table to initialize the mapper with.
//! \param theScalarsArrName [in]     name of the array storing the scalars
//!                                   for colorization.
//! \param doInterpolation   [in]     indicates whether to ask mapper to interpolate
//!                                   scalars before actual mapping.
void asiVisu_MeshResultUtils::InitPointScalarMapper(vtkMapper*      theMapper,
                                                    vtkLookupTable* theLookup,
                                                    const char*     theScalarsArrName,
                                                    const bool      doInterpolation)
{
  theMapper->ScalarVisibilityOn();
  theMapper->SetScalarModeToUsePointData();
  theMapper->SelectColorArray(theScalarsArrName);
  theMapper->SetColorModeToMapScalars();
  theMapper->SetScalarRange( theLookup->GetRange() );
  theMapper->SetLookupTable(theLookup);

  if ( doInterpolation )
    theMapper->InterpolateScalarsBeforeMappingOn();
  else
    theMapper->InterpolateScalarsBeforeMappingOff();

  theMapper->Update();
}

//! Initializes the passed VTK mapper with the default Lookup Table.
//! \param theMapper         [in/out] mapper to initialize.
//! \param theScalarsArrName [in]     name of the array storing the scalars
//!                                   for colorization.
//! \param theRangeMin       [in]     minimal scalar value.
//! \param theRangeMax       [in]     maximal scalar value.
//! \param doInterpolation   [in]     indicates whether to ask mapper to interpolate
//!                                   scalars before actual mapping.
void asiVisu_MeshResultUtils::InitPointScalarMapper(vtkMapper*   theMapper,
                                                    const char*  theScalarsArrName,
                                                    const double theRangeMin,
                                                    const double theRangeMax,
                                                    const bool   doInterpolation)
{
  vtkSmartPointer<vtkLookupTable> aLookup = InitLookupTable(theRangeMin, theRangeMax);
  InitPointScalarMapper(theMapper, aLookup, theScalarsArrName, doInterpolation);
}

//! Initializes the passed scalar bar widget for scenes containing
//! analysis results.
//! \param theScalarBarWidget [in] scalar bar widget to initialize.
void asiVisu_MeshResultUtils::InitScalarBarWidget(vtkScalarBarWidget* scalarBarWidget,
                                                  const int           orientation)
{
  vtkScalarBarRepresentation* rep = scalarBarWidget->GetScalarBarRepresentation();
  rep->SetOrientation(orientation);
  rep->GetPositionCoordinate()->SetValue(0.9, 0.2);
  rep->GetPosition2Coordinate()->SetValue(0.1, 0.6);
}

//! Returns polygonal source for VTK glyph representing vectorial data.
//! \return polygonal source of the mentioned glyph.
vtkSmartPointer<vtkPolyDataAlgorithm> asiVisu_MeshResultUtils::GetVectorGlyph()
{
  vtkSmartPointer<vtkGlyphSource2D> aResult = vtkSmartPointer<vtkGlyphSource2D>::New();
  aResult->SetGlyphTypeToArrow();
  aResult->SetFilled(0);
  return aResult;
}

//! Returns VTK Transformation object describing the relative transformation
//! to apply on the vector glyphs. The actual transformation defined by this
//! method moves the glyph so that it starts from the point it is imposed to.
//! \return VTK transformation object describing the relative transformation
//!         to apply on each glyph being rendered.
vtkSmartPointer<vtkTransform> asiVisu_MeshResultUtils::GetVectorGlyphTransform()
{
  vtkSmartPointer<vtkTransform> aResult = vtkSmartPointer<vtkTransform>::New();
  aResult->Translate(0.5, 0.0, 0.0);
  return aResult;
}

//! Sets the predefined lighting options to the passed Actor.
//! \param actor [in] Actor to adjust lighting options.
void asiVisu_MeshResultUtils::ApplySoftLightingRules(vtkActor* actor)
{
  actor->GetProperty()->SetAmbient(0.9);
  actor->GetProperty()->SetDiffuse(0.1);
  actor->GetProperty()->SetSpecular(0.1);
}
