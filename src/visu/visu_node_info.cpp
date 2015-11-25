//-----------------------------------------------------------------------------
// Created on: 16 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <visu_node_info.h>

// VTK includes
#include <vtkActor.h>
#include <vtkInformation.h>
#include <vtkInformationObjectBaseKey.h>
#include <vtkObjectFactory.h>

vtkSmartPointer<vtkInformationObjectBaseKey> visu_node_info::m_key = NULL;

vtkStandardNewMacro(visu_node_info);

//! Default constructor.
visu_node_info::visu_node_info()
{
}

//! Default destructor.
visu_node_info::~visu_node_info()
{
}

//! Sets Node ID to store in actor's Information property.
//! \param nodeId [in] Node ID to store.
void visu_node_info::SetNodeId(const ActAPI_DataObjectId& nodeId)
{
  m_nodeId = nodeId;
}

//! Accessor for Node ID.
//! \return Node ID.
ActAPI_DataObjectId visu_node_info::GetNodeId() const
{
  return m_nodeId;
}

//! Accessor for statically defined information key used to store Node ID
//! in actor's Information properties.
//! \return information key.
vtkInformationObjectBaseKey* visu_node_info::GetKey()
{
  if ( m_key.GetPointer() == NULL )
    m_key = new vtkInformationObjectBaseKey("NodeInformation", "visu_node_info::m_key");
  return m_key;
}

//! Retrieves Information properties from the passed actor attempting to
//! access NodeInformation reference. If such reference is not bound, returns
//! NULL pointer.
//! \param actor [in] actor to access information from.
//! \return requested NodeInformation reference or NULL.
visu_node_info* visu_node_info::Retrieve(vtkActor* actor)
{
  visu_node_info* result = NULL;
  vtkInformation* info = actor->GetPropertyKeys();
  if ( info )
  {
    vtkInformationObjectBaseKey* key = GetKey();
    if ( key->Has(info) )
      result = reinterpret_cast<visu_node_info*>( key->Get(info) );
  }
  return result;
}

//! Sets actor's Information property storing the passed Node ID.
//! \param nodeId [in] Node ID to store.
//! \param actor  [in] actor to store the Node ID in.
void visu_node_info::Store(const ActAPI_DataObjectId& nodeId,
                           vtkActor*                  actor)
{
  if ( !actor->GetPropertyKeys() )
    actor->SetPropertyKeys( vtkSmartPointer<vtkInformation>::New() );

  // Create new wrapper for Node ID
  vtkSmartPointer<visu_node_info> nodeInfo = vtkSmartPointer<visu_node_info>::New();
  nodeInfo->SetNodeId(nodeId);

  // Set Information property
  GetKey()->Set(actor->GetPropertyKeys(), nodeInfo);
}
