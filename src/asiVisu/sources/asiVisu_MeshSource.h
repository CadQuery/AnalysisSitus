//-----------------------------------------------------------------------------
// Created on: 20 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef asiVisu_MeshSource_h
#define asiVisu_MeshSource_h

// asiVisu includes
#include <asiVisu.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

// Mesh (Active Data) includes
#include <ActData_Mesh.h>
#include <ActData_Mesh_Group.h>

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
    SetInputElemGroup(const Handle(ActData_Mesh_Group)& theGroup);

  const Handle(ActData_Mesh)&
    GetInputMesh() const;

  const Handle(ActData_Mesh_Group)&
    GetInputElemGroup() const;

protected:

  virtual int RequestData(vtkInformation*        theRequest,
                          vtkInformationVector** theInputVector,
                          vtkInformationVector*  theOutputVector);

private:

  void
    translateElement(const Handle(ActData_Mesh_Element)& theElem,
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
  Handle(ActData_Mesh_Group) m_elemGroup;

  //! Registered VTK points.
  NCollection_DataMap<int, vtkIdType> m_regPoints;

  //! Role trigger for empty mesh groups.
  bool m_bIsEmptyGroupForAll;

};

#endif
