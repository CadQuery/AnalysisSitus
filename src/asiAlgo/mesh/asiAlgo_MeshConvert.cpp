//-----------------------------------------------------------------------------
// Created on: 14 December 2015
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

// Own include
#include <asiAlgo_MeshConvert.h>

// asiAlgo includes
#include <asiAlgo_MeshMerge.h>

// Active Data includes
#include <ActData_Mesh_ElementsIterator.h>
#include <ActData_Mesh_Quadrangle.h>
#include <ActData_Mesh_Triangle.h>

// VTK includes
#include <vtkCellArray.h>
#include <vtkIdList.h>
#include <vtkSmartPointer.h>

//-----------------------------------------------------------------------------

void ElemNodes(const Handle(ActData_Mesh)&             mesh,
               const Handle(ActData_Mesh_Element)&     elem,
               std::vector<Handle(ActData_Mesh_Node)>& nodes)
{
  nodes.clear();
  int  numNodes = elem->NbNodes();
  int* nodeIDs  = NULL;

  if ( numNodes == 3 )
  {
    const Handle(ActData_Mesh_Triangle)&
      tri = Handle(ActData_Mesh_Triangle)::DownCast(elem);
    //
    nodeIDs = (int*) tri->GetConnections();
  }
  else if ( numNodes == 4 )
  {
    const Handle(ActData_Mesh_Quadrangle)&
      quad = Handle(ActData_Mesh_Quadrangle)::DownCast(elem);
    //
    nodeIDs = (int*) quad->GetConnections();
  }

  // Populate output collection.
  for ( int n = 0; n < numNodes; ++n )
    nodes.push_back( mesh->FindNode(nodeIDs[n]) );
}

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

