//-----------------------------------------------------------------------------
// Created on: 20 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
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