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
#include <asiEngine_RE.h>

//-----------------------------------------------------------------------------

Handle(asiData_ReTopoNode) asiEngine_RE::Create_Topo()
{
  /* ==================
   *  Create root Node
   * ================== */

  // Add Node to Partition.
  Handle(asiData_ReTopoNode)
    topo_n = Handle(asiData_ReTopoNode)::DownCast( asiData_ReTopoNode::Instance() );
  //
  m_model->GetReTopoPartition()->AddNode(topo_n);

  // Initialize Node.
  topo_n->Init();
  topo_n->SetName("Topology");

  // Add as a child to Triangulation Node.
  m_model->GetTriangulationNode()->AddChildNode(topo_n);

  /* ====================
   *  Create child Nodes
   * ==================== */

  this->Create_Patches  (topo_n);
  this->Create_Edges    (topo_n);
  this->Create_Vertices (topo_n);

  // Return the just created Node.
  return topo_n;
}

//-----------------------------------------------------------------------------

Handle(asiData_ContourNode)
  asiEngine_RE::Create_Contour(const Handle(asiData_ReTopoNode)& topo_n)
{
  // Add Node to Partition.
  Handle(asiData_ContourNode)
    contour_n = Handle(asiData_ContourNode)::DownCast( asiData_ContourNode::Instance() );
  //
  m_model->GetContourPartition()->AddNode(contour_n);

  // Initialize.
  contour_n->Init();
  contour_n->SetName("Active contour");

  // Set as child.
  topo_n->AddChildNode(contour_n);

  // Return the just created Node.
  return contour_n;
}

//-----------------------------------------------------------------------------

Handle(asiData_RePatchesNode)
  asiEngine_RE::Create_Patches(const Handle(asiData_ReTopoNode)& topo_n)
{
  // Add Node to Partition.
  Handle(asiData_RePatchesNode)
    patches_n = Handle(asiData_RePatchesNode)::DownCast( asiData_RePatchesNode::Instance() );
  //
  m_model->GetRePatchesPartition()->AddNode(patches_n);

  // Initialize Node.
  patches_n->Init();
  patches_n->SetName("Patches");

  // Add as a child to Topology Node.
  topo_n->AddChildNode(patches_n);

  // Return the just created Node.
  return patches_n;
}

//-----------------------------------------------------------------------------

Handle(asiData_ReEdgesNode)
  asiEngine_RE::Create_Edges(const Handle(asiData_ReTopoNode)& topo_n)
{
  // Add Node to Partition.
  Handle(asiData_ReEdgesNode)
    edges_n = Handle(asiData_ReEdgesNode)::DownCast( asiData_ReEdgesNode::Instance() );
  //
  m_model->GetReEdgesPartition()->AddNode(edges_n);

  // Initialize Node.
  edges_n->Init();
  edges_n->SetName("Edges");

  // Add as a child to Topology Node.
  topo_n->AddChildNode(edges_n);

  // Return the just created Node.
  return edges_n;
}

//-----------------------------------------------------------------------------

Handle(asiData_ReVerticesNode)
  asiEngine_RE::Create_Vertices(const Handle(asiData_ReTopoNode)& topo_n)
{
  // Add Node to Partition.
  Handle(asiData_ReVerticesNode)
    vertices_n = Handle(asiData_ReVerticesNode)::DownCast( asiData_ReVerticesNode::Instance() );
  //
  m_model->GetReVerticesPartition()->AddNode(vertices_n);

  // Initialize Node.
  vertices_n->Init();
  vertices_n->SetName("Vertices");

  // Add as a child to Topology Node.
  topo_n->AddChildNode(vertices_n);

  // Return the just created Node.
  return vertices_n;
}

//-----------------------------------------------------------------------------

Handle(asiData_ContourNode) asiEngine_RE::GetOrCreate_Contour()
{
  Handle(asiData_Partition<asiData_ReTopoNode>) topo_p = m_model->GetReTopoPartition();

  // Get number of Topology Nodes.
  int numTopoNodes = 0;
  Handle(asiData_ReTopoNode) any_topo_n;
  //
  for ( asiData_Partition<asiData_ReTopoNode>::Iterator pit(topo_p); pit.More(); pit.Next() )
  {
    any_topo_n = Handle(asiData_ReTopoNode)::DownCast( pit.Value() );
    numTopoNodes++;
  }

  // Get Topology Node.
  Handle(asiData_ReTopoNode) topo_n;
  if ( !numTopoNodes )
    topo_n = this->Create_Topo();
  else
    topo_n = any_topo_n;

  // Get Contour Node.
  Handle(asiData_ContourNode) contour_n;
  for ( Handle(ActAPI_IChildIterator) cit = topo_n->GetChildIterator(); cit->More(); cit->Next() )
  {
    Handle(ActAPI_INode) child_n = cit->Value();
    //
    if ( child_n->IsKind( STANDARD_TYPE(asiData_ContourNode) ) )
    {
      contour_n = Handle(asiData_ContourNode)::DownCast(child_n);
      break;
    }
  }

  if ( !contour_n.IsNull() )
    return contour_n;

  return this->Create_Contour(topo_n);
}
