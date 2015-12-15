//-----------------------------------------------------------------------------
// Created on: 20 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

// Own include
#include <visu_mesh_source.h>

// Visualization includes
#include <visu_mesh_utils.h>
#include <visu_utils.h>

// VTK includes
#include <vtkCellData.h>
#include <vtkDataObject.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

// OMFDS includes
#pragma warning(push, 0)
#include <OMFDS_MapIteratorOfExtendedMap.hxx>
#include <OMFDS_MeshElementsIterator.hxx>
#include <OMFDS_MeshNode.hxx>
#include <OMFDS_MeshQuadrangle.hxx>
#include <OMFDS_MeshTriangle.hxx>
#pragma warning(pop)

//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------

vtkStandardNewMacro(visu_mesh_source);

//! Default constructor.
visu_mesh_source::visu_mesh_source()
{
  this->EmptyGroupForAllModeOn();
  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline
}

//! Destructor.
visu_mesh_source::~visu_mesh_source()
{
}

//-----------------------------------------------------------------------------
// Kernel methods
//-----------------------------------------------------------------------------

//! Switches ON inverse mode of filtering.
void visu_mesh_source::EmptyGroupForAllModeOn()
{
  m_bIsEmptyGroupForAll = true;
}

//! Switches OFF inverse mode of filtering.
void visu_mesh_source::EmptyGroupForAllModeOff()
{
  m_bIsEmptyGroupForAll = false;
}

//! Sets the instance of Mesh DS being used as an input for Data Source.
//! \param theMesh [in] input Mesh DS.
void visu_mesh_source::SetInputMesh(const Handle(OMFDS_Mesh)& theMesh)
{
  m_mesh = theMesh;
}

//! Sets the Mesh Group used to filter the input mesh.
//! \param theGroup [in] input Mesh Group.
void visu_mesh_source::SetInputElemGroup(const Handle(OMFDS_MeshGroup)& theGroup)
{
  m_elemGroup = theGroup;
}

//! Accessor for the input Mesh DS.
//! \return requested Mesh DS.
Handle(OMFDS_Mesh) visu_mesh_source::GetInputMesh() const
{
  return m_mesh;
}

//! Accessor for the input Mesh Group.
//! \return requested Mesh Group.
Handle(OMFDS_MeshGroup) visu_mesh_source::GetInputElemGroup() const
{
  return m_elemGroup;
}

