//-----------------------------------------------------------------------------
// Created on: 26 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <common_model.h>

// A-Situs (common) includes
#include <common_facilities.h>

// Active Data includes
#include <ActData_CAFConverter.h>
#include <ActData_Utils.h>
#include <ActData_RealEvaluatorFunc.h>
#include <ActData_RealVarNode.h>
#include <ActData_RealVarPartition.h>

//-----------------------------------------------------------------------------
// Register Node types
//-----------------------------------------------------------------------------

// Shipped with Active Data
REGISTER_NODE_TYPE(ActData_RealVarNode)

// Custom Nodes
REGISTER_NODE_TYPE(common_root_node)
REGISTER_NODE_TYPE(geom_node)
REGISTER_NODE_TYPE(geom_face_node)
REGISTER_NODE_TYPE(geom_surf_node)
REGISTER_NODE_TYPE(mesh_node)

//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------

//! Default constructor. Initializes Base Model foundation object so that
//! to enable Extended Transaction Mode.
common_model::common_model() : ActData_BaseModel(true)
{
  common_facilities::Instance()->Model = this;
}

//-----------------------------------------------------------------------------

//! Populates Data Model.
void common_model::Populate()
{
  /* ===============================================
   *  Add Project Node (the root one) to Data Model
   * =============================================== */

  Handle(common_root_node)
    root_n = Handle(common_root_node)::DownCast( common_root_node::Instance() );

  this->RootPartition()->AddNode(root_n);

  // Set name
  root_n->SetName("Part");
}

//! Clears the Model.
void common_model::Clear()
{
  Handle(ActAPI_HNodeList) nodesToDelete = new ActAPI_HNodeList();

  // Loop over direct children of a root (the root one itself is not deleted)
  for ( Handle(ActAPI_IChildIterator) cit = this->GetRootNode()->GetChildIterator(); cit->More(); cit->Next() )
  {
    Handle(ActAPI_INode) child_n = cit->Value();

    // Check if the given Node is consistent
    if ( child_n.IsNull() || !child_n->IsWellFormed() )
      continue;

    // Clean up Presentations
    common_facilities::Instance()->Prs.DeRenderAll();

    // Set Node for deletion
    nodesToDelete->Append(child_n);
  }

  this->OpenCommand(); // tx start
  {
    // Delete all Nodes queued for removal
    for ( ActAPI_NodeList::Iterator nit( *nodesToDelete.operator->() ); nit.More(); nit.Next() )
      this->DeleteNode( nit.Value()->GetId() );
  }
  this->CommitCommand(); // tx end
}

//! \return single Geometry Node.
Handle(geom_node) common_model::GeometryNode() const
{
  for ( ActData_BasePartition::Iterator it( this->GeomPartition() ); it.More(); it.Next() )
  {
    Handle(geom_node) N = Handle(geom_node)::DownCast( it.Value() );
    if ( !N.IsNull() && N->IsWellFormed() )
      return N;
  }
  return NULL;
}

//-----------------------------------------------------------------------------

//! Initializes Partitions.
void common_model::initPartitions()
{
  REGISTER_PARTITION(common_partition<common_root_node>, Partition_Root);
  REGISTER_PARTITION(common_partition<geom_node>,        Partition_Geom);
  REGISTER_PARTITION(common_partition<geom_face_node>,   Partition_GeomFace);
  REGISTER_PARTITION(common_partition<geom_surf_node>,   Partition_GeomSurface);
  REGISTER_PARTITION(common_partition<mesh_node>,        Partition_Mesh);
}

//! Initializes the Tree Functions bound to the Data Model.
void common_model::initFunctionDrivers()
{
  REGISTER_TREE_FUNCTION(ActData_RealEvaluatorFunc);
}

//-----------------------------------------------------------------------------
// Structure management methods
//-----------------------------------------------------------------------------

//! Returns a Partition of Data Nodes representing Variables for Expression
//! Evaluation mechanism.
//! \param theVarType [in] type of Variable to return the dedicated
//!                        Partition for.
//! \return Variable Partition.
Handle(ActAPI_IPartition)
  common_model::getVariablePartition(const VariableType& theVarType) const
{
  switch ( theVarType )
  {
    case Variable_Real:
      return this->Partition(Partition_RealVar);
    default:
      break;
  }
  return NULL;
}

//! Accessor for the root Project Node.
//! \return root Project Node.
Handle(ActAPI_INode) common_model::getRootNode() const
{
  common_partition<common_root_node>::Iterator anIt( this->RootPartition() );
  return ( anIt.More() ? anIt.Value() : NULL );
}

//! Populates the passed collections of references to pass out-scope filtering
//! in Copy/Paste operation.
//! \param FuncGUIDs [in/out] Function GUIDs to pass.
//! \param Refs      [in/out] Reference Parameters to pass.
void common_model::invariantCopyRefs(ActAPI_FuncGUIDStream&         ASitus_NotUsed(FuncGUIDs),
                                     ActAPI_ParameterLocatorStream& ASitus_NotUsed(Refs)) const
{
}

//-----------------------------------------------------------------------------
// Versions
//-----------------------------------------------------------------------------

//! Returns version of Data Model.
//! \return current version of Data Model.
int common_model::actualVersionApp()
{
  return ASitus_Version_HEX;
}

//! Callback supplying CAF converter required to perform application-specific
//! conversion of Data Model from older version of the application to the
//! recent one.
//! \return properly initialized CAF converter.
Handle(ActData_CAFConverter) common_model::converterApp()
{
  return NULL;
}