//! Converts AD mesh to Poly mesh. All quads as split into triangle pairs.
//! \param[in]  source AD mesh to convert.
//! \param[out] result result mesh.
bool asiAlgo_MeshConvert::FromPersistent(const Handle(ActData_Mesh)& source,
                                         Handle(Poly_Triangulation)& result)
{
  const int numNodes = source->NbNodes();

  // Loop over the nodes of AD mesh to populate those of Poly triangulation.
  TColgp_Array1OfPnt polyNodes(1, numNodes);
  int arrIdx = 1;
  NCollection_DataMap<int, int> nodeIdsMap;
  //
  for ( ActData_Mesh_ElementsIterator nit(source, ActData_Mesh_ET_Node); nit.More(); nit.Next(), ++arrIdx )
  {
    // Access next node with its owner mesh elements.
    const Handle(ActData_Mesh_Node)&
      node = Handle(ActData_Mesh_Node)::DownCast( nit.GetValue() );

    polyNodes(arrIdx) = node->Pnt();
    nodeIdsMap.Bind(node->GetID(), arrIdx);
  }

  // Prepare triangles.
  std::vector<Poly_Triangle> triangles;
  //
  for ( ActData_Mesh_ElementsIterator it(source, ActData_Mesh_ET_Face); it.More(); it.Next() )
  {
    const Handle(ActData_Mesh_Element)& elem = it.GetValue();

    // Get element's nodes.
    std::vector<Handle(ActData_Mesh_Node)> elemNodes;
    ElemNodes(source, elem, elemNodes);

    // Compute norm vector.
    if ( elem->IsKind( STANDARD_TYPE(ActData_Mesh_Triangle) ) )
    {
      const int n0 = elemNodes[0]->GetID();
      const int n1 = elemNodes[1]->GetID();
      const int n2 = elemNodes[2]->GetID();

      triangles.push_back( Poly_Triangle( nodeIdsMap(n0), nodeIdsMap(n1), nodeIdsMap(n2) ) );
    }
    else if ( elem->IsKind( STANDARD_TYPE(ActData_Mesh_Quadrangle) ) )
    {
      const int n0 = elemNodes[0]->GetID();
      const int n1 = elemNodes[1]->GetID();
      const int n2 = elemNodes[2]->GetID();
      const int n3 = elemNodes[3]->GetID();

      triangles.push_back( Poly_Triangle( nodeIdsMap(n0), nodeIdsMap(n1), nodeIdsMap(n2) ) );
      triangles.push_back( Poly_Triangle( nodeIdsMap(n0), nodeIdsMap(n2), nodeIdsMap(n3) ) );
    }
  }

  // Construct array of triangles.
  Poly_Array1OfTriangle polyTriangles( 1, int( triangles.size() ) );
  //
  for ( size_t t = 0; t < triangles.size(); ++t )
    polyTriangles( int(t + 1) ) = triangles[t];

  // Construct the result.
  result = new Poly_Triangulation(polyNodes, polyTriangles);
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

//-----------------------------------------------------------------------------

bool asiAlgo_MeshConvert::FromVTK(vtkPolyData*                source,
                                  Handle(Poly_Triangulation)& result)
{
  // Nodes.
  const vtkIdType numCells = source->GetNumberOfCells();

  if ( numCells == 0 )
    return false;

  // Calculate real amount of triangles and nodes in resulting mesh.
  // Some algorithms, like healing can produce linear segments instead of triangles.
  std::vector<gp_Pnt> nodes;
  std::vector<std::vector<int>> faces;
  for ( vtkIdType cellId = 0; cellId < numCells; ++cellId )
  {
    const int cellType = source->GetCellType(cellId);
    vtkSmartPointer<vtkIdList> ptIds = vtkSmartPointer<vtkIdList>::New();
    if ( cellType != VTK_TRIANGLE )
      continue;

    // Get the list of points for this cell
    source->GetCellPoints(cellId, ptIds);

    double coord1[3] = { 0.0, 0.0, 0.0 };
    double coord2[3] = { 0.0, 0.0, 0.0 };
    double coord3[3] = { 0.0, 0.0, 0.0 };

    const int node1 = ptIds->GetId(0);
    const int node2 = ptIds->GetId(1);
    const int node3 = ptIds->GetId(2);

    source->GetPoint(node1, coord1);
    source->GetPoint(node2, coord2);
    source->GetPoint(node3, coord3);

    const gp_Pnt pnt1(coord1[0],coord1[1],coord1[2]);
    const gp_Pnt pnt2(coord2[0],coord2[1],coord2[2]);
    const gp_Pnt pnt3(coord3[0],coord3[1],coord3[2]);
    int resIdx1 = 0, resIdx2 = 0, resIdx3 = 0;

    nodes.push_back(pnt1);
    nodes.push_back(pnt2);
    nodes.push_back(pnt3);

    std::vector<int> indexes;
    indexes.push_back(resIdx1);
    indexes.push_back(resIdx2);
    indexes.push_back(resIdx3);
    faces.push_back(indexes);
  }

  // Construct resulting mesh and copy data.
  result = new Poly_Triangulation((int) nodes.size(), (int) faces.size(), false);
  for (int i = 0; i < (int)nodes.size(); ++i)
    result->ChangeNode(i + 1) = nodes[i];
  for (int i = 0; i < (int)faces.size(); ++i)
    result->ChangeTriangle(i + 1).Set(faces[i][0], faces[i][1], faces[i][2]);

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_MeshConvert::ToVTK(const Handle(Poly_Triangulation)& source,
                                vtkSmartPointer<vtkPolyData>&     result)
{
  result = vtkSmartPointer<vtkPolyData>::New();
  result->Allocate(); // TODO: is that necessary?

  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  result->SetPoints(points);

  // Processed nodes
  NCollection_DataMap<int, vtkIdType> nodeRepo;

  // Pass mesh elements to VTK data source
  const Poly_Array1OfTriangle& aTriangles = source->ChangeTriangles();
  for ( int i = aTriangles.Lower(); i <= aTriangles.Upper(); ++i)
    __translateElement(source, aTriangles(i), result, nodeRepo);

  return true;
}

//-----------------------------------------------------------------------------

void asiAlgo_MeshConvert::__translateElement(const Handle(Poly_Triangulation)&    source,
                                             const Poly_Triangle&                 elem,
                                             vtkPolyData*                         polyData,
                                             NCollection_DataMap<int, vtkIdType>& nodeRepo)
{
  int id[3];
  elem.Get(id[0], id[1], id[2]);

  // Transfer element to VTK polygonal source
  __registerMeshFace(source, &id[0], 3, polyData, nodeRepo);
}

//-----------------------------------------------------------------------------

vtkIdType
  asiAlgo_MeshConvert::__registerMeshFace(const Handle(Poly_Triangulation)&    source,
                                          const void*                          nodes,
                                          const int                            nbNodes,
                                          vtkPolyData*                         polyData,
                                          NCollection_DataMap<int, vtkIdType>& nodeRepo)
{
  // Define the type of the cell being registered
  VTKCellType cellType = VTK_EMPTY_CELL;
  switch ( nbNodes )
  {
    case 3: cellType = VTK_TRIANGLE; break;
    case 4: cellType = VTK_QUAD; break;
    default: return -1;
  }

  // Register cell in the polygonal data set
  int* nodeIDs = (int*) nodes;
  std::vector<vtkIdType> pids;
  for ( int k = 0; k < nbNodes; k++ )
  {
    vtkIdType pid = __registerMeshNode(source, nodeIDs[k], polyData, nodeRepo);
    pids.push_back(pid);
  }
  //
  vtkIdType
    cellID = polyData->InsertNextCell( cellType, (int) pids.size(), &pids[0] );

  return cellID;
}


//-----------------------------------------------------------------------------

vtkIdType
  asiAlgo_MeshConvert::__registerMeshNode(const Handle(Poly_Triangulation)&    source,
                                          const int                            nodeID,
                                          vtkPolyData*                         polyData,
                                          NCollection_DataMap<int, vtkIdType>& nodeRepo)
{
  vtkPoints* points    = polyData->GetPoints();
  vtkIdType* resPidPtr = nodeRepo.ChangeSeek(nodeID);

  if ( resPidPtr == NULL )
  {
    // Access mesh node
    const gp_Pnt& aPnt = source->Nodes().Value(nodeID);

    // Push the point into VTK data set
    resPidPtr = nodeRepo.Bound(nodeID, points->InsertNextPoint( aPnt.X(), aPnt.Y(), aPnt.Z() ));
  }

  return (*resPidPtr);
}
