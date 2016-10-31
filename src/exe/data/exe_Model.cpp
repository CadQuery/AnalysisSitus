//-----------------------------------------------------------------------------
// Created on: 26 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiData_Model.h>

// A-Situs (common) includes
#include <common_facilities.h>

// A-Situs (engine) includes
#include <asiEngine_IV.h>
#include <asiEngine_Part.h>
#include <asiEngine_RE.h>
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
REGISTER_NODE_TYPE(asiData_RootNode)
//
REGISTER_NODE_TYPE(asiData_TessNode)
//
REGISTER_NODE_TYPE(asiData_PartNode)
REGISTER_NODE_TYPE(asiData_FaceNode)
REGISTER_NODE_TYPE(asiData_SurfNode)
REGISTER_NODE_TYPE(asiData_EdgeNode)
REGISTER_NODE_TYPE(asiData_CurveNode)
REGISTER_NODE_TYPE(asiData_BoundaryEdgesNode)
REGISTER_NODE_TYPE(geom_volume_node)
REGISTER_NODE_TYPE(asiData_ContourNode)
//
REGISTER_NODE_TYPE(asiData_RENode)
REGISTER_NODE_TYPE(asiData_RESurfacesNode)
REGISTER_NODE_TYPE(asiData_RESurfaceNode)
REGISTER_NODE_TYPE(asiData_REContoursNode)
REGISTER_NODE_TYPE(asiData_REContourNode)
REGISTER_NODE_TYPE(asiData_REPointsNode)
//
REGISTER_NODE_TYPE(geom_sections_node)
REGISTER_NODE_TYPE(geom_section_node)
//
REGISTER_NODE_TYPE(asiData_DesignLawNode)
REGISTER_NODE_TYPE(geom_ubend_node)
REGISTER_NODE_TYPE(geom_ubend_law_node)
REGISTER_NODE_TYPE(geom_ubend_laws_node)
//
REGISTER_NODE_TYPE(asiData_IVCurveNode)
REGISTER_NODE_TYPE(asiData_IVCurvesNode)
REGISTER_NODE_TYPE(asiData_IVNode)
REGISTER_NODE_TYPE(asiData_IVPoints2dNode)
REGISTER_NODE_TYPE(asiData_IVPointsNode)
REGISTER_NODE_TYPE(asiData_IVPointSet2dNode)
REGISTER_NODE_TYPE(asiData_IVPointSetNode)
REGISTER_NODE_TYPE(asiData_IVSurfaceNode)
REGISTER_NODE_TYPE(asiData_IVSurfacesNode)
REGISTER_NODE_TYPE(asiData_IVTessItemNode)
REGISTER_NODE_TYPE(asiData_IVTessNode)
REGISTER_NODE_TYPE(asiData_IVTopoItemNode)
REGISTER_NODE_TYPE(asiData_IVTopoNode)
REGISTER_NODE_TYPE(asiData_IVTextItemNode)
REGISTER_NODE_TYPE(asiData_IVTextNode)

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
asiData_Model::asiData_Model() : ActData_BaseModel(true)
{
  common_facilities::Instance()->Model = this;
}

//-----------------------------------------------------------------------------

//! Populates Data Model.
void asiData_Model::Populate()
{
  //---------------------------------------------------------------------------
  // Add root Node
  //---------------------------------------------------------------------------

  Handle(asiData_RootNode)
    root_n = Handle(asiData_RootNode)::DownCast( asiData_RootNode::Instance() );
  //
  this->GetRootPartition()->AddNode(root_n);

  // Set name
  root_n->SetName("Analysis Situs");

  //---------------------------------------------------------------------------
  // Add Mesh Node
  //---------------------------------------------------------------------------

  // Add Mesh Node to Partition
  Handle(asiData_TessNode) tess_n = Handle(asiData_TessNode)::DownCast( asiData_TessNode::Instance() );
  this->GetMeshPartition()->AddNode(tess_n);

  // Initialize mesh
  tess_n->Init();
  tess_n->SetName("Tessellation");

  // Set as a child for root
  root_n->AddChildNode(tess_n);

  //---------------------------------------------------------------------------
  // Add Part Node
  //---------------------------------------------------------------------------

  root_n->AddChildNode( asiEngine_Part::Create_Part() );

  //---------------------------------------------------------------------------
  // Add Reverse Engineering Node
  //---------------------------------------------------------------------------

  root_n->AddChildNode( asiEngine_RE::Create_RE() );

  //---------------------------------------------------------------------------
  // Add Sections Node
  //---------------------------------------------------------------------------

  Handle(geom_sections_node)
    sections_n = Handle(geom_sections_node)::DownCast( geom_sections_node::Instance() );
  //
  this->GetSectionsPartition()->AddNode(sections_n);

  // Initialize
  sections_n->Init();
  sections_n->SetName("Skinning Sections");

  // Add as a child for the root
  root_n->AddChildNode(sections_n);

  //---------------------------------------------------------------------------
  // Add Imperative Viewer Node
  //---------------------------------------------------------------------------

  root_n->AddChildNode( asiEngine_IV::Create_IV() );
}

