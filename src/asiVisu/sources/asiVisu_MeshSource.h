//-----------------------------------------------------------------------------
// Created on: 20 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
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
    SetInputMesh(const Handle(Mesh)& theMesh);

  void
    SetInputElemGroup(const Handle(Mesh_Group)& theGroup);

  const Handle(Mesh)&
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
  Handle(Mesh) m_mesh;

  //! Group of mesh elements to be extracted from the input data set. If this
  //! group is empty, the entire mesh is taken as-is, without any filtering.
  Handle(Mesh_Group) m_elemGroup;

  //! Registered VTK points.
  NCollection_DataMap<int, vtkIdType> m_regPoints;

  //! Role trigger for empty mesh groups.
  bool m_bIsEmptyGroupForAll;

};

#endif
