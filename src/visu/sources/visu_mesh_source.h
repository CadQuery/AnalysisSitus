//-----------------------------------------------------------------------------
// Created on: 20 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_mesh_source_h
#define visu_mesh_source_h

// A-Situs includes
#include <analysis_situs.h>

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
class visu_mesh_source : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(visu_mesh_source, vtkPolyDataAlgorithm);
  static visu_mesh_source* New();

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

  visu_mesh_source();
  ~visu_mesh_source();

private:

  visu_mesh_source(const visu_mesh_source&);
  visu_mesh_source& operator=(const visu_mesh_source&);

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
