//-----------------------------------------------------------------------------
// Created on: 13 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <engine_volume.h>

// A-Situs (common) includes
#include <common_facilities.h>
#include <common_model.h>

// Active Data includes
#include <ActData_ParameterFactory.h>
#include <ActData_UniqueNodeName.h>

//-----------------------------------------------------------------------------

//! Creates a new Volume Node.
//! \param shape  [in] geometry to store.
//! \param name   [in] name of the Node.
//! \param parent [in] optional parent.
//! \return newly created Volume Node.
Handle(geom_volume_node)
  engine_volume::Create_Volume(const TopoDS_Shape&            shape,
                               const TCollection_AsciiString& name,
                               const Handle(ActAPI_INode)&    parent)
{
  // Access Model
  Handle(common_model) M = common_facilities::Instance()->Model;

  // Add Node to Partition
  Handle(geom_volume_node) volume_n = Handle(geom_volume_node)::DownCast( geom_volume_node::Instance() );
  M->GetGeomVolumePartition()->AddNode(volume_n);

  // Generate unique name
  TCollection_ExtendedString item_name;
  if ( !parent.IsNull() && parent->IsWellFormed() )
  {
    item_name = ( name.IsEmpty() ? "Volume" : name );
    item_name = ActData_UniqueNodeName::Generate(ActData_SiblingNodes::CreateForChild(volume_n, parent), item_name);
  }
  else
    item_name = name;

  // Initialize Node
  volume_n->Init();
  volume_n->SetName(item_name);
  volume_n->SetShape(shape);

  // Add as child
  if ( !parent.IsNull() && parent->IsWellFormed() )
    parent->AddChildNode(volume_n);

  // Return the just created Node
  return volume_n;
}

//-----------------------------------------------------------------------------

//! Deletes all Volumes.
void engine_volume::Clean_Volumes()
{
  Handle(common_model)     M             = common_facilities::Instance()->Model;
  Handle(ActAPI_HNodeList) nodesToDelete = new ActAPI_HNodeList;

  // Collect Nodes to delete
  for ( ActData_BasePartition::Iterator it( M->GetGeomVolumePartition() ); it.More(); it.Next() )
  {
    nodesToDelete->Append( it.Value() );
  }

  // Delete all Nodes queued for removal
  for ( ActAPI_NodeList::Iterator nit( *nodesToDelete.operator->() ); nit.More(); nit.Next() )
  {
    if ( !nit.Value().IsNull() && nit.Value()->IsWellFormed() )
      M->DeleteNode( nit.Value()->GetId() );
  }
}

//-----------------------------------------------------------------------------

//! \return root volume.
Handle(ActAPI_INode) engine_volume::Get_RootVolume()
{
  Handle(common_model) M = common_facilities::Instance()->Model;

  // Iterate over the Partition trying to find a volume without a parent
  for ( ActData_BasePartition::Iterator it( M->GetGeomVolumePartition() ); it.More(); it.Next() )
  {
    Handle(ActAPI_INode) item_n = it.Value();
    //
    if ( item_n->GetParentNode().IsNull() )
      return item_n;
  }

  return NULL;
}

//-----------------------------------------------------------------------------

//! \return all leaf volumes.
Handle(ActAPI_HDataCursorList) engine_volume::Get_LeafVolumes()
{
  return Get_LeafVolumesExcept(NULL);
}

//-----------------------------------------------------------------------------

//! Returns all leaf items of the feature tree excluding the passed one.
//! \param excludedItem [in] item to exclude.
//! \return all leaf items except the given one.
Handle(ActAPI_HDataCursorList)
  engine_volume::Get_LeafVolumesExcept(const Handle(ActAPI_INode)& excludedItem)
{
  Handle(common_model)           M    = common_facilities::Instance()->Model;
  Handle(ActAPI_HDataCursorList) List = new ActAPI_HDataCursorList;

  // Iterate over the Partition and take only those volumes which do not
  // have any children
  for ( ActData_BasePartition::Iterator it( M->GetGeomVolumePartition() ); it.More(); it.Next() )
  {
    Handle(ActAPI_INode) item_n = it.Value();

    // Skip the undesired item
    if ( !excludedItem.IsNull() && ActAPI_IDataCursor::IsEqual(item_n, excludedItem) )
      continue;

    // Check the number of children
    Handle(ActAPI_IChildIterator) cit = item_n->GetChildIterator();
    if ( cit->More() )
      continue;

    // Add the appropriate item to the result list
    List->Append(item_n);
  }

  return List;
}

//-----------------------------------------------------------------------------

//! Moves the given feature from the list of kept items to the list of
//! excluded items.
//! \param owner_n   [in] owner Node.
//! \param feature_n [in] feature to move.
void engine_volume::Move_Feature2Excluded(const Handle(ActAPI_INode)& owner_n,
                                          const Handle(ActAPI_INode)& feature_n)
{
  Handle(ActData_ReferenceListParameter)
    ref_kept = ActParamTool::AsReferenceList( owner_n->Parameter(geom_volume_node::PID_Features2Keep) );
  //
  Handle(ActData_ReferenceListParameter)
    ref_excluded = ActParamTool::AsReferenceList( owner_n->Parameter(geom_volume_node::PID_Features2Exclude) );

  // Transfer target
  ref_kept->RemoveTarget(feature_n);
  ref_excluded->AddTarget(feature_n);
}

//! Moves the given feature from the list of excluded items to the list of
//! kept items.
//! \param owner_n   [in] owner Node.
//! \param feature_n [in] feature to move.
void engine_volume::Move_Feature2Kept(const Handle(ActAPI_INode)& owner_n,
                                      const Handle(ActAPI_INode)& feature_n)
{
  Handle(ActData_ReferenceListParameter)
    ref_kept = ActParamTool::AsReferenceList( owner_n->Parameter(geom_volume_node::PID_Features2Keep) );
  //
  Handle(ActData_ReferenceListParameter)
    ref_excluded = ActParamTool::AsReferenceList( owner_n->Parameter(geom_volume_node::PID_Features2Exclude) );

  // Transfer target
  ref_excluded->RemoveTarget(feature_n);
  ref_kept->AddTarget(feature_n);
}
