//-----------------------------------------------------------------------------
// Created on: 31 May 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_MeshTopoSource.h>

// Visualization includes
#include <asiVisu_MeshUtils.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkCellData.h>
#include <vtkDataObject.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

// Mesh (Active Data) includes
#include <Mesh_ElementsIterator.h>
#include <Mesh_Node.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------

vtkStandardNewMacro(asiVisu_MeshTopoSource);

//! Default constructor.
asiVisu_MeshTopoSource::asiVisu_MeshTopoSource()
{
  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline
}

//! Destructor.
asiVisu_MeshTopoSource::~asiVisu_MeshTopoSource()
{
}

//-----------------------------------------------------------------------------
// Kernel methods
//-----------------------------------------------------------------------------

//! Sets the instance of Mesh DS being used as an input for Data Source.
//! \param mesh [in] input Mesh DS.
void asiVisu_MeshTopoSource::SetInputMesh(const Handle(Mesh)& mesh)
{
  m_mesh = mesh;
}

//! Accessor for the input Mesh DS.
//! \return requested Mesh DS.
const Handle(Mesh)& asiVisu_MeshTopoSource::GetInputMesh() const
{
  return m_mesh;
}

//! Sets base nodes.
//! \param nodes [in] nodes to set.
void asiVisu_MeshTopoSource::SetBaseNodes(const TColStd_PackedMapOfInteger& nodes)
{
  m_nodes = nodes;
}

//! \return base nodes.
const TColStd_PackedMapOfInteger& asiVisu_MeshTopoSource::GetBaseNodes() const
{
  return m_nodes;
}

//! This is called by the superclass. Creates VTK polygonal data set
//! from the input Mesh DS.
//! \param pRequest      [in] information about data object.
//! \param pInputVector  [in] the input data. As a data source is the start
//!                           stage of the VTK pipeline, theInputVector is
//!                           empty and not used (no input port).
//! \param pOutputVector [in] the pointer to output data, that is filled
//!                           in this method.
//! \return status.
int asiVisu_MeshTopoSource::RequestData(vtkInformation*        pRequest,
                                       vtkInformationVector** pInputVector,
                                       vtkInformationVector*  pOutputVector)
{
  // Prepare back references between elements
  m_mesh->RebuildAllInverseConnections();

  // Prepare basic VTK arrays
  vtkPolyData* aPolyOutput = vtkPolyData::GetData(pOutputVector);
  aPolyOutput->Allocate();
  //
  vtkPointData* aPD = aPolyOutput->GetPointData();
  //
  vtkSmartPointer<vtkPoints> aPoints = vtkSmartPointer<vtkPoints>::New();
  aPolyOutput->SetPoints(aPoints);

  // Array for mesh node IDs
  vtkSmartPointer<vtkIntArray>
    aNodeIDsArr = asiVisu_Utils::InitIntArray(ARRNAME_MESH_NODE_IDS);
  aPD->AddArray(aNodeIDsArr);

  /* =======================
   *  Populate VTK data set
   * ======================= */

  NCollection_DataMap<int, vtkIdType> nodeIds;

  // Register VTK geometry and build vertices for the base nodes
  for ( Mesh_ElementsIterator nit(m_mesh, Mesh_ET_Node); nit.More(); nit.Next() )
  {
    // Access next node
    const Handle(Mesh_Node)& N = Handle(Mesh_Node)::DownCast( nit.GetValue() );

    // Add geometric primitive
    const int     N_id = N->GetID();
    const gp_Pnt& P    = N->Pnt();
    //
    const vtkIdType point_id = this->registerNode(N_id, P, aPolyOutput);
    nodeIds.Bind(N_id, point_id);

    // Add topological vertex for bases
    if ( m_nodes.Contains(N_id) )
      this->registerVertex(point_id, aPolyOutput);
  }

  NCollection_IndexedMap<t_link, t_link::Hasher> links;

  // Loop over the base nodes
  for ( TColStd_MapIteratorOfPackedMapOfInteger mit(m_nodes); mit.More(); mit.Next() )
  {
    // Get next base node
    const int              N_id = mit.Key();
    Handle(Mesh_Node) N    = m_mesh->FindNode(N_id);

    // Loop for neighbors of the base node. Create links for all neighbor bases
    const Mesh_ListOfElements& owners = N->InverseElements();
    //
    for ( Mesh_ListOfElements::Iterator lit(owners); lit.More(); lit.Next() )
    {
      const Handle(Mesh_Element)& owner = lit.Value();
      if ( owner.IsNull() )
        continue;

      int* pNodeIDs = (int*) owner->GetConnections();
      for ( int k = 0; k < owner->NbNodes(); ++k )
      {
        const int NN_id = pNodeIDs[k];
        t_link link(N_id, NN_id);

        if ( !links.Contains(link) )
        {
          links.Add(link);
          this->registerLink(nodeIds(N_id), nodeIds(NN_id), aPolyOutput);
        }
      }
    }
  }

  return Superclass::RequestData(pRequest, pInputVector, pOutputVector);
}

//! Adds a mesh node as a point to the passed polygonal data.
//! \param nodeID    [in]     node ID.
//! \param coords    [in]     Cartesian coordinates of the node.
//! \param pPolyData [in/out] output polygonal data.
//! \return internal VTK ID for the newly added point.
vtkIdType asiVisu_MeshTopoSource::registerNode(const int     nodeId,
                                              const gp_Pnt& coords,
                                              vtkPolyData*  pPolyData)
{
  // Access necessary arrays
  vtkPoints* aPoints = pPolyData->GetPoints();
  vtkIntArray* aNodeIDsArr =
    vtkIntArray::SafeDownCast( pPolyData->GetPointData()->GetArray(ARRNAME_MESH_NODE_IDS) );

  // Push the point into VTK data set
  vtkIdType aResPid = aPoints->InsertNextPoint( coords.X(), coords.Y(), coords.Z() );
  aNodeIDsArr->InsertNextValue(nodeId);

  // Return ID
  return aResPid;
}

//! Adds vertex cell.
//! \param n         [in] ID of the point.
//! \param pPolyData [in] polygonal data being populated.
//! \return ID of the just added VTK cell.
vtkIdType asiVisu_MeshTopoSource::registerVertex(vtkIdType    n,
                                                vtkPolyData* pPolyData)
{
  std::vector<vtkIdType> pids = {n};
  //
  vtkIdType
    cellID = pPolyData->InsertNextCell( VTK_VERTEX, (int) pids.size(), &pids[0] );

  return cellID;
}

//! Adds link cell.
//! \param n1        [in] ID of the first node.
//! \param n2        [in] ID of the second node.
//! \param pPolyData [in] polygonal data being populated.
//! \return ID of the just added VTK cell.
vtkIdType asiVisu_MeshTopoSource::registerLink(vtkIdType    n1,
                                              vtkIdType    n2,
                                              vtkPolyData* pPolyData)
{
  std::vector<vtkIdType> pids = {n1, n2};
  //
  vtkIdType
    cellID = pPolyData->InsertNextCell( VTK_LINE, (int) pids.size(), &pids[0] );

  return cellID;
}
