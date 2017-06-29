//-----------------------------------------------------------------------------
// Created on: 20 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef asiVisu_MeshSource_h
#define asiVisu_MeshSource_h

// A-Situs includes
#include <asiVisu.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

// Mesh (Active Data) includes
#include <Mesh.h>
#include <Mesh_Group.h>

// OCCT includes
#include <NCollection_DataMap.hxx>
#include <NCollection_Sequence.hxx>

//! Source of polygonal data representing mesh.
class asiVisu_MeshSource : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_MeshSource, vtkPolyDataAlgorithm);
  static asiVisu_MeshSource* New();

// Kernel:
public:

  void
    EmptyGroupForAllModeOn();

  void
    EmptyGroupForAllModeOff();

  void
    SetInputMesh(const Handle(ActData_Mesh)& theMesh);

  void
    SetInputElemGroup(const Handle(Mesh_Group)& theGroup);

  const Handle(ActData_Mesh)&
    GetInputMesh() const;

  const Handle(Mesh_Group)&
    GetInputElemGroup() const;

protected:

  virtual int RequestData(vtkInformation*        theRequest,
                          vtkInformationVector** theInputVector,
                          vtkInformationVector*  theOutputVector);

private:

  void
    translateElement(const Handle(Mesh_Element)& theElem,
                     vtkPolyData*                thePolyData);

  vtkIdType
    registerMeshNode(const int    theNodeID,
                     vtkPolyData* thePolyData);

  vtkIdType
    registerMeshFace(const int    theFaceID,
                     const void*  theNodes,
                     const int    theNbNodes,
                     vtkPolyData* thePolyData);

  vtkIdType
    registerFreeNodesCell(const NCollection_Sequence<int>& theNodeIDs,
                          vtkPolyData*                     thePolyData);

private:

  asiVisu_MeshSource();
  ~asiVisu_MeshSource();

private:

  asiVisu_MeshSource(const asiVisu_MeshSource&);
  asiVisu_MeshSource& operator=(const asiVisu_MeshSource&);

private:
  
  //! Initial Mesh DS being converted to VTK polygonal data.
  Handle(ActData_Mesh) m_mesh;

  //! Group of mesh elements to be extracted from the input data set. If this
  //! group is empty, the entire mesh is taken as-is, without any filtering.
  Handle(Mesh_Group) m_elemGroup;

  //! Registered VTK points.
  NCollection_DataMap<int, vtkIdType> m_regPoints;

  //! Role trigger for empty mesh groups.
  bool m_bIsEmptyGroupForAll;

};

#endif
