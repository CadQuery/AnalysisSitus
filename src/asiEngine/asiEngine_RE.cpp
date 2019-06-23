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
#include <asiEngine_RE.h>

// Active Data includes
#include <ActData_UniqueNodeName.h>

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

Handle(asiData_RePatchesNode) asiEngine_RE::Get_Patches()
{
  Handle(asiData_Partition<asiData_RePatchesNode>)
    patches_p = m_model->GetRePatchesPartition();

  return Handle(asiData_RePatchesNode)::DownCast( patches_p->GetNode(1) );
}

//-----------------------------------------------------------------------------

Handle(asiData_ReEdgesNode) asiEngine_RE::Get_Edges()
{
  Handle(asiData_Partition<asiData_ReEdgesNode>)
    edges_p = m_model->GetReEdgesPartition();

  return Handle(asiData_ReEdgesNode)::DownCast( edges_p->GetNode(1) );
}

//-----------------------------------------------------------------------------

Handle(asiData_ReVerticesNode) asiEngine_RE::Get_Vertices()
{
  Handle(asiData_Partition<asiData_ReVerticesNode>)
    vertices_p = m_model->GetReVerticesPartition();

  return Handle(asiData_ReVerticesNode)::DownCast( vertices_p->GetNode(1) );
}

//-----------------------------------------------------------------------------

Handle(asiData_RePatchNode) asiEngine_RE::Create_Patch()
{
  // Get or create parent Topo Node.
  Handle(asiData_ReTopoNode) topo_n = m_model->GetReTopoNode();;
  //
  if ( topo_n.IsNull() )
    topo_n = this->Create_Topo();

  // Get Patches Node.
  Handle(asiData_RePatchesNode) patches_n = this->Get_Patches();

  // Add Node to Partition.
  Handle(asiData_RePatchNode)
    patch_n = Handle(asiData_RePatchNode)::DownCast( asiData_RePatchNode::Instance() );
  //
  m_model->GetRePatchPartition()->AddNode(patch_n);

  // Initialize Node.
  patch_n->Init();

  // Add as a child to Patches Node.
  patches_n->AddChildNode(patch_n);

  // Generate unique name.
  TCollection_ExtendedString
    patchName = ActData_UniqueNodeName::Generate(ActData_SiblingNodes::CreateForChild(patch_n, patches_n), "Patch");
  //
  patch_n->SetName(patchName);

  // Return the just created Node.
  return patch_n;
}

//-----------------------------------------------------------------------------

Handle(asiData_ReEdgeNode)
  asiEngine_RE::Create_Edge(const Handle(asiData_ReVertexNode)& vfirst,
                            const Handle(asiData_ReVertexNode)& vlast)
{
  // Get or create parent Topo Node.
  Handle(asiData_ReTopoNode) topo_n = m_model->GetReTopoNode();;
  //
  if ( topo_n.IsNull() )
    topo_n = this->Create_Topo();

  // Get parent Edges Node.
  Handle(asiData_ReEdgesNode) edges_n = this->Get_Edges();

  // Add Node to Partition.
  Handle(asiData_ReEdgeNode)
    edge_n = Handle(asiData_ReEdgeNode)::DownCast( asiData_ReEdgeNode::Instance() );
  //
  m_model->GetReEdgePartition()->AddNode(edge_n);

  // Initialize Node.
  edge_n->Init(vfirst, vlast);
  edge_n->SetUserFlags(NodeFlag_IsPresentedInPartView);

  // Add as a child to Edges Node.
  edges_n->AddChildNode(edge_n);

  // Generate unique name.
  TCollection_ExtendedString
    edgeName = ActData_UniqueNodeName::Generate(ActData_SiblingNodes::CreateForChild(edge_n, edges_n), "Edge");
  //
  edge_n->SetName(edgeName);

  // Return the just created Node.
  return edge_n;
}

//-----------------------------------------------------------------------------

Handle(asiData_ReCoEdgeNode)
  asiEngine_RE::Create_CoEdge(const Handle(asiData_RePatchNode)& patch,
                              const Handle(asiData_ReEdgeNode)&  edge,
                              const bool                         samesense)
{
  // Add Node to Partition.
  Handle(asiData_ReCoEdgeNode)
    coedge_n = Handle(asiData_ReCoEdgeNode)::DownCast( asiData_ReCoEdgeNode::Instance() );
  //
  m_model->GetReCoEdgePartition()->AddNode(coedge_n);

  // Initialize Node.
  coedge_n->Init(edge, samesense);
  coedge_n->SetUserFlags(NodeFlag_IsPresentedInPartView);

  // Add as a child to Patch Node.
  patch->AddChildNode(coedge_n);

  // Generate unique name.
  TCollection_ExtendedString
    edgeName = ActData_UniqueNodeName::Generate(ActData_SiblingNodes::CreateForChild(coedge_n, patch), "CoEdge");
  //
  coedge_n->SetName(edgeName);

  // Return the just created Node.
  return coedge_n;
}

//-----------------------------------------------------------------------------

