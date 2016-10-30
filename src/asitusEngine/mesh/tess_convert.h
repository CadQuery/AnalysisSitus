//-----------------------------------------------------------------------------
// Created on: 14 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef tess_convert_h
#define tess_convert_h

// A-Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <Poly_Triangulation.hxx>

// Mesh (Active Data) includes
#include <Mesh.h>

// VTK includes
#include <vtkPolyData.h>

//! Services to convert mesh from one format to another.
namespace tess_convert
{
  bool
    ToPersistent(const TopoDS_Shape& source,
                 Handle(Mesh)&       result);

  bool
    ToPersistent(const Handle(Poly_Triangulation)& source,
                 Handle(Mesh)&                     result);

  bool
    ToPersistent(vtkPolyData*  source,
                 Handle(Mesh)& result);
};

#endif
