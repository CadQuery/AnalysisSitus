//-----------------------------------------------------------------------------
// Created on: 21 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeFeatures_AAGAdaptor_h
#define exeFeatures_AAGAdaptor_h

// Feature includes
#include <feature_aag.h>

// VTK includes
#include <vtkMutableUndirectedGraph.h>
#include <vtkSmartPointer.h>

//! Converter of AAG to VTK presentable graph data structure.
namespace exeFeatures_AAGAdaptor
{
  vtkSmartPointer<vtkMutableUndirectedGraph>
    Convert(const Handle(feature_aag)& aag);
};

#endif
