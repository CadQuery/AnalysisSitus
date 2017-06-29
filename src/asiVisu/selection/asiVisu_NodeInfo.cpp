//-----------------------------------------------------------------------------
// Created on: 16 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
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

//! Default destructor.
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
  vtkInformation* info = actor->GetPropertyKeys();
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
