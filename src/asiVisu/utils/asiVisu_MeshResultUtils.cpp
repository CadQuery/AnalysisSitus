//-----------------------------------------------------------------------------
// Created on: 20 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_mesh_result_utils.h>

// VTK includes
#include <vtkCoordinate.h>
#include <vtkGlyphSource2D.h>
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
  visu_mesh_result_utils::InitLookupTable(const double theRangeMin,
                                          const double theRangeMax)
{
  vtkSmartPointer<vtkLookupTable> aLookup = vtkSmartPointer<vtkLookupTable>::New();
  aLookup->SetTableRange(theRangeMin, theRangeMax);
  aLookup->SetHueRange(0.66667, 0.0);
  return aLookup;
}

//! Initializes the passed VTK mapper with the given Lookup Table.
//! \param theMapper         [in/out] mapper to initialize.
//! \param theLookup         [in]     Lookup Table to initialize the mapper with.
//! \param theScalarsArrName [in]     name of the array storing the scalars
//!                                   for colorization.
//! \param doInterpolation   [in]     indicates whether to ask mapper to interpolate
//!                                   scalars before actual mapping.
void visu_mesh_result_utils::InitCellScalarMapper(vtkMapper*      theMapper,
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
void visu_mesh_result_utils::InitCellScalarMapper(vtkMapper*   theMapper,
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
void visu_mesh_result_utils::InitPointScalarMapper(vtkMapper*      theMapper,
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
void visu_mesh_result_utils::InitPointScalarMapper(vtkMapper*   theMapper,
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
void visu_mesh_result_utils::InitScalarBarWidget(vtkScalarBarWidget* theScalarBarWidget)
{
  vtkScalarBarRepresentation* aRep = theScalarBarWidget->GetScalarBarRepresentation();
  aRep->SetOrientation(1);
  aRep->GetPositionCoordinate()->SetValue(0.9, 0.2);
  aRep->GetPosition2Coordinate()->SetValue(0.1, 0.6);
}

//! Returns polygonal source for VTK glyph representing vectorial data.
//! \return polygonal source of the mentioned glyph.
vtkSmartPointer<vtkPolyDataAlgorithm> visu_mesh_result_utils::GetVectorGlyph()
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
vtkSmartPointer<vtkTransform> visu_mesh_result_utils::GetVectorGlyphTransform()
{
  vtkSmartPointer<vtkTransform> aResult = vtkSmartPointer<vtkTransform>::New();
  aResult->Translate(0.5, 0.0, 0.0);
  return aResult;
}