//! This is called by the superclass. Creates VTK polygonal data set
//! from the input Mesh DS.
//! \param theRequest      [in] information about data object.
//! \param theInputVector  [in] the input data. As a data source is the start
//!                             stage of the VTK pipeline, theInputVector is
//!                             empty and not used (no input port).
//! \param theOutputVector [in] the pointer to output data, that is filled
//!                             in this method.
//! \return status.
int visu_mesh_source::RequestData(vtkInformation*        theRequest,
                                  vtkInformationVector** theInputVector,
                                  vtkInformationVector*  theOutputVector)
{
  /* ==============================
   *  Prepare involved collections
   * ============================== */

  vtkPolyData* aPolyOutput = vtkPolyData::GetData(theOutputVector);
  aPolyOutput->Allocate();

  vtkPointData* aPD = aPolyOutput->GetPointData();
  vtkCellData*  aCD = aPolyOutput->GetCellData();

  vtkSmartPointer<vtkPoints> aPoints = vtkSmartPointer<vtkPoints>::New();
  aPolyOutput->SetPoints(aPoints);

  /* ==================================
   *  Initialize necessary data arrays
   * ================================== */

  // Array for mesh item types
  vtkSmartPointer<vtkIntArray>
    aTypeArr = visu_utils::InitIntArray(ARRNAME_MESH_ITEM_TYPE);
  aCD->AddArray(aTypeArr);

  // Array for mesh node IDs
  vtkSmartPointer<vtkIntArray>
    aNodeIDsArr = visu_utils::InitIntArray(ARRNAME_MESH_NODE_IDS);
  aPD->AddArray(aNodeIDsArr);

  // Array for mesh element IDs
  vtkSmartPointer<vtkIntArray>
    aFaceIDsArr = visu_utils::InitIntArray(ARRNAME_MESH_ELEM_IDS);
  aCD->AddArray(aFaceIDsArr);

  /* ==============================================================
   *  Take care of free nodes by collecting them into a dedicated
   *  VTK_POLY_VERTEX cell. Notice that such cell (as well as
   *  simple VTK_VERTEX) must be added FIRST when working with
   *  vtkPolyData objects in order to have the data set consistent
   * ============================================================== */

  // Prepare back references for the elements stored in Mesh DS. Thus we are
  // able to track free nodes
  m_mesh->RebuildAllInverseConnections();

  // Iterate over the entire collection of nodes to cumulate them into
  // a sequence and prepare a single VTK cell for all detected free nodes
  NCollection_Sequence<int> aFreeNodeIDs;
  OMFDS_MeshElementsIterator aMeshNodesIt(m_mesh, OMFAbs_Node);
  for ( ; aMeshNodesIt.More(); aMeshNodesIt.Next() )
  {
    // Access next node
    Handle(OMFDS_MeshNode) aNode = Handle(OMFDS_MeshNode)::DownCast( aMeshNodesIt.GetValue() );
    const OMFDS_ListOfMeshElement& lstInvElements = aNode->InverseElements();

    // Skip connected nodes as we're looking for free ones here
    if ( lstInvElements.Extent() > 0 )
      continue;

    aFreeNodeIDs.Append( aNode->GetID() );
  }

  this->registerFreeNodesCell(aFreeNodeIDs, aPolyOutput);

  /* =======================================
   *  Pass mesh elements to VTK data source
   * ======================================= */

  if ( m_elemGroup.IsNull() && m_bIsEmptyGroupForAll ) // No filtering by group enabled
  {
    OMFDS_MeshElementsIterator aMeshElemsIt(m_mesh, OMFAbs_Face);
    for ( ; aMeshElemsIt.More(); aMeshElemsIt.Next() )
      this->translateElement(aMeshElemsIt.GetValue(), aPolyOutput);
  }
  else if ( !m_elemGroup.IsNull() )
  {
    const OMFDS_MapOfMeshElement& aGroupElems = m_elemGroup->Elements();
    OMFDS_MapIteratorOfExtendedMap aGroupIt(aGroupElems);
    for ( ; aGroupIt.More(); aGroupIt.Next() )
      this->translateElement(aGroupIt.Key(), aPolyOutput);
  }

  return Superclass::RequestData(theRequest, theInputVector, theOutputVector);
}

//! Translates the passed OMFDS mesh element to VTK polygonal cell.
//! \param theElem     [in]     OMFDS element to translate.
//! \param thePolyData [in/out] output polygonal data.
void visu_mesh_source::translateElement(const Handle(OMFDS_MeshElement)& theElem,
                                        vtkPolyData*                     thePolyData)
{
  // Proceed with TRIANGLE elements
  if ( theElem->IsInstance( STANDARD_TYPE(OMFDS_MeshTriangle) ) )
  {
    // Access element data
    Handle(OMFDS_MeshTriangle) aTriElem = Handle(OMFDS_MeshTriangle)::DownCast(theElem);
    int aTriNodeIds[3], aNbNodes;
    aTriElem->GetFaceDefinedByNodes(3, aTriNodeIds, aNbNodes);

    // Transfer element to VTK polygonal source
    this->registerMeshFace(aTriElem->GetID(), aTriNodeIds, aNbNodes, thePolyData);
  }
  // Proceed with QUADRANGLE elements
  else if ( theElem->IsInstance( STANDARD_TYPE(OMFDS_MeshQuadrangle) ) )
  {
    // Access element data
    Handle(OMFDS_MeshQuadrangle) aQuadElem = Handle(OMFDS_MeshQuadrangle)::DownCast(theElem);
    int aQuadNodeIds[4], aNbNodes;
    aQuadElem->GetFaceDefinedByNodes(4, aQuadNodeIds, aNbNodes);

    // Transfer element to VTK polygonal source
    this->registerMeshFace(aQuadElem->GetID(), aQuadNodeIds, aNbNodes, thePolyData);
  }
}

