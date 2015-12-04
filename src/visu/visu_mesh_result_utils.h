//-----------------------------------------------------------------------------
// Created on: 20 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef visu_mesh_result_utils_h
#define visu_mesh_result_utils_h

// Visualization includes
#include <visu_common.h>

// VTK includes
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
namespace visu_mesh_result_utils
{
  ASitus_EXPORT vtkSmartPointer<vtkLookupTable>
    InitLookupTable(const double theRangeMin,
                    const double theRangeMax);

  ASitus_EXPORT void
    InitCellScalarMapper(vtkMapper* theMapper,
                         vtkLookupTable* theLookup,
                         const char* theScalarsArrName,
                         const bool doInterpolation = true);

  ASitus_EXPORT void
    InitCellScalarMapper(vtkMapper* theMapper,
                         const char* theScalarsArrName,
                         const double theRangeMin,
                         const double theRangeMax,
                         const bool doInterpolation = true);

  ASitus_EXPORT void
    InitPointScalarMapper(vtkMapper* theMapper,
                          vtkLookupTable* theLookup,
                          const char* theScalarsArrName,
                          const bool doInterpolation = true);

  ASitus_EXPORT void
    InitPointScalarMapper(vtkMapper* theMapper,
                          const char* theScalarsArrName,
                          const double theRangeMin,
                          const double theRangeMax,
                          const bool doInterpolation = true);

  ASitus_EXPORT void
    InitScalarBarWidget(vtkScalarBarWidget* theScalarBarWidget);

  ASitus_EXPORT vtkSmartPointer<vtkPolyDataAlgorithm>
    GetVectorGlyph();

  ASitus_EXPORT vtkSmartPointer<vtkTransform>
    GetVectorGlyphTransform();
};

#endif
