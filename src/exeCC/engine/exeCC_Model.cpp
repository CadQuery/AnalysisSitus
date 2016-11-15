//-----------------------------------------------------------------------------
// Created on: 15 November 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <exeCC_Model.h>

//-----------------------------------------------------------------------------
// Register Node types
//-----------------------------------------------------------------------------

REGISTER_NODE_TYPE(asiData_ContourNode)

//-----------------------------------------------------------------------------

//! Default constructor.
exeCC_Model::exeCC_Model() : asiEngine_Model()
{}

//-----------------------------------------------------------------------------

//! Populates Data Model.
void exeCC_Model::Populate()
{
  // Populate base data
  asiEngine_Model::Populate();

  // Get Part Node
  Handle(asiData_PartNode) part_n = this->GetPartNode();

  // Create underlying Contour Node
  {
    Handle(ActAPI_INode) geom_contour_base = asiData_ContourNode::Instance();
    this->GetContourPartition()->AddNode(geom_contour_base);

    // Initialize
    Handle(asiData_ContourNode) geom_contour_n = Handle(asiData_ContourNode)::DownCast(geom_contour_base);
    geom_contour_n->Init();
    geom_contour_n->SetName("Contour to capture");

    // Set as child
    part_n->AddChildNode(geom_contour_n);
  }
}

//-----------------------------------------------------------------------------

//! Clears the Model.
void exeCC_Model::Clear()
{
  // Clean base data
  asiEngine_Model::Clear();
}

//-----------------------------------------------------------------------------

//! \return single Contour Node.
Handle(asiData_ContourNode) exeCC_Model::GetContourNode() const
{
  for ( ActData_BasePartition::Iterator it( this->GetContourPartition() ); it.More(); it.Next() )
  {
    Handle(asiData_ContourNode) N = Handle(asiData_ContourNode)::DownCast( it.Value() );
    //
    if ( !N.IsNull() && N->IsWellFormed() )
      return N;
  }
  return NULL;
}

//-----------------------------------------------------------------------------

//! Initializes Partitions.
void exeCC_Model::initPartitions()
{
  // Register basic Partitions
  asiEngine_Model::initPartitions();

  // Register custom Partitions
  REGISTER_PARTITION(asiData_Partition<asiData_ContourNode>, Partition_Contour);
}
