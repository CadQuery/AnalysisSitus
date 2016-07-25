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
#include <engine_iv.h>
#include <engine_part.h>
#include <engine_re.h>
#include <engine_ubend.h>
#include <engine_volume.h>

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
//
REGISTER_NODE_TYPE(tess_node)
//
REGISTER_NODE_TYPE(geom_part_node)
REGISTER_NODE_TYPE(geom_face_node)
REGISTER_NODE_TYPE(geom_surf_node)
REGISTER_NODE_TYPE(geom_boundary_edges_node)
REGISTER_NODE_TYPE(geom_volume_node)
//
REGISTER_NODE_TYPE(geom_re_node)
REGISTER_NODE_TYPE(geom_re_surfaces_node)
REGISTER_NODE_TYPE(geom_re_surface_node)
REGISTER_NODE_TYPE(geom_re_contours_node)
REGISTER_NODE_TYPE(geom_re_contour_node)
REGISTER_NODE_TYPE(geom_re_points_node)
//
REGISTER_NODE_TYPE(geom_sections_node)
REGISTER_NODE_TYPE(geom_section_node)
//
REGISTER_NODE_TYPE(calculus_design_law_node)
REGISTER_NODE_TYPE(geom_ubend_node)
REGISTER_NODE_TYPE(geom_ubend_law_node)
REGISTER_NODE_TYPE(geom_ubend_laws_node)
//
REGISTER_NODE_TYPE(visu_iv_curve_node)
REGISTER_NODE_TYPE(visu_iv_curves_node)
REGISTER_NODE_TYPE(visu_iv_node)
REGISTER_NODE_TYPE(visu_iv_points_2d_node)
REGISTER_NODE_TYPE(visu_iv_points_node)
REGISTER_NODE_TYPE(visu_iv_point_set_2d_node)
REGISTER_NODE_TYPE(visu_iv_point_set_node)
REGISTER_NODE_TYPE(visu_iv_surface_node)
REGISTER_NODE_TYPE(visu_iv_surfaces_node)
REGISTER_NODE_TYPE(visu_iv_tess_item_node)
REGISTER_NODE_TYPE(visu_iv_tess_node)
REGISTER_NODE_TYPE(visu_iv_topo_item_node)
REGISTER_NODE_TYPE(visu_iv_topo_node)
REGISTER_NODE_TYPE(visu_iv_text_item_node)
REGISTER_NODE_TYPE(visu_iv_text_node)

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
  Handle(tess_node) tess_n = Handle(tess_node)::DownCast( tess_node::Instance() );
  this->MeshPartition()->AddNode(tess_n);

  // Initialize mesh
  tess_n->Init();
  tess_n->SetName("Tessellation");

  // Set as a child for root
  root_n->AddChildNode(tess_n);

  //---------------------------------------------------------------------------
  // Add Part Node
  //---------------------------------------------------------------------------

  root_n->AddChildNode( engine_part::Create_Part() );

  //---------------------------------------------------------------------------
  // Add Reverse Engineering Node
  //---------------------------------------------------------------------------

  root_n->AddChildNode( engine_re::Create_RE() );

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

  //---------------------------------------------------------------------------
  // Add Imperative Viewer Node
  //---------------------------------------------------------------------------

  root_n->AddChildNode( engine_iv::Create_IV() );
}

//! Clears the Model.
void common_model::Clear()
{
  // Clean up Presentations
  common_facilities::Instance()->Prs.DeleteAll();

  //---------------------------------------------------------------------------
  // Collects Nodes to delete
  //---------------------------------------------------------------------------

  Handle(ActAPI_HNodeList) nodesToDelete = new ActAPI_HNodeList;

  // NOTE: Part Node is not touched as it is structural. No sense in
  //       removing it since we will have to create it again once a new
  //       part is loaded. The same rule applies to other structural Nodes.

  ::PrepareForRemoval(this->SectionsNode(),           nodesToDelete);
  //
  ::PrepareForRemoval(this->RENode()->Surfaces(),     nodesToDelete);
  ::PrepareForRemoval(this->RENode()->Contours(),     nodesToDelete);
  ::PrepareForRemoval(this->RENode()->Points(),       nodesToDelete);
  //
  ::PrepareForRemoval(this->IVNode()->Points2d(),     nodesToDelete);
  ::PrepareForRemoval(this->IVNode()->Points(),       nodesToDelete);
  ::PrepareForRemoval(this->IVNode()->Curves(),       nodesToDelete);
  ::PrepareForRemoval(this->IVNode()->Surfaces(),     nodesToDelete);
  ::PrepareForRemoval(this->IVNode()->Topology(),     nodesToDelete);
  ::PrepareForRemoval(this->IVNode()->Tessellation(), nodesToDelete);
  ::PrepareForRemoval(this->IVNode()->Text(),         nodesToDelete);

  //---------------------------------------------------------------------------
  // Perform deletion
  //---------------------------------------------------------------------------

  this->OpenCommand(); // tx start
  {
    // Clean up persistent selection
    this->PartNode()->FaceRepresentation()    ->SetSelectedFace(0);
    this->PartNode()->SurfaceRepresentation() ->SetSelectedFace(0);

    // Delete all Nodes queued for removal
    for ( ActAPI_NodeList::Iterator nit( *nodesToDelete.operator->() ); nit.More(); nit.Next() )
      this->DeleteNode( nit.Value()->GetId() );

    // Delete volumes
    engine_volume::Clean_Volumes();
  }
  this->CommitCommand(); // tx end
}

