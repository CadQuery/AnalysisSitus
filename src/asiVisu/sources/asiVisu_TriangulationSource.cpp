//-----------------------------------------------------------------------------
// Created on: 11 July 2017
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
//    * Neither the name of Sergey Slyadnev nor the
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
#include <asiVisu_TriangulationSource.h>

// asiAlgo includes
#include <asiAlgo_MeshLink.h>

// asiVisu includes
#include <asiVisu_MeshUtils.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkCellData.h>
#include <vtkDataObject.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

//-----------------------------------------------------------------------------

vtkStandardNewMacro(asiVisu_TriangulationSource);

//-----------------------------------------------------------------------------

asiVisu_TriangulationSource::asiVisu_TriangulationSource()
{
  // Enable collecting edges
  this->CollectEdgesModeOn();

  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline
}

//-----------------------------------------------------------------------------

asiVisu_TriangulationSource::~asiVisu_TriangulationSource()
{}

//-----------------------------------------------------------------------------

void asiVisu_TriangulationSource::SetInputTriangulation(const Handle(Poly_Triangulation)& triangulation)
{
  m_triangulation = triangulation;
  //
  this->Modified();
}

//-----------------------------------------------------------------------------

const Handle(Poly_Triangulation)& asiVisu_TriangulationSource::GetInputTriangulation() const
{
  return m_triangulation;
}

//-----------------------------------------------------------------------------

