//-----------------------------------------------------------------------------
// Created on: 26 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <common_model.h>

// A-Situs (common) includes
#include <common_facilities.h>

// A-Situs (engine) includes
#include <engine_ubend.h>

// Active Data includes
#include <ActData_CAFConverter.h>
#include <ActData_RealEvaluatorFunc.h>
#include <ActData_RealVarNode.h>
#include <ActData_RealVarPartition.h>
#include <ActData_UniqueNodeName.h>
#include <ActData_Utils.h>

//-----------------------------------------------------------------------------
// Register Node types
//-----------------------------------------------------------------------------

// Shipped with Active Data
REGISTER_NODE_TYPE(ActData_RealVarNode)

// Custom Nodes
REGISTER_NODE_TYPE(common_root_node)
REGISTER_NODE_TYPE(calculus_design_law_node)
REGISTER_NODE_TYPE(mesh_node)
REGISTER_NODE_TYPE(geom_part_node)
REGISTER_NODE_TYPE(geom_face_node)
REGISTER_NODE_TYPE(geom_surf_node)
REGISTER_NODE_TYPE(geom_sections_node)
REGISTER_NODE_TYPE(geom_section_node)
REGISTER_NODE_TYPE(geom_ubend_node)
REGISTER_NODE_TYPE(geom_ubend_law_node)
REGISTER_NODE_TYPE(geom_ubend_laws_node)

//-----------------------------------------------------------------------------

static void PrepareForRemoval(const Handle(ActAPI_INode)&     root_n,
                              const Handle(ActAPI_HNodeList)& nodesToDelete)
{
  if ( root_n.IsNull() || !root_n->IsWellFormed() )
    return;

  // Loop over direct children of a given Node
  for ( Handle(ActAPI_IChildIterator) cit = root_n->GetChildIterator(); cit->More(); cit->Next() )
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
}

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
  //---------------------------------------------------------------------------
  // Add root Node
  //---------------------------------------------------------------------------

  Handle(common_root_node)
    root_n = Handle(common_root_node)::DownCast( common_root_node::Instance() );
  //
  this->RootPartition()->AddNode(root_n);

  // Set name
  root_n->SetName("Analysis Situs");

  //---------------------------------------------------------------------------
  // Add Mesh Node
  //---------------------------------------------------------------------------

  // Add Mesh Node to Partition
  Handle(mesh_node) mesh_n = Handle(mesh_node)::DownCast( mesh_node::Instance() );
  this->MeshPartition()->AddNode(mesh_n);

  // Initialize mesh
  mesh_n->Init();
  mesh_n->SetName("Tessellation");

  // Set as a child for root
  root_n->AddChildNode(mesh_n);

  //---------------------------------------------------------------------------
  // Add Part Node
  //---------------------------------------------------------------------------

  // Add Part Node to Partition
  Handle(geom_part_node) geom_n = Handle(geom_part_node)::DownCast( geom_part_node::Instance() );
  this->PartPartition()->AddNode(geom_n);

  // Initialize geometry
  geom_n->Init();
  geom_n->SetName("Part");

  // Create underlying face representation Node
  {
    Handle(ActAPI_INode) geom_face_base = geom_face_node::Instance();
    this->GeomFacePartition()->AddNode(geom_face_base);

    // Initialize
    Handle(geom_face_node) geom_face_n = Handle(geom_face_node)::DownCast(geom_face_base);
    geom_face_n->Init();
    geom_face_n->SetName("Face domain");

    // Set as child
    geom_n->AddChildNode(geom_face_n);
  }

  // Create underlying surface representation Node
  {
    Handle(ActAPI_INode) geom_surf_base = geom_surf_node::Instance();
    this->GeomSurfacePartition()->AddNode(geom_surf_base);

    // Initialize
    Handle(geom_surf_node) geom_surf_n = Handle(geom_surf_node)::DownCast(geom_surf_base);
    geom_surf_n->Init();
    geom_surf_n->SetName("Host surface");

    // Set as child
    geom_n->AddChildNode(geom_surf_n);
  }

  // Set as a child for root
  root_n->AddChildNode(geom_n);

  //---------------------------------------------------------------------------
  // Add Sections Node
  //---------------------------------------------------------------------------

  Handle(geom_sections_node)
    sections_n = Handle(geom_sections_node)::DownCast( geom_sections_node::Instance() );
  //
  this->SectionsPartition()->AddNode(sections_n);

  // Initialize
  sections_n->Init();
  sections_n->SetName("Skinning Sections");

  // Add as a child for the root
  root_n->AddChildNode(sections_n);
}

