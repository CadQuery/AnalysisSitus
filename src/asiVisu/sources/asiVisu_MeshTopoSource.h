//-----------------------------------------------------------------------------
// Created on: 31 May 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_MeshTopoSource_h
#define asiVisu_MeshTopoSource_h

// A-Situs includes
#include <asiVisu.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

// Mesh (Active Data) includes
#include <Mesh.h>

//! Source of polygonal data representing mesh topology constraints
//! basing on the predefined fixed nodes.
class asiVisu_MeshTopoSource : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_MeshTopoSource, vtkPolyDataAlgorithm);
  static asiVisu_MeshTopoSource* New();

// Kernel:
public:

  void
    SetInputMesh(const Handle(Mesh)& mesh);

  const Handle(Mesh)&
    GetInputMesh() const;

  void
    SetBaseNodes(const TColStd_PackedMapOfInteger& nodes);

  const TColStd_PackedMapOfInteger&
    GetBaseNodes() const;

protected:

  virtual int RequestData(vtkInformation*        pRequest,
                          vtkInformationVector** pInputVector,
                          vtkInformationVector*  pOutputVector);

private:

  vtkIdType
    registerNode(const int     nodeId,
                 const gp_Pnt& coords,
                 vtkPolyData*  pPolyData);

  vtkIdType
    registerVertex(vtkIdType    n,
                   vtkPolyData* pPolyData);

  vtkIdType
    registerLink(vtkIdType    n1,
                 vtkIdType    n2,
                 vtkPolyData* pPolyData);

private:

  asiVisu_MeshTopoSource();
  ~asiVisu_MeshTopoSource();

private:

  asiVisu_MeshTopoSource(const asiVisu_MeshTopoSource&);
  asiVisu_MeshTopoSource& operator=(const asiVisu_MeshTopoSource&);

private:

  Handle(Mesh)               m_mesh;  //!< Host mesh.
  TColStd_PackedMapOfInteger m_nodes; //!< Base nodes.

};

#endif
