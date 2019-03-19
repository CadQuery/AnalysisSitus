//-----------------------------------------------------------------------------
// Created on: 16 February 2019
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <ottoEngine_Model.h>

// ottoEngine includes
#include <ottoEngine_BuildOBBFunc.h>
#include <ottoEngine_BuildSectionsFunc.h>
#include <ottoEngine_Limb.h>
#include <ottoEngine_LoadMeshFunc.h>
#include <ottoEngine_SkinSurfaceFunc.h>

// asiEngine includes
#include <asiEngine_IV.h>

//-----------------------------------------------------------------------------
// Register Node types
//-----------------------------------------------------------------------------

// Custom Nodes.
REGISTER_NODE_TYPE(ottoData_LimbNode)

//-----------------------------------------------------------------------------

//! Default constructor.
ottoEngine_Model::ottoEngine_Model() : asiEngine_Model()
{}

//-----------------------------------------------------------------------------

//! Populates Data Model.
void ottoEngine_Model::Populate()
{
  asiEngine_Model::Populate();

  // Rename root Node.
  Handle(ActAPI_INode) root_n = this->GetRootNode();
  root_n->SetName("Ottobock workbench");

  // Make all default Nodes invisible.
  for ( Handle(ActAPI_IChildIterator) cit = root_n->GetChildIterator(true); cit->More(); cit->Next() )
  {
    Handle(ActAPI_INode) child_n = cit->Value();
    //
    if ( !child_n.IsNull() && child_n->IsWellFormed() )
      child_n->AddUserFlags(NodeFlag_IsHiddenInBrowser);
  }

  // Add Limb Node.
  root_n->AddChildNode( ottoEngine_Limb(this).CreateLimb() );
}

//-----------------------------------------------------------------------------

//! Clears the Model.
void ottoEngine_Model::clearCustom()
{}

//-----------------------------------------------------------------------------

//! Initializes Partitions.
void ottoEngine_Model::initPartitions()
{
  // Register basic Partitions.
  asiEngine_Model::initPartitions();

  // Register custom Partitions.
  REGISTER_PARTITION(asiData_Partition<ottoData_LimbNode>, Partition_Limb);
}

//-----------------------------------------------------------------------------

//! Registers Tree Functions.
void ottoEngine_Model::initFunctionDrivers()
{
  REGISTER_TREE_FUNCTION(ottoEngine_LoadMeshFunc);
  REGISTER_TREE_FUNCTION(ottoEngine_BuildOBBFunc);
  REGISTER_TREE_FUNCTION(ottoEngine_BuildSectionsFunc);
  REGISTER_TREE_FUNCTION(ottoEngine_SkinSurfaceFunc);
}
