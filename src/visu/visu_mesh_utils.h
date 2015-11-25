//-----------------------------------------------------------------------------
// Created on: 20 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef visu_mesh_utils_h
#define visu_mesh_utils_h

// Visualization includes
#include <visu_common.h>

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
namespace visu_mesh_utils
{
  ASitus_EXPORT vtkSmartPointer<vtkLookupTable>
    InitLookupTable();

  ASitus_EXPORT void
    InitMapper(vtkMapper* theMapper,
               vtkLookupTable* theLookup,
               const char* theScalarsArrName);

  ASitus_EXPORT void
    InitMapper(vtkMapper* theMapper,
               const char* theScalarsArrName);

  ASitus_EXPORT double
    DefaultShrinkFactor();

  ASitus_EXPORT double
    DefaultPointSize();

  ASitus_EXPORT void
    DefaultContourColor(double& fR,
                        double& fG,
                        double& fB);

  ASitus_EXPORT double
    DefaultContourLineWidth();

  ASitus_EXPORT double
    DefaultContourOpacity();
};

#endif