//! Clears the Model.
void common_model::Clear()
{
  Handle(ActAPI_HNodeList) nodesToDelete = new ActAPI_HNodeList;

  //---------------------------------------------------------------------------
  // Collects Nodes to delete
  //---------------------------------------------------------------------------

  // NOTE: Part Node is not touched as it is structural. No sense in
  //       removing it since we will have to create it again once a new
  //       part is loaded. The same rule applies to other structural Nodes.

  ::PrepareForRemoval(this->SectionsNode(), nodesToDelete);

  //---------------------------------------------------------------------------
  // Perform deletion
  //---------------------------------------------------------------------------

  this->OpenCommand(); // tx start
  {
    // Delete all Nodes queued for removal
    for ( ActAPI_NodeList::Iterator nit( *nodesToDelete.operator->() ); nit.More(); nit.Next() )
      this->DeleteNode( nit.Value()->GetId() );
  }
  this->CommitCommand(); // tx end
}

//-----------------------------------------------------------------------------

//! \return single Mesh Node.
Handle(mesh_node) common_model::MeshNode() const
{
  for ( ActData_BasePartition::Iterator it( this->MeshPartition() ); it.More(); it.Next() )
  {
    Handle(mesh_node) N = Handle(mesh_node)::DownCast( it.Value() );
    if ( !N.IsNull() && N->IsWellFormed() )
      return N;
  }
  return NULL;
}

//! \return single Geometry Part Node.
Handle(geom_part_node) common_model::PartNode() const
{
  for ( ActData_BasePartition::Iterator it( this->PartPartition() ); it.More(); it.Next() )
  {
    Handle(geom_part_node) N = Handle(geom_part_node)::DownCast( it.Value() );
    if ( !N.IsNull() && N->IsWellFormed() )
      return N;
  }
  return NULL;
}

//! \return single Sections Node.
Handle(geom_sections_node) common_model::SectionsNode() const
{
  for ( ActData_BasePartition::Iterator it( this->SectionsPartition() ); it.More(); it.Next() )
  {
    Handle(geom_sections_node) N = Handle(geom_sections_node)::DownCast( it.Value() );
    if ( !N.IsNull() && N->IsWellFormed() )
      return N;
  }
  return NULL;
}

//! \return single U-bend Node.
Handle(geom_ubend_node) common_model::UBendNode() const
{
  for ( ActData_BasePartition::Iterator it( this->UBendPartition() ); it.More(); it.Next() )
  {
    Handle(geom_ubend_node) N = Handle(geom_ubend_node)::DownCast( it.Value() );
    if ( !N.IsNull() && N->IsWellFormed() )
      return N;
  }
  return NULL;
}

//-----------------------------------------------------------------------------

//! Initializes Partitions.
void common_model::initPartitions()
{
  REGISTER_PARTITION(common_partition<common_root_node>,         Partition_Root);
  REGISTER_PARTITION(common_partition<calculus_design_law_node>, Partition_CalculusDesignLaw);
  REGISTER_PARTITION(common_partition<mesh_node>,                Partition_Mesh);
  REGISTER_PARTITION(common_partition<geom_part_node>,           Partition_GeomPart);
  REGISTER_PARTITION(common_partition<geom_face_node>,           Partition_GeomFace);
  REGISTER_PARTITION(common_partition<geom_surf_node>,           Partition_GeomSurface);
  REGISTER_PARTITION(common_partition<geom_sections_node>,       Partition_Sections);
  REGISTER_PARTITION(common_partition<geom_section_node>,        Partition_Section);
  REGISTER_PARTITION(common_partition<geom_ubend_node>,          Partition_UBend);
  REGISTER_PARTITION(common_partition<geom_ubend_laws_node>,     Partition_UBendLaws);
  REGISTER_PARTITION(common_partition<geom_ubend_law_node>,      Partition_UBendLaw);
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
