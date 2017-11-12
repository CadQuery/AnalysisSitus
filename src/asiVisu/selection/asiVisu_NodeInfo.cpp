//-----------------------------------------------------------------------------
// Created on: 16 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of Sergey Slyadnev nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_NodeInfo.h>

// VTK includes
#include <vtkActor.h>
#include <vtkInformation.h>
#include <vtkInformationObjectBaseKey.h>
#include <vtkObjectFactory.h>

vtkSmartPointer<vtkInformationObjectBaseKey> asiVisu_NodeInfo::m_key = NULL;

vtkStandardNewMacro(asiVisu_NodeInfo);

//! Default constructor.
asiVisu_NodeInfo::asiVisu_NodeInfo()
{}

//! Destructor.
asiVisu_NodeInfo::~asiVisu_NodeInfo()
{}

//! Sets Node ID to store in actor's Information property.
//! \param nodeId [in] Node ID to store.
void asiVisu_NodeInfo::SetNodeId(const ActAPI_DataObjectId& nodeId)
{
  m_nodeId = nodeId;
}

//! Accessor for Node ID.
//! \return Node ID.
ActAPI_DataObjectId asiVisu_NodeInfo::GetNodeId() const
{
  return m_nodeId;
}

//! Accessor for statically defined information key used to store Node ID
//! in actor's Information properties.
//! \return information key.
vtkInformationObjectBaseKey* asiVisu_NodeInfo::GetKey()
{
  if ( m_key.GetPointer() == NULL )
    m_key = new vtkInformationObjectBaseKey("NodeInformation", "asiVisu_NodeInfo::m_key");
  return m_key;
}

//! Retrieves Information properties from the passed actor attempting to
//! access NodeInformation reference. If such reference is not bound, returns
//! NULL pointer.
//! \param actor [in] actor to access information from.
//! \return requested NodeInformation reference or NULL.
asiVisu_NodeInfo* asiVisu_NodeInfo::Retrieve(vtkActor* actor)
{
  asiVisu_NodeInfo* result = NULL;
  //
  if ( !actor )
    return result;

  vtkInformation* info = actor->GetPropertyKeys();
  //
  if ( info )
  {
    vtkInformationObjectBaseKey* key = GetKey();
    if ( key->Has(info) )
      result = dynamic_cast<asiVisu_NodeInfo*>( key->Get(info) );
  }
  return result;
}

//! Sets actor's Information property storing the passed Node ID.
//! \param nodeId [in] Node ID to store.
//! \param actor  [in] actor to store the Node ID in.
void asiVisu_NodeInfo::Store(const ActAPI_DataObjectId& nodeId,
                             vtkActor*                  actor)
{
  if ( !actor->GetPropertyKeys() )
    actor->SetPropertyKeys( vtkSmartPointer<vtkInformation>::New() );

  // Create new wrapper for Node ID
  vtkSmartPointer<asiVisu_NodeInfo> nodeInfo = vtkSmartPointer<asiVisu_NodeInfo>::New();
  nodeInfo->SetNodeId(nodeId);

  // Set Information property
  GetKey()->Set(actor->GetPropertyKeys(), nodeInfo);
}
