//-----------------------------------------------------------------------------
// Created on: 14 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <tess_convert.h>

// Mesh includes
#include <tess_merge.h>

// VTK includes
#include <vtkCellArray.h>
#include <vtkIdList.h>
#include <vtkSmartPointer.h>

//-----------------------------------------------------------------------------

//! Converts internal mesh of the given shape to a persistent form.
//! \param source [in]  source shape (mesh keeper).
//! \param result [out] mesh in a persistent form.
//! \return true in case of success, false -- otherwise.
bool tess_convert::ToPersistent(const TopoDS_Shape& source,
                                Handle(Mesh)& result)
{
  tess_merge conglomerate(source, tess_merge::Mode_Mesh);
  result = conglomerate.GetResultMesh();
  return true;
}

//-----------------------------------------------------------------------------

//! Converts from Poly mesh to AD mesh.
//! \param source [in]  Poly mesh.
//! \param result [out] mesh in a persistent form.
//! \return true in case of success, false -- otherwise.
bool tess_convert::ToPersistent(const Handle(Poly_Triangulation)& source,
                                Handle(Mesh)&               result)
{
  result = new Mesh(source);
  return true;
}

//-----------------------------------------------------------------------------

//! Converts VTK polygonal data set to the persistent one.
//! \param source [in]  VTK polygonal data set.
//! \param result [out] persistent mesh.
//! \return true in case of success, false -- otherwise.
bool tess_convert::ToPersistent(vtkPolyData*  source,
                                Handle(Mesh)& result)
{
  // Elements
  const vtkIdType numCells = source->GetNumberOfCells();
  if ( numCells )
    result = new Mesh;

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