int asiVisu_TriangulationSource::RequestData(vtkInformation*        request,
                                             vtkInformationVector** inputVector,
                                             vtkInformationVector*  outputVector)
{
  if ( m_triangulation.IsNull() )
  {
    vtkErrorMacro( << "Invalid input: NULL triangulation" );
    return 0;
  }

  m_regPoints.Clear();

  /* ================
   *  Prepare output
   * ================ */

  // Get output polygonal data from the information vector
  vtkPolyData* polyOutput = vtkPolyData::GetData(outputVector);
  polyOutput->Allocate();
  polyOutput->SetPoints( vtkSmartPointer<vtkPoints>::New() );

  // Add array for mesh item types
  vtkSmartPointer<vtkIdTypeArray> typeArr = vtkSmartPointer<vtkIdTypeArray>::New();
  typeArr->SetName(ARRNAME_MESH_ITEM_TYPE);
  typeArr->SetNumberOfComponents(1);
  polyOutput->GetCellData()->AddArray(typeArr);

  // Add array for mesh element IDs
  vtkSmartPointer<vtkIdTypeArray> faceIDsArr = vtkSmartPointer<vtkIdTypeArray>::New();
  faceIDsArr->SetName(ARRNAME_MESH_ELEM_IDS);
  faceIDsArr->SetNumberOfComponents(1);
  polyOutput->GetCellData()->SetPedigreeIds(faceIDsArr);

  /* ==============================================================
   *  Take care of free nodes by collecting them into a dedicated
   *  VTK_POLY_VERTEX cell. Notice that such cell (as well as
   *  simple VTK_VERTEX) must be added FIRST when working with
   *  vtkPolyData objects in order to have the data set consistent
   * ============================================================== */

  // Internal collections of triangles and nodes
  const Poly_Array1OfTriangle& triangles = m_triangulation->Triangles();
  const TColgp_Array1OfPnt&    nodes     = m_triangulation->Nodes();

  // Collect all used nodes
  NCollection_Map<int> usedNodeIDs;
  //
  for ( int i = triangles.Lower(); i <= triangles.Upper(); ++i )
  {
    const Poly_Triangle& tri = triangles(i);

    // Get nodes
    int n1, n2, n3;
    tri.Get(n1, n2, n3);
    //
    usedNodeIDs.Add(n1);
    usedNodeIDs.Add(n2);
    usedNodeIDs.Add(n3);
  }

  // Collect free nodes (ones which are not used)
  NCollection_List<int> freeNodeIDs;
  //
  for ( int i = nodes.Lower(); i <= nodes.Upper(); ++i )
  {
    // Add as a free node
    if ( !usedNodeIDs.Contains(i) )
      freeNodeIDs.Append(i);
  }
  //
  if ( !freeNodeIDs.IsEmpty() )
  {
    m_progress.SendLogMessage( LogWarn(Normal) << "Num. free nodes: %1" << freeNodeIDs.Extent() );
    //
    this->registerFreeNodesCell(freeNodeIDs, polyOutput);
  }

  /* ===========
   *  Add edges
   * =========== */

  if ( m_bEdgesOn )
  {
    //   0 -- never happens by construction
    //   1 -- free link
    //   2 -- shared link
    // > 2 -- non-manifold link
    NCollection_DataMap<asiAlgo_MeshLink, int, asiAlgo_MeshLink> linkOccurenceMap;

    for ( int i = triangles.Lower(); i <= triangles.Upper(); ++i )
    {
      const Poly_Triangle& tri = triangles(i);

      // Get nodes
      int n1, n2, n3;
      tri.Get(n1, n2, n3);

      // Add unoriented links to the set of all links
      asiAlgo_MeshLink l1(n1, n2);
      asiAlgo_MeshLink l2(n1, n3);
      asiAlgo_MeshLink l3(n2, n3);
      //
      if ( linkOccurenceMap.IsBound(l1) )
        linkOccurenceMap.ChangeFind(l1)++;
      else
        linkOccurenceMap.Bind(l1, 1);
      //
      if ( linkOccurenceMap.IsBound(l2) )
        linkOccurenceMap.ChangeFind(l2)++;
      else
        linkOccurenceMap.Bind(l2, 1);
      //
      if ( linkOccurenceMap.IsBound(l3) )
        linkOccurenceMap.ChangeFind(l3)++;
      else
        linkOccurenceMap.Bind(l3, 1);
    }

    // Add links to VTK data set
    //
    int numFreeLinks = 0;
    int numNonManifoldLinks = 0;
    //
    for ( NCollection_DataMap<asiAlgo_MeshLink, int, asiAlgo_MeshLink>::Iterator lit(linkOccurenceMap); lit.More(); lit.Next() )
    {
      asiVisu_MeshPrimitive linkType;

      if ( lit.Value() == 1 )
      {
        numFreeLinks++;
        linkType = MeshPrimitive_BorderLink;
      }
      else if ( lit.Value() == 2 )
      {
        linkType = MeshPrimitive_ManifoldLink;
      }
      else if ( lit.Value() > 2 )
      {
        numNonManifoldLinks++;
        linkType = MeshPrimitive_NonManifoldLink;
      }
      else
      {
        linkType = MeshPrimitive_Undefined;
      }

      if ( lit.Key().N1 == lit.Key().N2 )
        m_progress.SendLogMessage(LogWarn(Normal) << "Topologically degenerated link skipped...");
      else
        this->registerLinkCell(lit.Key().N1, lit.Key().N2, linkType, polyOutput);
    }

    if ( numFreeLinks )
      m_progress.SendLogMessage( LogWarn(Normal) << "Num. free links: %1" << numFreeLinks );
    if ( numNonManifoldLinks )
      m_progress.SendLogMessage( LogWarn(Normal) << "Num. non-manifold links: %1" << numNonManifoldLinks );
  }

  /* ============
   *  Add facets
   * ============ */

  for ( int elemId = triangles.Lower(); elemId <= triangles.Upper(); ++elemId )
  {
    const Poly_Triangle& tri = triangles(elemId);

    // Get nodes
    int n1, n2, n3;
    tri.Get(n1, n2, n3);

    // Register VTK cell
    this->registerFacet(elemId, n1, n2, n3, polyOutput);
  }

  return Superclass::RequestData(request, inputVector, outputVector);
}

//-----------------------------------------------------------------------------

vtkIdType
  asiVisu_TriangulationSource::findMeshNode(const int    nodeID,
                                            vtkPolyData* polyData)
{
  // Access necessary arrays
  vtkPoints* points = polyData->GetPoints();

  // Access mesh node
  const gp_Pnt& node = m_triangulation->Node(nodeID);

  vtkIdType resPid;
  if ( !m_regPoints.IsBound(nodeID) )
  {
    // Push the point into VTK data set
    resPid = points->InsertNextPoint( node.X(), node.Y(), node.Z() );
    m_regPoints.Bind(nodeID, resPid);
  }
  else
    resPid = m_regPoints.Find(nodeID);

  return resPid;
}

