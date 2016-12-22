//-----------------------------------------------------------------------------
// Created on: 14 December 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_MeshConvert_h
#define asiAlgo_MeshConvert_h

// A-Situs includes
#include <asiAlgo.h>

// OCCT includes
#include <Poly_Triangulation.hxx>
#include <TopoDS_Shape.hxx>

// Mesh (Active Data) includes
#include <Mesh.h>

// VTK includes
#include <vtkPolyData.h>

//! Services to convert mesh from one format to another.
namespace asiAlgo_MeshConvert
{
  asiAlgo_EXPORT bool
    ToPersistent(const TopoDS_Shape& source,
                 Handle(Mesh)&       result);

  asiAlgo_EXPORT bool
    ToPersistent(const Handle(Poly_Triangulation)& source,
                 Handle(Mesh)&                     result);

  asiAlgo_EXPORT bool
    ToPersistent(vtkPolyData*  source,
                 Handle(Mesh)& result);
};

#endif
