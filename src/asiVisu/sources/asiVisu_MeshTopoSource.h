//-----------------------------------------------------------------------------
// Created on: 31 May 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_mesh_topo_source_h
#define visu_mesh_topo_source_h

// A-Situs includes
#include <analysis_situs.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

// Mesh (Active Data) includes
#include <Mesh.h>

//! Source of polygonal data representing mesh topology constraints
//! basing on the predefined fixed nodes.
class visu_mesh_topo_source : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(visu_mesh_topo_source, vtkPolyDataAlgorithm);
  static visu_mesh_topo_source* New();

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

  visu_mesh_topo_source();
  ~visu_mesh_topo_source();

private:

  visu_mesh_topo_source(const visu_mesh_topo_source&);
  visu_mesh_topo_source& operator=(const visu_mesh_topo_source&);

private:

  Handle(Mesh)               m_mesh;  //!< Host mesh.
  TColStd_PackedMapOfInteger m_nodes; //!< Base nodes.

};

#endif