//-----------------------------------------------------------------------------

vtkIdType
  asiVisu_TriangulationSource::registerFacet(const int    elemId,
                                             const int    nodeID1,
                                             const int    nodeID2,
                                             const int    nodeID3,
                                             vtkPolyData* polyData)
{
  std::vector<vtkIdType> pids =
  {
    this->findMeshNode(nodeID1, polyData),
    this->findMeshNode(nodeID2, polyData),
    this->findMeshNode(nodeID3, polyData)
  };

  // Register cell
  vtkIdType cellID = polyData->InsertNextCell(VTK_TRIANGLE, 3, &pids[0]);

  // Store element type
  vtkIdTypeArray*
    typeArr = vtkIdTypeArray::SafeDownCast( polyData->GetCellData()->GetArray(ARRNAME_MESH_ITEM_TYPE) );
  //
  typeArr->InsertNextValue(MeshPrimitive_FacetTriangle);

  // Store element ID
  vtkIdTypeArray*
    elemIDsArr = vtkIdTypeArray::SafeDownCast( polyData->GetCellData()->GetArray(ARRNAME_MESH_ELEM_IDS) );
  //
  elemIDsArr->InsertNextValue(elemId);

  return cellID;
}

//-----------------------------------------------------------------------------

vtkIdType
  asiVisu_TriangulationSource::registerFreeNodesCell(const NCollection_List<int>& nodeIDs,
                                                     vtkPolyData*                 polyData)
{
  if ( nodeIDs.IsEmpty() )
    return VTK_BAD_ID;

  std::vector<vtkIdType> pids;
  for ( NCollection_List<int>::Iterator it(nodeIDs); it.More(); it.Next() )
  {
    const int nodeID = it.Value();
    pids.push_back( this->findMeshNode(nodeID, polyData) );
  }

  // Create cell
  vtkIdType
    cellID = polyData->InsertNextCell( VTK_POLY_VERTEX, (int) pids.size(), &pids[0] );

  // Store primitive type
  vtkIdTypeArray*
    typeArr = vtkIdTypeArray::SafeDownCast( polyData->GetCellData()->GetArray(ARRNAME_MESH_ITEM_TYPE) );
  //
  typeArr->InsertNextValue(MeshPrimitive_FreeNode);

  // Store element ID
  vtkIdTypeArray*
    elemIDsArr = vtkIdTypeArray::SafeDownCast( polyData->GetCellData()->GetArray(ARRNAME_MESH_ELEM_IDS) );
  //
  elemIDsArr->InsertNextValue(VTK_BAD_ID);

  return cellID;
}

//-----------------------------------------------------------------------------

vtkIdType
    asiVisu_TriangulationSource::registerLinkCell(const int                   nodeID1,
                                                  const int                   nodeID2,
                                                  const asiVisu_MeshPrimitive type,
                                                  vtkPolyData*                polyData)
{
  if ( nodeID1 == VTK_BAD_ID || nodeID2 == VTK_BAD_ID )
    return VTK_BAD_ID;

  std::vector<vtkIdType> pids;
  pids.push_back( this->findMeshNode(nodeID1, polyData) );
  pids.push_back( this->findMeshNode(nodeID2, polyData) );

  // Register VTK cell
  vtkIdType cellID =
    polyData->InsertNextCell( VTK_LINE, (int) pids.size(), &pids[0] );

  // Set type of the mesh element
  vtkIdTypeArray*
    typeArr = vtkIdTypeArray::SafeDownCast( polyData->GetCellData()->GetArray(ARRNAME_MESH_ITEM_TYPE) );
  //
  typeArr->InsertNextValue(type);

  // Store element ID
  vtkIdTypeArray*
    elemIDsArr = vtkIdTypeArray::SafeDownCast( polyData->GetCellData()->GetArray(ARRNAME_MESH_ELEM_IDS) );
  //
  elemIDsArr->InsertNextValue(VTK_BAD_ID);

  return cellID;
}
