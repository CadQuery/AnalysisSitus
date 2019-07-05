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

// asiAlgo includes
#include <asiAlgo_ProjectPointOnMesh.h>

// Active Data includes
#include <ActData_UniqueNodeName.h>

// OCCT includes
#include <Poly_CoherentNode.hxx>
#include <Poly_CoherentTriangle.hxx>
#include <Poly_CoherentTriangulation.hxx>

#define DRAW_DEBUG
#if defined DRAW_DEBUG
  #pragma message("===== warning: DRAW_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

struct TriangleHasher
{
  static int HashCode(const Poly_CoherentTriangle* object, const int upper)
  {
    return ::HashCode((Standard_Address*) object, upper);
  }

  static bool IsEqual(const Poly_CoherentTriangle* object1, const Poly_CoherentTriangle* object2)
  {
    return object1 == object2;
  }
};

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

//-----------------------------------------------------------------------------

gp_XYZ asiEngine_RE::ComputeMidPoint(const Handle(asiData_RePatchNode)& patch) const
{
  std::vector<gp_XYZ> pts;
  this->CollectBoundaryPoints(patch, pts);

  return asiAlgo_Utils::ComputeAveragePoint(pts);
}

//-----------------------------------------------------------------------------

bool
  asiEngine_RE::ExtractBoundedRegion(const Handle(asiData_RePatchNode)& patch,
                                     Handle(Poly_Triangulation)&        region) const
{
  // Get indices of the triangles constituting the contour coverage.
  TColStd_PackedMapOfInteger boundaryInds;
  this->CollectContourTriangles(patch, boundaryInds);
  //
  if ( boundaryInds.IsEmpty() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "No triangles are covered by the patch contour.");
    return false;
  }

  // Get working triangulation.
  Handle(Poly_Triangulation) tris = m_model->GetTriangulationNode()->GetTriangulation();

  // Get BVH from the working triangulation.
  Handle(asiAlgo_BVHFacets) bvh = m_model->GetTriangulationNode()->GetBVH();

#if defined DRAW_DEBUG
  {
    // Extract sub-mesh.
    m_plotter.REDRAW_TRIANGULATION("bndTris",
                                   asiAlgo_Utils::GetSubMesh(tris, boundaryInds),
                                   Color_Red, 1.0);
  }
#endif

  // Get center point of the contour.
  gp_XYZ center = this->ComputeMidPoint(patch);

#if defined DRAW_DEBUG
  {
    m_plotter.REDRAW_POINT("center", center, Color_Red);
  }
#endif

  // Project midpoint to extract the central facet.
  asiAlgo_ProjectPointOnMesh pointToMesh(bvh, m_progress, m_plotter);
  //
  gp_Pnt centerProj     = pointToMesh.Perform(center);
  int    centerFacetInd = pointToMesh.GetFacetIds().size() ? pointToMesh.GetFacetIds()[0] : -1;
  //
  if ( centerFacetInd == -1 )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Failed to pick up a central facet of the bounded region.");
    return false;
  }

  // Convert facet index to triangle index.
  const int centerTriangleId = bvh->GetFacet(centerFacetInd).FaceIndex;

  // Index of the central facet should be decremented as in the coherent
  // triangulation we have 0-based indexation.
  const int cohCenterTriangleId = centerTriangleId - 1;

  // Construct a coherent triangulation which stores the connectivity
  // of mesh elements.
  Handle(Poly_CoherentTriangulation)
    cohTris = new Poly_CoherentTriangulation(tris);

#if defined DRAW_DEBUG
  {
    const Poly_CoherentTriangle& cohTri = cohTris->Triangle(cohCenterTriangleId);
    //
    const int N1 = cohTri.Node(0);
    const int N2 = cohTri.Node(1);
    const int N3 = cohTri.Node(2);

    m_plotter.REDRAW_TRIANGLE("centralFacetCoh", cohTris->Node(N1), cohTris->Node(N2), cohTris->Node(N3), Color_Blue);
  }

  {
    const Poly_Triangle& tri = tris->Triangle(centerTriangleId);
    //
    int N1, N2, N3;
    tri.Get(N1, N2, N3);

    m_plotter.REDRAW_TRIANGLE("centralFacet", tris->Node(N1), tris->Node(N2), tris->Node(N3), Color_Green);
  }
