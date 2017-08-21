//-----------------------------------------------------------------------------
// Created on: 14 December 2015
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

// Own include
#include <asiAlgo_MeshConvert.h>

// Mesh includes
#include <asiAlgo_MeshMerge.h>

// VTK includes
#include <vtkCellArray.h>
#include <vtkIdList.h>
#include <vtkSmartPointer.h>

//-----------------------------------------------------------------------------

//! Converts internal mesh of the given shape to a persistent form.
//! \param source [in]  source shape (mesh keeper).
//! \param result [out] mesh in a persistent form.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_MeshConvert::ToPersistent(const TopoDS_Shape&   source,
                                       Handle(ActData_Mesh)& result)
{
  asiAlgo_MeshMerge conglomerate(source, asiAlgo_MeshMerge::Mode_Mesh);
  result = conglomerate.GetResultMesh();
  return true;
}

//-----------------------------------------------------------------------------

//! Converts from Poly mesh to AD mesh.
//! \param source [in]  Poly mesh.
//! \param result [out] mesh in a persistent form.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_MeshConvert::ToPersistent(const Handle(Poly_Triangulation)& source,
                                       Handle(ActData_Mesh)&             result)
{
  result = new ActData_Mesh(source);
  return true;
}

//-----------------------------------------------------------------------------

//! Converts VTK polygonal data set to the persistent one.
//! \param source [in]  VTK polygonal data set.
//! \param result [out] persistent mesh.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_MeshConvert::ToPersistent(vtkPolyData*          source,
                                       Handle(ActData_Mesh)& result)
{
  // Elements
  const vtkIdType numCells = source->GetNumberOfCells();
  if ( numCells )
    result = new ActData_Mesh;

  // Mapping between VTK nodes against stored nodes
  NCollection_DataMap<vtkIdType, int> NodeMap;

  // Points
  const vtkIdType numPts = source->GetNumberOfPoints();
  for ( vtkIdType ptId = 0; ptId < numPts; ++ptId )
  {
    double coord[3] = {0.0, 0.0, 0.0};
    source->GetPoint(ptId, coord);
    const int storedNodeId = result->AddNode(coord[0], coord[1], coord[2]);
    //
    NodeMap.Bind(ptId, storedNodeId);
  }

  // Loop over the VTK polygonal cells
  vtkSmartPointer<vtkIdList> ptIds = vtkSmartPointer<vtkIdList>::New();
  ptIds->Allocate(VTK_CELL_SIZE);
  //
  for ( vtkIdType cellId = 0; cellId < numCells; ++cellId )
  {
    const int cellType = source->GetCellType(cellId);
    if ( cellType != VTK_TRIANGLE )
    {
      std::cout << "Warning: non-triangular elements are not supported" << std::endl;
      continue;
    }

    // Get the list of points for this cell
    source->GetCellPoints(cellId, ptIds);

    // Add face
    const int id0 = NodeMap.Find( ptIds->GetId(0) );
    const int id1 = NodeMap.Find( ptIds->GetId(1) );
    const int id2 = NodeMap.Find( ptIds->GetId(2) );
    //
    result->AddFace(id0, id1, id2);
  }
  return true;
}