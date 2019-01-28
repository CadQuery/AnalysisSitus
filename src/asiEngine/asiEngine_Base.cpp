//-----------------------------------------------------------------------------
// Created on: 06 October 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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
#include <asiEngine_Base.h>

//-----------------------------------------------------------------------------

Handle(ActAPI_HNodeList) asiEngine_Base::FindPresentableNodes()
{
  Handle(ActAPI_HNodeList) result = new ActAPI_HNodeList;

  // Iterate over the Partitions as they give us the storage order.
  Handle(ActAPI_HPartitionList) parititons = m_model->Partitions();
  //
  for ( ActAPI_HPartitionList::Iterator it(*parititons); it.More(); it.Next() )
  {
    Handle(ActAPI_IPartition) iPartition = it.Value();

    // Access base Partition.
    Handle(ActData_BasePartition)
      partition = Handle(ActData_BasePartition)::DownCast( it.Value() );
    //
    if ( partition.IsNull() )
      Standard_ProgramError::Raise("Non-standard Partitions prohibited");

    // Iterate over the Nodes in their persistent order.
    for ( ActData_BasePartition::Iterator nit(partition); nit.More(); nit.Next() )
    {
      Handle(ActAPI_INode) N = nit.Value();
      //
      if ( N.IsNull() || !N->IsWellFormed() )
        continue;

      // Add presentable Nodes to the resulting collection.
      const int userFlags = N->GetUserFlags();
      if ( userFlags & NodeFlag_IsPresentedInPartView )
        result->Append(N);
    }
  }

  return result;
}

//-----------------------------------------------------------------------------

void asiEngine_Base::_cleanChildren(const Handle(ActAPI_INode)& parent)
{
  Handle(ActAPI_HNodeList) nodesToDelete = new ActAPI_HNodeList;

  // Loop over direct children of a Surfaces Node
  for ( Handle(ActAPI_IChildIterator) cit = parent->GetChildIterator(); cit->More(); cit->Next() )
  {
    Handle(ActAPI_INode) child_n = cit->Value();

    // Check if the given Node is consistent
    if ( child_n.IsNull() || !child_n->IsWellFormed() )
      continue;

    // Set Node for deletion
    nodesToDelete->Append(child_n);
  }

  // Delete all Nodes queued for removal
  for ( ActAPI_NodeList::Iterator nit( *nodesToDelete.operator->() ); nit.More(); nit.Next() )
    m_model->DeleteNode( nit.Value()->GetId() );
}
