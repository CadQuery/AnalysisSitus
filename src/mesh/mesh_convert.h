//-----------------------------------------------------------------------------
// Created on: 14 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef mesh_convert_h
#define mesh_convert_h

// A-Situs includes
#include <analysis_situs.h>

// OMFDS includes
#include <OMFDS_Mesh.hxx>

// VTK includes
#include <vtkPolyData.h>

//! Services to convert mesh from one format to another.
namespace mesh_convert
{
  bool
    ToPersistent(const TopoDS_Shape& source,
                 Handle(OMFDS_Mesh)& result);

  bool
    ToPersistent(vtkPolyData*        source,
                 Handle(OMFDS_Mesh)& result);
};

#endif
