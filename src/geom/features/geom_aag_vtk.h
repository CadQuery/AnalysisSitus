//-----------------------------------------------------------------------------
// Created on: 21 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_aag_vtk_h
#define geom_aag_vtk_h

// A-Situs includes
#include <analysis_situs.h>

// Geometry includes
#include <geom_aag.h>

// VTK includes
#include <vtkMutableUndirectedGraph.h>
#include <vtkSmartPointer.h>

//! Converter of AAG to VTK presentable graph data structure.
namespace geom_aag_vtk
{
  ASitus_EXPORT vtkSmartPointer<vtkMutableUndirectedGraph>
    Convert(const Handle(geom_aag)& aag);
};

#endif