//! Adds a mesh node as a point to the passed polygonal data.
//! \param theNodeID   [in]     node ID.
//! \param thePolyData [in/out] output polygonal data.
//! \return internal VTK ID for the newly added point.
vtkIdType visu_mesh_source::registerMeshNode(const int    theNodeID,
                                             vtkPolyData* thePolyData)
{
  // Access necessary arrays
  vtkPoints* aPoints = thePolyData->GetPoints();
  vtkIntArray* aNodeIDsArr =
    vtkIntArray::SafeDownCast( thePolyData->GetPointData()->GetArray(ARRNAME_MESH_NODE_IDS) );

  // Access mesh node
  Handle(OMFDS_MeshNode) aNode = m_mesh->FindNode(theNodeID);

  vtkIdType aResPid;
  if ( !m_regPoints.IsBound(theNodeID) )
  {
    // Get nodal coordinates
    gp_Pnt aPnt = aNode->Pnt();

    // Push the point into VTK data set
    aResPid = aPoints->InsertNextPoint( aPnt.X(), aPnt.Y(), aPnt.Z() );
    aNodeIDsArr->InsertNextValue( aNode->GetID() );

    m_regPoints.Bind(theNodeID, aResPid);
  }
  else
    aResPid = m_regPoints.Find(theNodeID);

  return aResPid;
}

//! Adds the mesh element with the given ID to VTK polygonal data as a
//! Triangle or Quadrangle VTK element depending on the actual number
//! of nodes.
//! \param theFaceID   [in] ID of the element.
//! \param theNodes    [in] node IDs.
//! \param theNbNodes  [in] number of nodes.
//! \param thePolyData [in] polygonal data being populated.
//! \return ID of the just added VTK cell.
vtkIdType visu_mesh_source::registerMeshFace(const int    theFaceID,
                                             const void*  theNodes,
                                             const int    theNbNodes,
                                             vtkPolyData* thePolyData)
{
  /* ==============================================
   *  Define the type of the cell being registered
   * ============================================== */

  VTKCellType aCellType = VTK_EMPTY_CELL;
  switch ( theNbNodes )
  {
    case 3: aCellType = VTK_TRIANGLE; break;
    case 4: aCellType = VTK_QUAD; break;
    default: return VTK_BAD_ID;
  }

  /* =========================================
   *  Register cell in the polygonal data set
   * ========================================= */

  int* aNodeIDs = (int*) theNodes;
  std::vector<vtkIdType> aPids;
  for ( int k = 0; k < theNbNodes; k++ )
  {
    vtkIdType aPointID = this->registerMeshNode(aNodeIDs[k], thePolyData);
    aPids.push_back(aPointID);
  }

  vtkIdType aCellID =
    thePolyData->InsertNextCell( aCellType, (int) aPids.size(), &aPids[0] );

  /* ==================================================
   *  Associate the newly added cell with element type
   * ================================================== */

  vtkIntArray* aTypeArr =
    vtkIntArray::SafeDownCast( thePolyData->GetCellData()->GetArray(ARRNAME_MESH_ITEM_TYPE) );
  int aType = (aCellType == VTK_TRIANGLE ? MeshItem_Triangle : MeshItem_Quadrangle);
  aTypeArr->InsertNextValue(aType);

  /* =========================================================
   *  Associate the newly added cell with original element ID
   * ========================================================= */

  vtkIntArray* aFaceIDsArr =
    vtkIntArray::SafeDownCast( thePolyData->GetCellData()->GetArray(ARRNAME_MESH_ELEM_IDS) );
  aFaceIDsArr->InsertNextValue(theFaceID);

  return aCellID;
}

//! Registers the passed mesh nodes as free nodes by pushing them into a
//! dedicated VTK cell of VTK_POLY_VERTEX type.
//! \param theNodeIDs  [in] collection of free node IDs.
//! \param thePolyData [in] polygonal data being populated.
//! \return ID of the registered cell.
vtkIdType
  visu_mesh_source::registerFreeNodesCell(const NCollection_Sequence<int>& theNodeIDs,
                                          vtkPolyData*                     thePolyData)
{
  if ( theNodeIDs.IsEmpty() )
    return VTK_BAD_ID;

  std::vector<vtkIdType> aPids;
  NCollection_Sequence<int>::Iterator anIt(theNodeIDs);
  for ( ; anIt.More(); anIt.Next() )
  {
    int aNodeID = anIt.Value();
    aPids.push_back( this->registerMeshNode(aNodeID, thePolyData) );
  }

  vtkIdType aCellID =
    thePolyData->InsertNextCell( VTK_POLY_VERTEX, (int) aPids.size(), &aPids[0] );

  vtkIntArray* aTypeArr =
    vtkIntArray::SafeDownCast( thePolyData->GetCellData()->GetArray(ARRNAME_MESH_ITEM_TYPE) );
  aTypeArr->InsertNextValue(MeshItem_FreeNode);

  return aCellID;
}