#endif

  // Prepare the collection of boundary triangles.
  NCollection_Map<const Poly_CoherentTriangle*, TriangleHasher> boundary;
  //
  for ( TColStd_MapIteratorOfPackedMapOfInteger tit(boundaryInds); tit.More(); tit.Next() )
  {
    const int id = tit.Key();
    //
    if ( id == -1 ) continue;

    const int cohId = id - 1; // Minus one for coherent notation.
    boundary.Add( &cohTris->Triangle(cohId) );
  }

  // Prepare the collection of seeds.
  std::vector<const Poly_CoherentTriangle*> seeds;
  seeds.push_back( &cohTris->Triangle(cohCenterTriangleId) );

  // Prepare the collection of already processed triangles.
  NCollection_Map<const Poly_CoherentTriangle*, TriangleHasher> processed;

  // Correspondence of old-to-new node IDs in the resulting mesh.
  NCollection_DataMap<int, int> nodeIds;
  int nextNodeId = 1;
  std::vector<gp_XYZ> newNodes;
  std::vector<Poly_Triangle> newTriangles;

  // Starting from the seed (center) triangle, traverse all neighbors until
  // the boundary triangles are reached.
  Handle(Poly_CoherentTriangulation) extracted = new Poly_CoherentTriangulation;
  //
  for ( size_t k = 0; k < seeds.size(); ++k )
  {
    const Poly_CoherentTriangle* seed = seeds[k];
    //
    if ( processed.Contains(seed) )
      continue; // Already processed.

    const Poly_CoherentTriangle* conn0 = seed->GetConnectedTri(0);
    const Poly_CoherentTriangle* conn1 = seed->GetConnectedTri(1);
    const Poly_CoherentTriangle* conn2 = seed->GetConnectedTri(2);

    if ( !boundary.Contains(conn0) )
      seeds.push_back(conn0);
    else
      std::cout << "Reached boundary on connection 0." << std::endl;

    if ( !boundary.Contains(conn1) )
      seeds.push_back(conn1);
    else
      std::cout << "Reached boundary on connection 1." << std::endl;

    if ( !boundary.Contains(conn2) )
      seeds.push_back(conn2);
    else
      std::cout << "Reached boundary on connection 2." << std::endl;

    // Add to the list of processed so that not to traverse again.
    processed.Add(seed);

    const int oldNodeId0 = seed->Node(0);
    const int oldNodeId1 = seed->Node(1);
    const int oldNodeId2 = seed->Node(2);
    //
    int newNodeId0, newNodeId1, newNodeId2;

    if ( nodeIds.IsBound(oldNodeId0) )
      newNodeId0 = nodeIds(oldNodeId0);
    else
    {
      newNodeId0 = nextNodeId++;
      nodeIds.Bind(oldNodeId0, newNodeId0);
      newNodes.push_back( cohTris->Node(oldNodeId0) );
    }
    //
    if ( nodeIds.IsBound(oldNodeId1) )
      newNodeId1 = nodeIds(oldNodeId1);
    else
    {
      newNodeId1 = nextNodeId++;
      nodeIds.Bind(oldNodeId1, newNodeId1);
      newNodes.push_back( cohTris->Node(oldNodeId1) );
    }
    //
    if ( nodeIds.IsBound(oldNodeId2) )
      newNodeId2 = nodeIds(oldNodeId2);
    else
    {
      newNodeId2 = nextNodeId++;
      nodeIds.Bind(oldNodeId2, newNodeId2);
      newNodes.push_back( cohTris->Node(oldNodeId2) );
    }

    // Add triangle.
    newTriangles.push_back( Poly_Triangle(newNodeId0, newNodeId1, newNodeId2) );
  }

  // Construct resulting triangulation.
  TColgp_Array1OfPnt    newNodesArr( 1, int( newNodes.size() ) );
  Poly_Array1OfTriangle newTrisArr( 1, int( newTriangles.size() ) );
  //
  for ( int i = 1; i <= newNodesArr.Length(); ++i )
    newNodesArr(i) = newNodes[i - 1];
  //
  for ( int i = 1; i <= newTrisArr.Length(); ++i )
    newTrisArr(i) = newTriangles[i - 1];

  // Set result and return.
  region = new Poly_Triangulation(newNodesArr, newTrisArr);

#if defined DRAW_DEBUG
  m_plotter.REDRAW_TRIANGULATION("region", region, Color_Blue, 1.0);
#endif

  return true;
}

//-----------------------------------------------------------------------------

