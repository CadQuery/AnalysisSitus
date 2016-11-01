//-----------------------------------------------------------------------------
// Created on: 21 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef feature_aag_vtk_h
#define feature_aag_vtk_h

// A-Situs includes
#include <analysis_situs.h>

// Feature includes
#include <feature_aag.h>

// VTK includes
#include <vtkMutableUndirectedGraph.h>
#include <vtkSmartPointer.h>

//! Converter of AAG to VTK presentable graph data structure.
namespace feature_aag_vtk
{
  ASitus_EXPORT vtkSmartPointer<vtkMutableUndirectedGraph>
    Convert(const Handle(feature_aag)& aag);
};

#endif
