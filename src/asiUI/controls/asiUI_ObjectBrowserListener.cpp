//-----------------------------------------------------------------------------
// Created on: 30 May 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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
//    * Neither the name of the copyright holder(s) nor the
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
#include <asiUI_ObjectBrowserListener.h>

// asiUI includes
#include <asiUI_ParamEditorImpl.h>

// asiData includes
#include <asiData_ElemMetadataNode.h>

//-----------------------------------------------------------------------------

asiUI_ObjectBrowserListener::asiUI_ObjectBrowserListener(const Handle(asiUI_CommonFacilities)& cf)
: QObject (),
  m_cf    (cf)
{}

//-----------------------------------------------------------------------------

asiUI_ObjectBrowserListener::~asiUI_ObjectBrowserListener()
{}

//-----------------------------------------------------------------------------

void asiUI_ObjectBrowserListener::Connect()
{
  if ( !m_cf->ObjectBrowser )
    return;

  // Connect the default reaction.
  connect( m_cf->ObjectBrowser, SIGNAL ( nodesSelected      (const ActAPI_DataObjectIdList&) ),
           this,                SLOT   ( onSelectionChanged (const ActAPI_DataObjectIdList&) ) );
}

//-----------------------------------------------------------------------------

void asiUI_ObjectBrowserListener::onSelectionChanged(const ActAPI_DataObjectIdList& nodes)
{
  for ( ActAPI_DataObjectIdList::Iterator nit(nodes); nit.More(); nit.Next() )
    this->processNode( nit.Value() );
}

//-----------------------------------------------------------------------------

void asiUI_ObjectBrowserListener::processNode(const ActAPI_DataObjectId& nid)
{
  Handle(ActAPI_INode) node = m_cf->Model->FindNode(nid);
  //
  if ( node.IsNull() || !node->IsWellFormed() )
    return;

  if ( node->IsInstance( STANDARD_TYPE(asiData_ElemMetadataNode) ) )
  {
    Handle(asiData_ElemMetadataNode)
      emn = Handle(asiData_ElemMetadataNode)::DownCast(node);

    // Get part shape.
    TopoDS_Shape partShape = m_cf->Model->GetPartNode()->GetShape();

    // Check if the sub-shape is contained with the part.
    TopoDS_Shape subshape = emn->GetShape();
    //
    if ( asiAlgo_Utils::Contains(partShape, subshape) )
      m_cf->Progress.SendLogMessage(LogInfo(Normal) << "Sub-shape is ALIVE.");
    else
      m_cf->Progress.SendLogMessage(LogWarn(Normal) << "Sub-shape is DEAD.");
  }
}
