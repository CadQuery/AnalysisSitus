//-----------------------------------------------------------------------------
// Created on: 20 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_MeshResultUtils_h
#define asiVisu_MeshResultUtils_h

// Visualization includes
#include <asiVisu_Common.h>

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
  vtkSmartPointer<vtkLookupTable>
    InitLookupTable(const double theRangeMin,
                    const double theRangeMax);

  void
    InitCellScalarMapper(vtkMapper* theMapper,
                         vtkLookupTable* theLookup,
                         const char* theScalarsArrName,
                         const bool doInterpolation = true);

  void
    InitCellScalarMapper(vtkMapper* theMapper,
                         const char* theScalarsArrName,
                         const double theRangeMin,
                         const double theRangeMax,
                         const bool doInterpolation = true);

  void
    InitPointScalarMapper(vtkMapper* theMapper,
                          vtkLookupTable* theLookup,
                          const char* theScalarsArrName,
                          const bool doInterpolation = true);

  void
    InitPointScalarMapper(vtkMapper* theMapper,
                          const char* theScalarsArrName,
                          const double theRangeMin,
                          const double theRangeMax,
                          const bool doInterpolation = true);

  void
    InitScalarBarWidget(vtkScalarBarWidget* theScalarBarWidget);

  vtkSmartPointer<vtkPolyDataAlgorithm>
    GetVectorGlyph();

  vtkSmartPointer<vtkTransform>
    GetVectorGlyphTransform();
};

#endif