bool
  asiEngine_RE::GetPatchesByEdge(const Handle(asiData_ReEdgeNode)& edge,
                                 Handle(asiData_RePatchNode)&      patchLeft,
                                 Handle(asiData_RePatchNode)&      patchRight) const
{
  // Get referrers.
  Handle(ActAPI_HParameterList) referrers = edge->GetReferrers();
  //
  if ( referrers.IsNull() || referrers->Length() == 0 )
    return false; // No coedges connected to the passed edge (?!)

  // Check referrers.
  for ( ActAPI_HParameterList::Iterator rit(*referrers); rit.More(); rit.Next() )
  {
    Handle(ActData_ReferenceParameter) ref = ActParamTool::AsReference( rit.Value() );

    // Get owning Node for the reference.
    Handle(asiData_ReCoEdgeNode) coedge = Handle(asiData_ReCoEdgeNode)::DownCast( ref->GetNode() );
    //
    if ( coedge.IsNull() )
      continue;

    if ( coedge->IsSameSense() )
      patchLeft = Handle(asiData_RePatchNode)::DownCast( coedge->GetParentNode() );
    else
      patchRight = Handle(asiData_RePatchNode)::DownCast( coedge->GetParentNode() );
  }

  return true;
}

//-----------------------------------------------------------------------------

bool asiEngine_RE::FillPatchCoons(const Handle(asiData_RePatchNode)& patch,
                                  Handle(Geom_BSplineSurface)&       surf) const
{
  std::vector<Handle(Geom_BSplineCurve)> curves;

  // Iterate over the coedges to collect curves for Coons interpolation. In
  // this loop, all curves are collected taking into account the orientation
  // flags stored in coedges, so that the 4-sided contour is properly oriented.
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

    // Get B-curve.
    Handle(Geom_BSplineCurve)
      bcurve = Handle(Geom_BSplineCurve)::DownCast( edge->GetCurve() );
    //
    if ( bcurve.IsNull() )
      return false; // For each edge, there should be a B-curve ready.

    // Collect curves taking into account the orientation of edges.
    if ( coedge->IsSameSense() )
      curves.push_back(bcurve);
    else
      curves.push_back( Handle(Geom_BSplineCurve)::DownCast( bcurve->Reversed() ) );
  }

  // Check if the patch is 4-cornered.
  if ( curves.size() != 4 )
    return false;

  const double prec = Precision::Confusion();

  // C0 is the first curve in the list (the 1-st edge).
  Handle(Geom_BSplineCurve) c0, c1, b0, b1;
  //
  c0 = curves[0];

  // P00, P10.
  gp_Pnt p00 = c0->Value( c0->FirstParameter() );
  gp_Pnt p10 = c0->Value( c0->LastParameter() );

  // B1 is the curve next to C0 which touches C0 at its end point (P01).
  size_t b1_idx = 0;
  for ( size_t k = 1; k < 4; ++k )
  {
    gp_Pnt Pf = curves[k]->Value( curves[k]->FirstParameter() );
    if ( Pf.Distance(p10) < prec )
    {
      b1     = curves[k];
      b1_idx = k;
      break;
    }
  }
  //
  if ( b1.IsNull() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot find b1 curve.");
    return false;
  }

  // P11.
  gp_Pnt p11 = b1->Value( b1->LastParameter() );

  // C1 is the curve touching B1 at its end point and reversed.
  size_t c1_idx = 0;
  for ( size_t k = 1; k < 4; ++k )
  {
    if ( k == b1_idx ) continue;

    gp_Pnt Pf = curves[k]->Value( curves[k]->FirstParameter() );
    if ( Pf.Distance(p11) < prec )
    {
      c1     = Handle(Geom_BSplineCurve)::DownCast( curves[k]->Reversed() );
      c1_idx = k;
      break;
    }
  }
  //
  if ( c1.IsNull() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot find c1 curve.");
    return false;
  }

  // P01.
  gp_Pnt p01 = c1->Value( c1->FirstParameter() );

  // Get b0.
  for ( size_t k = 1; k < 4; ++k )
  {
    if ( k == b1_idx || k == c1_idx ) continue;

    b0 = Handle(Geom_BSplineCurve)::DownCast( curves[k]->Reversed() );
  }
  //
  if ( b0.IsNull() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot find b0 curve.");
    return false;
  }

  // Build Coons patch.
  Handle(Geom_BSplineSurface) coonsSurf;
  if ( !asiAlgo_Utils::FillCoons(c0, c1, b0, b1,
                                 coonsSurf,
                                 m_progress, m_plotter) )
    return false;

  // Set surface.
  surf = coonsSurf;
  patch->SetSurface(surf); // Store in the Data Model.

  return true;
}
