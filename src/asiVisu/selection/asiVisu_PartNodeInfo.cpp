//-----------------------------------------------------------------------------
// Created on: 13 February 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_PartNodeInfo.h>

// VTK includes
#include <vtkActor.h>
#include <vtkInformation.h>
#include <vtkInformationObjectBaseKey.h>
#include <vtkObjectFactory.h>

vtkStandardNewMacro(asiVisu_PartNodeInfo);

//! Default constructor.
asiVisu_PartNodeInfo::asiVisu_PartNodeInfo()
{}

//! Default destructor.
asiVisu_PartNodeInfo::~asiVisu_PartNodeInfo()
{}

//! Accessor for statically defined information key used to store Node ID
//! in actor's Information properties.
//! \return information key.
vtkInformationObjectBaseKey* asiVisu_PartNodeInfo::GetKey()
{
  if ( m_key.GetPointer() == NULL )
    m_key = new vtkInformationObjectBaseKey("NodeInformation", "asiVisu_PartNodeInfo::m_key");
  return m_key;
}

//! Retrieves Information properties from the passed actor attempting to
//! access NodeInformation reference. If such reference is not bound, returns
//! NULL pointer.
//! \param actor [in] actor to access information from.
//! \return requested NodeInformation reference or NULL.
asiVisu_PartNodeInfo* asiVisu_PartNodeInfo::Retrieve(vtkActor* actor)
{
  asiVisu_PartNodeInfo* result = NULL;
  vtkInformation* info = actor->GetPropertyKeys();
  if ( info )
  {
    vtkInformationObjectBaseKey* key = GetKey();
    if ( key->Has(info) )
      result = dynamic_cast<asiVisu_PartNodeInfo*>( key->Get(info) );
  }
  return result;
}

//! Sets actor's Information property storing the passed Node ID.
//! \param nodeId [in] Node ID to store.
//! \param actor  [in] actor to store the Node ID in.
void asiVisu_PartNodeInfo::Store(const ActAPI_DataObjectId& nodeId,
                                 vtkActor*                  actor)
{
  if ( !actor->GetPropertyKeys() )
    actor->SetPropertyKeys( vtkSmartPointer<vtkInformation>::New() );

  // Create new wrapper for Node ID
  vtkSmartPointer<asiVisu_PartNodeInfo> nodeInfo = vtkSmartPointer<asiVisu_PartNodeInfo>::New();
  nodeInfo->SetNodeId(nodeId);

  // Set Information property
  GetKey()->Set(actor->GetPropertyKeys(), nodeInfo);
}
