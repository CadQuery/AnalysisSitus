//-----------------------------------------------------------------------------
// Created on: 26 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <exeAsBuilt_Model.h>

// exeAsBuilt includes
#include <exeAsBuilt_RE.h>

//-----------------------------------------------------------------------------
// Register Node types
//-----------------------------------------------------------------------------

REGISTER_NODE_TYPE(asiData_RENode)
REGISTER_NODE_TYPE(asiData_REContourNode)
REGISTER_NODE_TYPE(asiData_REContoursNode)
REGISTER_NODE_TYPE(asiData_REPointsNode)
REGISTER_NODE_TYPE(asiData_RESurfaceNode)
REGISTER_NODE_TYPE(asiData_RESurfacesNode)

//-----------------------------------------------------------------------------

//! Default constructor.
exeAsBuilt_Model::exeAsBuilt_Model() : asiEngine_Model()
{}

//-----------------------------------------------------------------------------

//! Populates Data Model.
void exeAsBuilt_Model::Populate()
{
  // Populate base data
  asiEngine_Model::Populate();

  // Get root Node
  Handle(ActAPI_INode) root_n = this->GetRootNode();

  // Add Reverse Engineering Node
  root_n->AddChildNode( exeAsBuilt_RE(this).Create_RE() );
}

//-----------------------------------------------------------------------------

//! Clears the Model.
void exeAsBuilt_Model::Clear()
{
  // Clean base data
  asiEngine_Model::Clear();
}

//-----------------------------------------------------------------------------

//! \return single Reverse Engineering Node.
Handle(asiData_RENode) exeAsBuilt_Model::GetRENode() const
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

//-----------------------------------------------------------------------------

//! Initializes Partitions.
void exeAsBuilt_Model::initPartitions()
{
  // Register basic Partitions
  asiEngine_Model::initPartitions();

  // Register custom Partitions
  REGISTER_PARTITION(asiData_Partition<asiData_RENode>,         Partition_RE);
  REGISTER_PARTITION(asiData_Partition<asiData_REContourNode>,  Partition_REContour);
  REGISTER_PARTITION(asiData_Partition<asiData_REContoursNode>, Partition_REContours);
  REGISTER_PARTITION(asiData_Partition<asiData_REPointsNode>,   Partition_REPoints);
  REGISTER_PARTITION(asiData_Partition<asiData_RESurfaceNode>,  Partition_RESurface);
  REGISTER_PARTITION(asiData_Partition<asiData_RESurfacesNode>, Partition_RESurfaces);
}