//-----------------------------------------------------------------------------

//! \return single Mesh Node.
Handle(tess_node) common_model::Mesh_Node() const
{
  for ( ActData_BasePartition::Iterator it( this->MeshPartition() ); it.More(); it.Next() )
  {
    Handle(tess_node) N = Handle(tess_node)::DownCast( it.Value() );
    //
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
    //
    if ( !N.IsNull() && N->IsWellFormed() )
      return N;
  }
  return NULL;
}

//! \return single Reverse Engineering Node.
Handle(geom_re_node) common_model::RENode() const
{
  for ( ActData_BasePartition::Iterator it( this->REPartition() ); it.More(); it.Next() )
  {
    Handle(geom_re_node) N = Handle(geom_re_node)::DownCast( it.Value() );
    //
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
    //
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
    //
    if ( !N.IsNull() && N->IsWellFormed() )
      return N;
  }
  return NULL;
}

//! \return single Imperative Viewer Node.
Handle(visu_iv_node) common_model::IVNode() const
{
  for ( ActData_BasePartition::Iterator it( this->IVPartition() ); it.More(); it.Next() )
  {
    Handle(visu_iv_node) N = Handle(visu_iv_node)::DownCast( it.Value() );
    //
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
  //
  REGISTER_PARTITION(common_partition<tess_node>,                Partition_Mesh);
  //
  REGISTER_PARTITION(common_partition<geom_part_node>,           Partition_GeomPart);
  REGISTER_PARTITION(common_partition<geom_face_node>,           Partition_GeomFace);
  REGISTER_PARTITION(common_partition<geom_surf_node>,           Partition_GeomSurface);
  REGISTER_PARTITION(common_partition<geom_boundary_edges_node>, Partition_GeomBoundaryEdges);
  REGISTER_PARTITION(common_partition<geom_volume_node>,         Partition_GeomVolume);
  //
  REGISTER_PARTITION(common_partition<geom_re_node>,             Partition_RE);
  REGISTER_PARTITION(common_partition<geom_re_surfaces_node>,    Partition_RESurfaces);
  REGISTER_PARTITION(common_partition<geom_re_surface_node>,     Partition_RESurface);
  REGISTER_PARTITION(common_partition<geom_re_contours_node>,    Partition_REContours);
  REGISTER_PARTITION(common_partition<geom_re_contour_node>,     Partition_REContour);
  REGISTER_PARTITION(common_partition<geom_re_points_node>,      Partition_REPoints);
  //
  REGISTER_PARTITION(common_partition<geom_sections_node>,       Partition_Sections);
  REGISTER_PARTITION(common_partition<geom_section_node>,        Partition_Section);
  //
  REGISTER_PARTITION(common_partition<calculus_design_law_node>, Partition_CalculusDesignLaw);
  REGISTER_PARTITION(common_partition<geom_ubend_node>,          Partition_UBend);
  REGISTER_PARTITION(common_partition<geom_ubend_laws_node>,     Partition_UBendLaws);
  REGISTER_PARTITION(common_partition<geom_ubend_law_node>,      Partition_UBendLaw);
  //
  REGISTER_PARTITION(common_partition<visu_iv_node>,              Partition_IV);
  REGISTER_PARTITION(common_partition<visu_iv_points_2d_node>,    Partition_IV_Points2d);
  REGISTER_PARTITION(common_partition<visu_iv_points_node>,       Partition_IV_Points);
  REGISTER_PARTITION(common_partition<visu_iv_point_set_2d_node>, Partition_IV_PointSet2d);
  REGISTER_PARTITION(common_partition<visu_iv_point_set_node>,    Partition_IV_PointSet);
  REGISTER_PARTITION(common_partition<visu_iv_curves_node>,       Partition_IV_Curves);
  REGISTER_PARTITION(common_partition<visu_iv_curve_node>,        Partition_IV_Curve);
  REGISTER_PARTITION(common_partition<visu_iv_surfaces_node>,     Partition_IV_Surfaces);
  REGISTER_PARTITION(common_partition<visu_iv_surface_node>,      Partition_IV_Surface);
  REGISTER_PARTITION(common_partition<visu_iv_topo_node>,         Partition_IV_Topo);
  REGISTER_PARTITION(common_partition<visu_iv_topo_item_node>,    Partition_IV_TopoItem);
  REGISTER_PARTITION(common_partition<visu_iv_tess_node>,         Partition_IV_Tess);
  REGISTER_PARTITION(common_partition<visu_iv_tess_item_node>,    Partition_IV_TessItem);
  REGISTER_PARTITION(common_partition<visu_iv_text_node>,         Partition_IV_Text);
  REGISTER_PARTITION(common_partition<visu_iv_text_item_node>,    Partition_IV_TextItem);
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