//! Clears the Model.
void asiData_Model::Clear()
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

  ::PrepareForRemoval(this->GetSectionsNode(),           nodesToDelete);
  //
  ::PrepareForRemoval(this->GetRENode()->Surfaces(),     nodesToDelete);
  ::PrepareForRemoval(this->GetRENode()->Contours(),     nodesToDelete);
  ::PrepareForRemoval(this->GetRENode()->Points(),       nodesToDelete);
  //
  ::PrepareForRemoval(this->GetIVNode()->Points2d(),     nodesToDelete);
  ::PrepareForRemoval(this->GetIVNode()->Points(),       nodesToDelete);
  ::PrepareForRemoval(this->GetIVNode()->Curves(),       nodesToDelete);
  ::PrepareForRemoval(this->GetIVNode()->Surfaces(),     nodesToDelete);
  ::PrepareForRemoval(this->GetIVNode()->Topology(),     nodesToDelete);
  ::PrepareForRemoval(this->GetIVNode()->Tessellation(), nodesToDelete);
  ::PrepareForRemoval(this->GetIVNode()->Text(),         nodesToDelete);

  //---------------------------------------------------------------------------
  // Perform deletion
  //---------------------------------------------------------------------------

  this->OpenCommand(); // tx start
  {
    // Clean up persistent selection
    this->GetPartNode()->GetFaceRepresentation()    ->SetSelectedFace(0);
    this->GetPartNode()->GetSurfaceRepresentation() ->SetSelectedFace(0);
    this->GetPartNode()->GetEdgeRepresentation()    ->SetSelectedEdge(0);
    this->GetPartNode()->GetCurveRepresentation()   ->SetSelectedEdge(0);

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
Handle(asiData_TessNode) asiData_Model::GetMeshNode() const
{
  for ( ActData_BasePartition::Iterator it( this->GetMeshPartition() ); it.More(); it.Next() )
  {
    Handle(asiData_TessNode) N = Handle(asiData_TessNode)::DownCast( it.Value() );
    //
    if ( !N.IsNull() && N->IsWellFormed() )
      return N;
  }
  return NULL;
}

//! \return single Geometry Part Node.
Handle(asiData_PartNode) asiData_Model::GetPartNode() const
{
  for ( ActData_BasePartition::Iterator it( this->GetPartPartition() ); it.More(); it.Next() )
  {
    Handle(asiData_PartNode) N = Handle(asiData_PartNode)::DownCast( it.Value() );
    //
    if ( !N.IsNull() && N->IsWellFormed() )
      return N;
  }
  return NULL;
}

//! \return single Reverse Engineering Node.
Handle(asiData_RENode) asiData_Model::GetRENode() const
{
  for ( ActData_BasePartition::Iterator it( this->GetREPartition() ); it.More(); it.Next() )
  {
    Handle(asiData_RENode) N = Handle(asiData_RENode)::DownCast( it.Value() );
    //
    if ( !N.IsNull() && N->IsWellFormed() )
      return N;
  }
  return NULL;
}

//! \return single Sections Node.
Handle(geom_sections_node) asiData_Model::GetSectionsNode() const
{
  for ( ActData_BasePartition::Iterator it( this->GetSectionsPartition() ); it.More(); it.Next() )
  {
    Handle(geom_sections_node) N = Handle(geom_sections_node)::DownCast( it.Value() );
    //
    if ( !N.IsNull() && N->IsWellFormed() )
      return N;
  }
  return NULL;
}

//! \return single U-bend Node.
Handle(geom_ubend_node) asiData_Model::GetUBendNode() const
{
  for ( ActData_BasePartition::Iterator it( this->GetUBendPartition() ); it.More(); it.Next() )
  {
    Handle(geom_ubend_node) N = Handle(geom_ubend_node)::DownCast( it.Value() );
    //
    if ( !N.IsNull() && N->IsWellFormed() )
      return N;
  }
  return NULL;
}

//! \return single Imperative Viewer Node.
Handle(asiData_IVNode) asiData_Model::GetIVNode() const
{
  for ( ActData_BasePartition::Iterator it( this->GetIVPartition() ); it.More(); it.Next() )
  {
    Handle(asiData_IVNode) N = Handle(asiData_IVNode)::DownCast( it.Value() );
    //
    if ( !N.IsNull() && N->IsWellFormed() )
      return N;
  }
  return NULL;
}

//-----------------------------------------------------------------------------

//! Initializes Partitions.
void asiData_Model::initPartitions()
{
  REGISTER_PARTITION(asiData_Partition<asiData_RootNode>,         Partition_Root);
  //
  REGISTER_PARTITION(asiData_Partition<asiData_TessNode>,                Partition_Mesh);
  //
  REGISTER_PARTITION(asiData_Partition<asiData_PartNode>,           Partition_GeomPart);
  REGISTER_PARTITION(asiData_Partition<asiData_FaceNode>,           Partition_GeomFace);
  REGISTER_PARTITION(asiData_Partition<asiData_SurfNode>,           Partition_GeomSurface);
  REGISTER_PARTITION(asiData_Partition<asiData_EdgeNode>,           Partition_GeomEdge);
  REGISTER_PARTITION(asiData_Partition<asiData_CurveNode>,          Partition_GeomCurve);
  REGISTER_PARTITION(asiData_Partition<asiData_BoundaryEdgesNode>, Partition_GeomBoundaryEdges);
  REGISTER_PARTITION(asiData_Partition<geom_volume_node>,         Partition_GeomVolume);
  REGISTER_PARTITION(asiData_Partition<asiData_ContourNode>,        Partition_GeomContour);
  //
  REGISTER_PARTITION(asiData_Partition<asiData_RENode>,             Partition_RE);
  REGISTER_PARTITION(asiData_Partition<asiData_RESurfacesNode>,    Partition_RESurfaces);
  REGISTER_PARTITION(asiData_Partition<asiData_RESurfaceNode>,     Partition_RESurface);
  REGISTER_PARTITION(asiData_Partition<asiData_REContoursNode>,    Partition_REContours);
  REGISTER_PARTITION(asiData_Partition<asiData_REContourNode>,     Partition_REContour);
  REGISTER_PARTITION(asiData_Partition<asiData_REPointsNode>,      Partition_REPoints);
  //
  REGISTER_PARTITION(asiData_Partition<geom_sections_node>,       Partition_Sections);
  REGISTER_PARTITION(asiData_Partition<geom_section_node>,        Partition_Section);
  //
  REGISTER_PARTITION(asiData_Partition<asiData_DesignLawNode>, Partition_CalculusDesignLaw);
  REGISTER_PARTITION(asiData_Partition<geom_ubend_node>,          Partition_UBend);
  REGISTER_PARTITION(asiData_Partition<geom_ubend_laws_node>,     Partition_UBendLaws);
  REGISTER_PARTITION(asiData_Partition<geom_ubend_law_node>,      Partition_UBendLaw);
  //
  REGISTER_PARTITION(asiData_Partition<asiData_IVNode>,              Partition_IV);
  REGISTER_PARTITION(asiData_Partition<asiData_IVPoints2dNode>,    Partition_IV_Points2d);
  REGISTER_PARTITION(asiData_Partition<asiData_IVPointsNode>,       Partition_IV_Points);
  REGISTER_PARTITION(asiData_Partition<asiData_IVPointSet2dNode>, Partition_IV_PointSet2d);
  REGISTER_PARTITION(asiData_Partition<asiData_IVPointSetNode>,    Partition_IV_PointSet);
  REGISTER_PARTITION(asiData_Partition<asiData_IVCurvesNode>,       Partition_IV_Curves);
  REGISTER_PARTITION(asiData_Partition<asiData_IVCurveNode>,        Partition_IV_Curve);
  REGISTER_PARTITION(asiData_Partition<asiData_IVSurfacesNode>,     Partition_IV_Surfaces);
  REGISTER_PARTITION(asiData_Partition<asiData_IVSurfaceNode>,      Partition_IV_Surface);
  REGISTER_PARTITION(asiData_Partition<asiData_IVTopoNode>,         Partition_IV_Topo);
  REGISTER_PARTITION(asiData_Partition<asiData_IVTopoItemNode>,    Partition_IV_TopoItem);
  REGISTER_PARTITION(asiData_Partition<asiData_IVTessNode>,         Partition_IV_Tess);
  REGISTER_PARTITION(asiData_Partition<asiData_IVTessItemNode>,    Partition_IV_TessItem);
  REGISTER_PARTITION(asiData_Partition<asiData_IVTextNode>,         Partition_IV_Text);
  REGISTER_PARTITION(asiData_Partition<asiData_IVTextItemNode>,    Partition_IV_TextItem);
}

//! Initializes the Tree Functions bound to the Data Model.
void asiData_Model::initFunctionDrivers()
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
  asiData_Model::getVariablePartition(const VariableType& theVarType) const
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
Handle(ActAPI_INode) asiData_Model::getRootNode() const
{
  asiData_Partition<asiData_RootNode>::Iterator anIt( this->GetRootPartition() );
  return ( anIt.More() ? anIt.Value() : NULL );
}

//! Populates the passed collections of references to pass out-scope filtering
//! in Copy/Paste operation.
//! \param FuncGUIDs [in/out] Function GUIDs to pass.
//! \param Refs      [in/out] Reference Parameters to pass.
void asiData_Model::invariantCopyRefs(ActAPI_FuncGUIDStream&         ASitus_NotUsed(FuncGUIDs),
                                     ActAPI_ParameterLocatorStream& ASitus_NotUsed(Refs)) const
{
}

//-----------------------------------------------------------------------------
// Versions
//-----------------------------------------------------------------------------

//! Returns version of Data Model.
//! \return current version of Data Model.
int asiData_Model::actualVersionApp()
{
  return ASitus_Version_HEX;
}

//! Callback supplying CAF converter required to perform application-specific
//! conversion of Data Model from older version of the application to the
//! recent one.
//! \return properly initialized CAF converter.
Handle(ActData_CAFConverter) asiData_Model::converterApp()
{
  return NULL;
}