Handle(asiData_ReVertexNode)
  asiEngine_RE::Create_Vertex(const gp_XYZ& coords, const gp_XYZ& norm)
{
  // Get or create parent Topo Node.
  Handle(asiData_ReTopoNode) topo_n = m_model->GetReTopoNode();;
  //
  if ( topo_n.IsNull() )
    topo_n = this->Create_Topo();

  // Get parent Vertices Node.
  Handle(asiData_ReVerticesNode) vertices_n = this->Get_Vertices();

  // Add Node to Partition.
  Handle(asiData_ReVertexNode)
    vertex_n = Handle(asiData_ReVertexNode)::DownCast( asiData_ReVertexNode::Instance() );
  //
  m_model->GetReVertexPartition()->AddNode(vertex_n);

  // Initialize Node.
  vertex_n->Init( coords.X(), coords.Y(), coords.Z(),
                  norm.X(),   norm.Y(),   norm.Z() );

  vertex_n->SetUserFlags(NodeFlag_IsPresentedInPartView);

  // Add as a child to Vertices Node.
  vertices_n->AddChildNode(vertex_n);

  // Generate unique name.
  TCollection_ExtendedString
    vertexName = ActData_UniqueNodeName::Generate(ActData_SiblingNodes::CreateForChild(vertex_n, vertices_n), "Vertex");
  //
  vertex_n->SetName(vertexName);

  // Return the just created Node.
  return vertex_n;
}

//-----------------------------------------------------------------------------

void asiEngine_RE::CollectBoundaryPoints(const Handle(asiData_RePatchNode)& patch,
                                         std::vector<gp_XYZ>&               pts) const
{
  // Iterate over the coedges.
  for ( Handle(ActAPI_IChildIterator) cit = patch->GetChildIterator(); cit->More(); cit->Next() )
  {
    Handle(asiData_ReCoEdgeNode)
      coedge = Handle(asiData_ReCoEdgeNode)::DownCast( cit->Value() );
    //
    if ( coedge.IsNull() || !coedge->IsWellFormed() )
      continue;

    // Get referenced edge to access the geometry.
    Handle(asiData_ReEdgeNode) edge = coedge->GetEdge();
    //
    if ( edge.IsNull() || !edge->IsWellFormed() )
      continue;

    std::vector<gp_XYZ> edgePts;
    edge->GetPolyline(edgePts);

    // Add points to the result.
    for ( size_t k = 0; k < edgePts.size(); ++k )
      pts.push_back(edgePts[k]);
  }
}

//-----------------------------------------------------------------------------

void asiEngine_RE::CollectVertexPoints(const Handle(asiData_RePatchNode)& patch,
                                       std::vector<gp_XYZ>&               pts) const
{
  NCollection_Map<ActAPI_DataObjectId> visited; // Visited vertices.

  // Iterate over the coedges.
  for ( Handle(ActAPI_IChildIterator) cit = patch->GetChildIterator(); cit->More(); cit->Next() )
  {
    Handle(asiData_ReCoEdgeNode)
      coedge = Handle(asiData_ReCoEdgeNode)::DownCast( cit->Value() );
    //
    if ( coedge.IsNull() || !coedge->IsWellFormed() )
      continue;

    // Get referenced edge to access the geometry.
    Handle(asiData_ReEdgeNode) edge = coedge->GetEdge();
    //
    if ( edge.IsNull() || !edge->IsWellFormed() )
      continue;

    Handle(asiData_ReVertexNode) vf = edge->GetFirstVertex();
    Handle(asiData_ReVertexNode) vl = edge->GetLastVertex();

    if ( !visited.Contains( vf->GetId() ) )
    {
      visited.Add( vf->GetId() );
      pts.push_back( vf->GetPoint() );
    }

    if ( !visited.Contains( vl->GetId() ) )
    {
      visited.Add( vl->GetId() );
      pts.push_back( vl->GetPoint() );
    }
  }
}

//-----------------------------------------------------------------------------

void asiEngine_RE::CollectContourTriangles(const Handle(asiData_RePatchNode)& patch,
                                           TColStd_PackedMapOfInteger&        tris) const
{
  // Iterate over the coedges.
  for ( Handle(ActAPI_IChildIterator) cit = patch->GetChildIterator(); cit->More(); cit->Next() )
  {
    Handle(asiData_ReCoEdgeNode)
      coedge = Handle(asiData_ReCoEdgeNode)::DownCast( cit->Value() );
    //
    if ( coedge.IsNull() || !coedge->IsWellFormed() )
      continue;

    // Get referenced edge to access the geometry.
    Handle(asiData_ReEdgeNode) edge = coedge->GetEdge();
    //
    if ( edge.IsNull() || !edge->IsWellFormed() )
      continue;

    // Accumulate all indices into one collection.
    Handle(HIntArray) inds = edge->GetPolylineTriangles();
    //
    if ( !inds.IsNull() )
      for ( int i = inds->Lower(); i <= inds->Upper(); ++i )
        tris.Add( inds->Value(i) );
  }
}
