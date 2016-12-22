//-----------------------------------------------------------------------------
// Created on: 20 November 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_MeshResultUtils_h
#define asiVisu_MeshResultUtils_h

// Visualization includes
#include <asiVisu.h>

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
};

#endif
