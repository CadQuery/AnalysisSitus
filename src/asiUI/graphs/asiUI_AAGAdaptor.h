//-----------------------------------------------------------------------------
// Created on: 21 March 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiUI_AAGAdaptor_h
#define asiUI_AAGAdaptor_h

// asAlgo includes
#include <asiAlgo_AAG.h>

// VTK includes
#include <vtkMutableUndirectedGraph.h>
#include <vtkSmartPointer.h>

//! Converter of AAG to VTK presentable graph data structure.
namespace asiUI_AAGAdaptor
{
  vtkSmartPointer<vtkMutableUndirectedGraph>
    Convert(const Handle(asiAlgo_AAG)& aag);
};

#endif
