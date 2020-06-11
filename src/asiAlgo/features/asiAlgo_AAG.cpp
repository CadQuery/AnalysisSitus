//-----------------------------------------------------------------------------
// Created on: 26 February 2016
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
#include <asiAlgo_AAG.h>

// asiAlgo includes
#include <asiAlgo_AAGIterator.h>
#include <asiAlgo_CheckDihedralAngle.h>
#include <asiAlgo_FeatureAttrAngle.h>
#include <asiAlgo_FeatureAttrFace.h>
#include <asiAlgo_JSON.h>

// OCCT includes
#include <ShapeAnalysis_Edge.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

//-----------------------------------------------------------------------------

asiAlgo_AAG::asiAlgo_AAG(const TopoDS_Shape&               masterCAD,
                         const TopTools_IndexedMapOfShape& selectedFaces,
                         const bool                        allowSmooth,
                         const double                      smoothAngularTol,
                         const int                         cachedMaps)
{
  this->init(masterCAD,
             selectedFaces,
             allowSmooth,
             smoothAngularTol,
             cachedMaps);
}

//-----------------------------------------------------------------------------

asiAlgo_AAG::asiAlgo_AAG(const TopoDS_Shape& masterCAD,
                         const bool          allowSmooth,
                         const double        smoothAngularTol,
                         const int           cachedMaps)
{
  this->init(masterCAD,
             TopTools_IndexedMapOfShape(),
             allowSmooth,
             smoothAngularTol,
             cachedMaps);
}

//-----------------------------------------------------------------------------

asiAlgo_AAG::~asiAlgo_AAG()
{}

//-----------------------------------------------------------------------------

Handle(asiAlgo_AAG) asiAlgo_AAG::Copy() const
{
  Handle(asiAlgo_AAG) copy = new asiAlgo_AAG;
  //
  copy->m_master            = this->m_master;
  copy->m_selected          = this->m_selected;
  copy->m_subShapes         = this->m_subShapes;
  copy->m_tSubShapes        = this->m_tSubShapes;
  copy->m_faces             = this->m_faces;
  copy->m_tFaces            = this->m_tFaces;
  copy->m_edges             = this->m_edges;
  copy->m_tEdges            = this->m_tEdges;
  copy->m_vertices          = this->m_vertices;
  copy->m_tVertices         = this->m_tVertices;
  copy->m_edgesFaces        = this->m_edgesFaces;
  copy->m_tEdgesFaces       = this->m_tEdgesFaces;
  copy->m_neighborsStack    = this->m_neighborsStack;
  copy->m_arcAttributes     = this->m_arcAttributes;
  copy->m_nodeAttributes    = this->m_nodeAttributes;
  copy->m_bAllowSmooth      = this->m_bAllowSmooth;
  copy->m_fSmoothAngularTol = this->m_fSmoothAngularTol;
  //
  return copy;
}

//-----------------------------------------------------------------------------

void asiAlgo_AAG::PushSubgraph(const asiAlgo_Feature& faces2Keep)
{
  asiAlgo_AdjacencyMx& currentMx = m_neighborsStack.top();

  // Gather all present face indices into a single map.
  asiAlgo_Feature allFaces;
  for ( asiAlgo_AdjacencyMx::t_mx::Iterator it(currentMx.mx); it.More(); it.Next() )
    allFaces.Add( it.Key() );

  // Prepare a collection of face indices to eliminate.
  asiAlgo_Feature face2Exclude;
  face2Exclude.Subtraction(allFaces, faces2Keep);

  // Erase faces.
  this->PushSubgraphX(face2Exclude);
}

//-----------------------------------------------------------------------------

void asiAlgo_AAG::PushSubgraphX(const t_topoId face2Exclude)
{
  asiAlgo_Feature faces2Exclude;
  faces2Exclude.Add(face2Exclude);

  // Erase face.
  this->PushSubgraphX(faces2Exclude);
}

//-----------------------------------------------------------------------------

void asiAlgo_AAG::PushSubgraphX(const asiAlgo_Feature& faces2Exclude)
{
  asiAlgo_AdjacencyMx& currentMx = m_neighborsStack.top();
  asiAlgo_AdjacencyMx subgraphMx(m_alloc);

  // Compose new adjacency matrix.
  for ( asiAlgo_AdjacencyMx::t_mx::Iterator it(currentMx.mx); it.More(); it.Next() )
  {
    const t_topoId fid = it.Key();
    //
    if ( faces2Exclude.Contains(fid) )
      continue;

    asiAlgo_Feature row{ it.Value() };
    row.Subtract(faces2Exclude);

    subgraphMx.mx.Bind(fid, row);
  }

  // Push sub-graph to stack.
  m_neighborsStack.push(subgraphMx);
}

//-----------------------------------------------------------------------------

void asiAlgo_AAG::PopSubgraph()
{
  m_neighborsStack.pop();
}

//-----------------------------------------------------------------------------

void asiAlgo_AAG::PopSubgraphs()
{
  while ( m_neighborsStack.size() != 1 )
    m_neighborsStack.pop();
}

//-----------------------------------------------------------------------------

const TopoDS_Shape& asiAlgo_AAG::GetMasterShape() const
{
  return m_master;
}

//-----------------------------------------------------------------------------

int asiAlgo_AAG::GetNumberOfNodes() const
{
  const asiAlgo_AdjacencyMx& neighborhood = this->GetNeighborhood();
  //
  return neighborhood.mx.Extent();
}

//-----------------------------------------------------------------------------

void asiAlgo_AAG::SetSelectedFaces(const TopTools_IndexedMapOfShape& selectedFaces)
{
  m_selected.Clear();

  // Save selected faces for future filtering.
  for ( t_topoId s = 1; s <= selectedFaces.Extent(); ++s )
    m_selected.Add( m_faces.FindIndex( selectedFaces.FindKey(s) ) );
}

//-----------------------------------------------------------------------------

const asiAlgo_Feature& asiAlgo_AAG::GetSelectedFaces() const
{
  return m_selected;
}

//-----------------------------------------------------------------------------

bool asiAlgo_AAG::HasFace(const t_topoId face_idx) const
{
  return (face_idx > 0) && ( face_idx <= m_faces.Extent() );
}

//-----------------------------------------------------------------------------

bool asiAlgo_AAG::HasFace(const TopoDS_Shape& face) const
{
  return m_faces.Contains(face);
}

//-----------------------------------------------------------------------------

const TopoDS_Face& asiAlgo_AAG::GetFace(const t_topoId face_idx) const
{
  return TopoDS::Face( m_faces.FindKey(face_idx) );
}

//-----------------------------------------------------------------------------

t_topoId asiAlgo_AAG::GetFaceId(const TopoDS_Shape& face) const
{
  return m_faces.FindIndex(face);
}

//-----------------------------------------------------------------------------

bool asiAlgo_AAG::HasNeighbors(const t_topoId face_idx) const
{
  return m_neighborsStack.top().mx.IsBound(face_idx);
}

//-----------------------------------------------------------------------------

const asiAlgo_Feature& asiAlgo_AAG::GetNeighbors(const t_topoId face_idx) const
{
  return m_neighborsStack.top().mx.Find(face_idx);
}

//-----------------------------------------------------------------------------

asiAlgo_Feature
  asiAlgo_AAG::GetNeighborsThru(const t_topoId     face_idx,
                                const TopoDS_Edge& edge)
{
  asiAlgo_Feature result;

  // Get all neighbors of the face of interest
  const asiAlgo_Feature& neighbors = this->GetNeighbors(face_idx);

  // Traverse all neighborhood arcs to see if there are any containing
  // the edge of interest in the list of common edges
  for ( asiAlgo_Feature::Iterator nit(neighbors); nit.More(); nit.Next() )
  {
    const t_topoId neighbor_idx = nit.Key();

    // Get neighborhood attribute
    Handle(asiAlgo_FeatureAttrAdjacency)
      adjAttr = Handle(asiAlgo_FeatureAttrAdjacency)::DownCast( this->GetArcAttribute( t_arc(face_idx, neighbor_idx) ) );
    //
    if ( adjAttr.IsNull() )
      continue;

    // Check the collection of common edges
    const asiAlgo_Feature&
      commonEdgeIndices = adjAttr->GetEdgeIndices();
    //
    const t_topoId edgeIdx = this->RequestMapOfEdges().FindIndex(edge);
    //
    if ( commonEdgeIndices.Contains(edgeIdx) )
      result.Add(neighbor_idx);
  }

  return result;
}

//-----------------------------------------------------------------------------

asiAlgo_Feature
  asiAlgo_AAG::GetNeighborsThru(const t_topoId         face_idx,
                                const asiAlgo_Feature& edge_ids)
{
  asiAlgo_Feature result;

  // Get neighbor faces
  const asiAlgo_Feature& neighbor_ids = this->GetNeighbors(face_idx);
  //
  for ( asiAlgo_Feature::Iterator nit(neighbor_ids); nit.More(); nit.Next() )
  {
    const t_topoId neighbor_idx = nit.Key();

    // Check arc attribute
    Handle(asiAlgo_FeatureAttr) attr = this->GetArcAttribute( t_arc(face_idx, neighbor_idx) );
    //
    if ( !attr->IsKind( STANDARD_TYPE(asiAlgo_FeatureAttrAdjacency) ) )
      continue;

    // Convert to adjacency attribute
    Handle(asiAlgo_FeatureAttrAdjacency)
      adjAttr = Handle(asiAlgo_FeatureAttrAdjacency)::DownCast(attr);
    //
    const asiAlgo_Feature&
      commonEdgeIndices = adjAttr->GetEdgeIndices();

    // Take the index of each edge and check if this edge is of interest
    for ( asiAlgo_Feature::Iterator eit(commonEdgeIndices); eit.More(); eit.Next() )
    {
      const t_topoId eidx = eit.Key();
      //
      if ( edge_ids.Contains(eidx) )
      {
        result.Add(neighbor_idx);
        break;
      }
    }
  }

  return result;
}

//-----------------------------------------------------------------------------

asiAlgo_Feature
  asiAlgo_AAG::GetNeighborsThruX(const t_topoId         face_idx,
                                 const asiAlgo_Feature& xEdges)
{
  asiAlgo_Feature result;

  // Get all neighbors of the face of interest
  const asiAlgo_Feature& neighbors = this->GetNeighbors(face_idx);

  // Traverse all neighborhood arcs to see if there are any containing
  // the edge of interest in the list of common edges
  for ( asiAlgo_Feature::Iterator nit(neighbors); nit.More(); nit.Next() )
  {
    const t_topoId neighbor_idx = nit.Key();

    // Get neighborhood attribute
    Handle(asiAlgo_FeatureAttrAdjacency)
      adjAttr = Handle(asiAlgo_FeatureAttrAdjacency)::DownCast( this->GetArcAttribute( t_arc(face_idx, neighbor_idx) ) );
    //
    if ( adjAttr.IsNull() )
      continue;

    // Check the collection of common edges
    asiAlgo_Feature commonEdgeIndices = adjAttr->GetEdgeIndices();

    // Subtract the restricted edges
    commonEdgeIndices.Subtract(xEdges);

    // If any edges remain, the neighbor face is added to the result
    if ( !commonEdgeIndices.IsEmpty() )
      result.Add(neighbor_idx);
  }

  return result;
}

//-----------------------------------------------------------------------------

const asiAlgo_AdjacencyMx& asiAlgo_AAG::GetNeighborhood() const
{
  return m_neighborsStack.top();
}

//-----------------------------------------------------------------------------

const TopTools_IndexedMapOfShape& asiAlgo_AAG::GetMapOfFaces() const
{
  return m_faces;
}

//-----------------------------------------------------------------------------

const asiAlgo_IndexedMapOfTShape& asiAlgo_AAG::RequestTMapOfFaces()
{
  if ( m_tFaces.IsEmpty() )
    asiAlgo_Utils::MapTShapes(m_master, TopAbs_FACE, m_tFaces);

  return m_tFaces;
}

//-----------------------------------------------------------------------------

const TopTools_IndexedMapOfShape& asiAlgo_AAG::RequestMapOfEdges()
{
  if ( m_edges.IsEmpty() )
    TopExp::MapShapes(m_master, TopAbs_EDGE, m_edges);

  return m_edges;
}

//-----------------------------------------------------------------------------

const asiAlgo_IndexedMapOfTShape& asiAlgo_AAG::RequestTMapOfEdges()
{
  if ( m_tEdges.IsEmpty() )
    asiAlgo_Utils::MapTShapes(m_master, TopAbs_EDGE, m_tEdges);

  return m_tEdges;
}

//-----------------------------------------------------------------------------

const TopTools_IndexedMapOfShape& asiAlgo_AAG::RequestMapOfVertices()
{
  if ( m_vertices.IsEmpty() )
    TopExp::MapShapes(m_master, TopAbs_VERTEX, m_vertices);

  return m_vertices;
}

//-----------------------------------------------------------------------------

const asiAlgo_IndexedMapOfTShape& asiAlgo_AAG::RequestTMapOfVertices()
{
  if ( m_tVertices.IsEmpty() )
    asiAlgo_Utils::MapTShapes(m_master, TopAbs_VERTEX, m_tVertices);

  return m_tVertices;
}

//-----------------------------------------------------------------------------

const TopTools_IndexedMapOfShape& asiAlgo_AAG::RequestMapOfSubShapes()
{
  if ( m_subShapes.IsEmpty() )
    TopExp::MapShapes(m_master, m_subShapes);

  return m_subShapes;
}

//-----------------------------------------------------------------------------

const asiAlgo_IndexedMapOfTShape& asiAlgo_AAG::RequestTMapOfSubShapes()
{
  if ( m_tSubShapes.IsEmpty() )
    asiAlgo_Utils::MapTShapes(m_master, m_tSubShapes);

  return m_tSubShapes;
}

//-----------------------------------------------------------------------------

void asiAlgo_AAG::RequestMapOf(const TopAbs_ShapeEnum      ssType,
                               TopTools_IndexedMapOfShape& map)
{
  switch ( ssType )
  {
    case TopAbs_VERTEX:
      map = this->RequestMapOfVertices();
      break;
    case TopAbs_EDGE:
      map = this->RequestMapOfEdges();
      break;
    case TopAbs_FACE:
      map = this->GetMapOfFaces();
      break;
    default: break;
  }
}

//-----------------------------------------------------------------------------

void asiAlgo_AAG::RequestTMapOf(const TopAbs_ShapeEnum      ssType,
                                asiAlgo_IndexedMapOfTShape& map)
{
  switch ( ssType )
  {
    case TopAbs_VERTEX:
      map = this->RequestTMapOfVertices();
      break;
    case TopAbs_EDGE:
      map = this->RequestTMapOfEdges();
      break;
    case TopAbs_FACE:
      map = this->RequestTMapOfFaces();
      break;
    default: break;
  }
}

//-----------------------------------------------------------------------------

const TopTools_IndexedDataMapOfShapeListOfShape&
  asiAlgo_AAG::RequestMapOfVerticesEdges()
{
  if ( m_verticesEdges.IsEmpty() )
    TopExp::MapShapesAndAncestors(m_master, TopAbs_VERTEX, TopAbs_EDGE, m_edgesFaces);

  return m_verticesEdges;
}

//-----------------------------------------------------------------------------

const asiAlgo_IndexedDataMapOfTShapeListOfShape&
  asiAlgo_AAG::RequestTMapOfVerticesEdges()
{
  if ( m_tVerticesEdges.IsEmpty() )
    asiAlgo_Utils::MapTShapesAndAncestors(m_master, TopAbs_VERTEX, TopAbs_EDGE, m_tEdgesFaces);

  return m_tVerticesEdges;
}

//-----------------------------------------------------------------------------

const TopTools_IndexedDataMapOfShapeListOfShape&
  asiAlgo_AAG::RequestMapOfEdgesFaces()
{
  if ( m_edgesFaces.IsEmpty() )
    TopExp::MapShapesAndAncestors(m_master, TopAbs_EDGE, TopAbs_FACE, m_edgesFaces);

  return m_edgesFaces;
}

//-----------------------------------------------------------------------------

const asiAlgo_IndexedDataMapOfTShapeListOfShape&
  asiAlgo_AAG::RequestTMapOfEdgesFaces()
{
  if ( m_tEdgesFaces.IsEmpty() )
    asiAlgo_Utils::MapTShapesAndAncestors(m_master, TopAbs_EDGE, TopAbs_FACE, m_tEdgesFaces);

  return m_tEdgesFaces;
}

//-----------------------------------------------------------------------------

TopoDS_Shape asiAlgo_AAG::FindSubShapeByAddr(const std::string& addr)
{
  // Get all subshapes.
  const TopTools_IndexedMapOfShape& subShapes = this->RequestMapOfSubShapes();

  // Loop to find the one with the requested address.
  for ( TopTools_IndexedMapOfShape::Iterator ssit(subShapes); ssit.More(); ssit.Next() )
  {
    const TopoDS_Shape& current = ssit.Value();

    // Compare.
    std::string currentAddr = asiAlgo_Utils::ShapeAddr(current);
    //
    if ( currentAddr == addr )
      return current;
  }

  return TopoDS_Shape();
}

//-----------------------------------------------------------------------------

bool asiAlgo_AAG::HasArc(const t_arc& arc) const
{
  const asiAlgo_AdjacencyMx& mx = m_neighborsStack.top();

  // Seek for adjacency record.
  const asiAlgo_Feature* pRow = mx.mx.Seek(arc.F1);
  //
  if ( !pRow ) return false;

  return pRow->Contains(arc.F2);
}

//-----------------------------------------------------------------------------

const asiAlgo_AAG::t_arc_attributes&
  asiAlgo_AAG::GetArcAttributes() const
{
  return m_arcAttributes;
}

//-----------------------------------------------------------------------------

const Handle(asiAlgo_FeatureAttr)&
  asiAlgo_AAG::GetArcAttribute(const t_arc& arc) const
{
  return m_arcAttributes.Find(arc);
}

//-----------------------------------------------------------------------------

bool asiAlgo_AAG::HasNodeAttributes(const t_topoId node) const
{
  return m_nodeAttributes.IsBound(node);
}

//-----------------------------------------------------------------------------

const asiAlgo_AAG::t_node_attributes& asiAlgo_AAG::GetNodeAttributes() const
{
  return m_nodeAttributes;
}

//-----------------------------------------------------------------------------

const asiAlgo_AAG::t_attr_set&
  asiAlgo_AAG::GetNodeAttributes(const t_topoId node) const
{
  return m_nodeAttributes(node);
}

//-----------------------------------------------------------------------------

Handle(asiAlgo_FeatureAttr)
  asiAlgo_AAG::GetNodeAttribute(const t_topoId       node,
                                const Standard_GUID& attr_id) const
{
  const t_attr_set* attrSetPtr = m_nodeAttributes.Seek(node);
  if ( attrSetPtr == nullptr )
    return nullptr;

  const Handle(asiAlgo_FeatureAttr)* attrPtr = (*attrSetPtr).Seek(attr_id);
  if ( attrPtr == nullptr )
    return nullptr;

  return (*attrPtr);
}

//-----------------------------------------------------------------------------

bool asiAlgo_AAG::RemoveNodeAttribute(const t_topoId       node,
                                      const Standard_GUID& attr_id)
{
  t_attr_set* attrSetPtr = m_nodeAttributes.ChangeSeek(node);
  if ( attrSetPtr == nullptr )
    return false;

  const Handle(asiAlgo_FeatureAttr)* attrPtr = (*attrSetPtr).Seek(attr_id);
  if ( attrPtr == nullptr )
    return false;

  return (*attrSetPtr).ChangeMap().UnBind(attr_id);
}

//-----------------------------------------------------------------------------

void asiAlgo_AAG::RemoveNodeAttributes()
{
  m_nodeAttributes.Clear();
}

//-----------------------------------------------------------------------------

void asiAlgo_AAG::SetNodeAttributes(const t_node_attributes& attrs)
{
  m_nodeAttributes = attrs;
}

//-----------------------------------------------------------------------------

bool asiAlgo_AAG::SetNodeAttribute(const t_topoId                     node,
                                   const Handle(asiAlgo_FeatureAttr)& attr)
{
  if ( attr.IsNull() )
    return false;

  Handle(asiAlgo_FeatureAttr) existing = this->GetNodeAttribute( node, attr->GetGUID() );
  //
  if ( !existing.IsNull() )
    return false; // Already there

  // Set owner AAG
  attr->setAAG(this);

  // Set face ID to the attribute representing a feature face
  if ( attr->IsKind( STANDARD_TYPE(asiAlgo_FeatureAttrFace) ) )
    Handle(asiAlgo_FeatureAttrFace)::DownCast(attr)->SetFaceId(node);

  t_attr_set* attrSetPtr = m_nodeAttributes.ChangeSeek(node);
  if ( attrSetPtr == nullptr )
    m_nodeAttributes.Bind( node, t_attr_set(attr) );
  else
    (*attrSetPtr).Add(attr);

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_AAG::FindBaseOnly(asiAlgo_Feature& resultFaceIds) const
{
  for ( asiAlgo_AdjacencyMx::t_mx::Iterator it( m_neighborsStack.top().mx );
        it.More(); it.Next() )
  {
    const t_topoId fid = it.Key();

    // Get face to check the number of wires.
    const TopoDS_Face& face = this->GetFace(fid);

    // Get the number of wires.
    TopTools_IndexedMapOfShape wires;
    TopExp::MapShapes(face, TopAbs_WIRE, wires);
    //
    const int numWires = wires.Extent();

    if ( numWires > 1 )
      resultFaceIds.Add(fid);
  }

  return resultFaceIds.Extent() > 0;
}

//-----------------------------------------------------------------------------

bool asiAlgo_AAG::FindConvexOnly(asiAlgo_Feature& resultFaceIds) const
{
  asiAlgo_Feature traversed;
  for ( asiAlgo_AdjacencyMx::t_mx::Iterator it( m_neighborsStack.top().mx );
        it.More(); it.Next() )
  {
    const t_topoId         current_face_idx       = it.Key();
    const asiAlgo_Feature& current_face_neighbors = it.Value();

    // Mark face as traversed.
    if ( !traversed.Contains(current_face_idx) )
      traversed.Add(current_face_idx);
    else
      continue;

    // Loop over the neighbors.
    bool isAllConvex = true;
    for ( asiAlgo_Feature::Iterator nit(current_face_neighbors); nit.More(); nit.Next() )
    {
      const t_topoId neighbor_face_idx = nit.Key();

      // Get angle attribute
      Handle(asiAlgo_FeatureAttrAngle)
        attr = Handle(asiAlgo_FeatureAttrAngle)::DownCast( this->GetArcAttribute( t_arc(current_face_idx,
                                                                                        neighbor_face_idx) ) );

      if ( attr->GetAngleType() != FeatureAngleType_Convex &&
           attr->GetAngleType() != FeatureAngleType_SmoothConvex )
      {
        isAllConvex = false;

        // Mark face as traversed as we don't want to check concave neighbors
        traversed.Add(neighbor_face_idx);
      }
    }

    if ( isAllConvex )
      resultFaceIds.Add(current_face_idx);
  }

  return resultFaceIds.Extent() > 0;
}

//-----------------------------------------------------------------------------

bool asiAlgo_AAG::FindConvexOnly(TopTools_IndexedMapOfShape& resultFaces) const
{
  asiAlgo_Feature resultFaceIds;
  //
  if ( !this->FindConvexOnly(resultFaceIds) )
    return false;

  for ( asiAlgo_Feature::Iterator fit(resultFaceIds); fit.More(); fit.Next() )
    resultFaces.Add( this->GetFace( fit.Key() ) );

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_AAG::FindConcaveOnly(asiAlgo_Feature& resultFaceIds) const
{
  asiAlgo_Feature traversed;
  for ( asiAlgo_AdjacencyMx::t_mx::Iterator it( m_neighborsStack.top().mx );
        it.More(); it.Next() )
  {
    const t_topoId         current_face_idx       = it.Key();
    const asiAlgo_Feature& current_face_neighbors = it.Value();

    // Mark face as traversed
    if ( !traversed.Contains(current_face_idx) )
      traversed.Add(current_face_idx);
    else
      continue;

    // Loop over the neighbors
    bool isAllConcave = true;
    for ( asiAlgo_Feature::Iterator nit(current_face_neighbors); nit.More(); nit.Next() )
    {
      const t_topoId neighbor_face_idx = nit.Key();

      // Get angle attribute
      Handle(asiAlgo_FeatureAttrAngle)
        attr = Handle(asiAlgo_FeatureAttrAngle)::DownCast( this->GetArcAttribute( t_arc(current_face_idx,
                                                                                        neighbor_face_idx) ) );

      if ( attr->GetAngleType() != FeatureAngleType_Concave &&
           attr->GetAngleType() != FeatureAngleType_SmoothConcave )
      {
        isAllConcave = false;

        // Mark face as traversed as we don't want to check concave neighbors
        traversed.Add(neighbor_face_idx);
      }
    }

    if ( isAllConcave )
      resultFaceIds.Add(current_face_idx);
  }

  return resultFaceIds.Extent() > 0;
}

//-----------------------------------------------------------------------------

bool asiAlgo_AAG::FindConcaveOnly(TopTools_IndexedMapOfShape& resultFaces) const
{
  asiAlgo_Feature resultFaceIds;
  //
  if ( !this->FindConcaveOnly(resultFaceIds) )
    return false;

  for ( asiAlgo_Feature::Iterator fit(resultFaceIds); fit.More(); fit.Next() )
    resultFaces.Add( this->GetFace( fit.Key() ) );

  return true;
}

//-----------------------------------------------------------------------------

void asiAlgo_AAG::Remove(const TopTools_IndexedMapOfShape& faces)
{
  // NOTICE: indexed map of shapes is not affected as we want to keep
  //         using the original indices of faces

  // Find IDs of the faces to remove
  asiAlgo_Feature toRemove;
  for ( t_topoId f = 1; f <= faces.Extent(); ++f )
  {
    const t_topoId face_idx = this->GetFaceId( faces.FindKey(f) );
    toRemove.Add(face_idx);
  }

  // Remove by indices
  this->Remove(toRemove);
}

//-----------------------------------------------------------------------------

void asiAlgo_AAG::Remove(const asiAlgo_Feature& faceIndices)
{
  // NOTICE: indexed map of shapes is not affected as we want to keep
  //         using the original indices of faces

  // Loop over the target faces
  for ( asiAlgo_Feature::Iterator fit(faceIndices); fit.More(); fit.Next() )
  {
    const t_topoId face_idx = fit.Key();

    // Unbind node attributes
    m_nodeAttributes.UnBind(face_idx);

    // Find all neighbors
    const asiAlgo_Feature& neighbor_indices = m_neighborsStack.top().mx.Find(face_idx);
    for ( asiAlgo_Feature::Iterator nit(neighbor_indices); nit.More(); nit.Next() )
    {
      const t_topoId neighbor_idx = nit.Key();

      // Unbind arc attributes
      m_arcAttributes.UnBind( t_arc(face_idx, neighbor_idx) );

      // Kill the corresponding chunks from the list of neighbors
      asiAlgo_Feature* mapPtr = m_neighborsStack.top().mx.ChangeSeek(neighbor_idx);
      if ( mapPtr != nullptr )
        (*mapPtr).Subtract(faceIndices);
    }

    // Unbind node
    m_neighborsStack.top().mx.UnBind(face_idx);
  }
}

//-----------------------------------------------------------------------------

int asiAlgo_AAG::GetConnectedComponentsNb()
{
  std::vector<asiAlgo_Feature> ccomps;
  this->GetConnectedComponents(ccomps);

  return int( ccomps.size() );
}

//-----------------------------------------------------------------------------

int asiAlgo_AAG::GetConnectedComponentsNb(const asiAlgo_Feature& excludedFaceIndices)
{
  Handle(asiAlgo_AAG) aagCopy = this->Copy();
  aagCopy->Remove(excludedFaceIndices);
  return aagCopy->GetConnectedComponentsNb();
}

//-----------------------------------------------------------------------------

void asiAlgo_AAG::GetConnectedComponents(const asiAlgo_Feature&        seeds,
                                         std::vector<asiAlgo_Feature>& res)
{
  res.clear();

  Handle(asiAlgo_AAGSetIterator) seed_it = new asiAlgo_AAGSetIterator(this, seeds);
  asiAlgo_Feature traversed;

  for ( ; seed_it->More() ; seed_it->Next() )
  {
    // Get seed face
    const t_topoId seed_face_id = seed_it->GetFaceId();

    if ( traversed.Contains(seed_face_id) )
      continue; // Skip checked nodes

    traversed.Add(seed_face_id);
    res.push_back( asiAlgo_Feature() );
    res.back().Add(seed_face_id);

    // Width-first search
    asiAlgo_Feature seed_neighbor_ids = this->GetNeighbors(seed_face_id);
    asiAlgo_Feature seed_neighbor_next_iter;

    do
    {
      seed_neighbor_next_iter.Clear();

      for ( asiAlgo_Feature::Iterator nit(seed_neighbor_ids); nit.More(); nit.Next() )
      {
        const t_topoId  seed_face_id_new       = nit.Key();
        asiAlgo_Feature seed_neighbor_ids_cand = this->GetNeighbors(seed_face_id_new);

        if ( !seeds.Contains(seed_face_id_new) )
          continue; // Skip

        traversed.Add(seed_face_id_new);

        // Set faces for the next iteration
        seed_neighbor_ids_cand.Subtract(traversed);
        seed_neighbor_ids_cand.Intersect(seeds);
        seed_neighbor_next_iter.Unite(seed_neighbor_ids_cand);
        res.back().Add(seed_face_id_new);
      }

      seed_neighbor_ids = seed_neighbor_next_iter;
    }
    while ( seed_neighbor_ids.Extent() != 0 );
  }
}

//-----------------------------------------------------------------------------

void asiAlgo_AAG::ClearCache()
{
  m_faces.Clear();
  m_edges.Clear();
  m_vertices.Clear();
  m_subShapes.Clear();
  m_edgesFaces.Clear();
}

//-----------------------------------------------------------------------------

void asiAlgo_AAG::GetConnectedComponents(std::vector<asiAlgo_Feature>& res)
{
  // Gather all present face indices into a single map.
  asiAlgo_Feature allFaces;
  for ( asiAlgo_AdjacencyMx::t_mx::Iterator it( m_neighborsStack.top().mx );
        it.More(); it.Next() )
  {
    const t_topoId face = it.Key();
    //
    allFaces.Add(face);
  }

  // Collect connected components using all faces as seeds.
  this->GetConnectedComponents(allFaces, res);
}

//-----------------------------------------------------------------------------

void asiAlgo_AAG::Dump(Standard_OStream& out) const
{
  out << "===================================================\n";
  out << "***AAG structure\n";
  out << "---------------------------------------------------\n";
  out << " Adjacency\n";
  out << "---------------------------------------------------\n";

  // Dump neighborhood
  for ( t_topoId f = 1; f <= m_faces.Extent(); ++f )
  {
    out << "\t" << f << " -> ";
    const asiAlgo_Feature& neighbors = this->GetNeighbors(f);
    //
    for ( asiAlgo_Feature::Iterator nit(neighbors); nit.More(); nit.Next() )
    {
      out << nit.Key() << " ";
    }
    out << "\n";
  }

  // Dump arc attributes
  out << "---------------------------------------------------\n";
  out << " Node attributes\n";
  out << "---------------------------------------------------\n";
  for ( t_topoId f = 1; f <= m_faces.Extent(); ++f )
  {
    if ( !this->HasNodeAttributes(f) )
      continue;

    const t_attrMap& attrs = this->GetNodeAttributes(f).GetMap();
    //
    if ( attrs.IsEmpty() )
      continue;

    out << "\t" << f << " ~ ";
    //
    for ( t_attrMap::Iterator ait(attrs); ait.More(); ait.Next() )
    {
      out << "[" << ait.Value()->DynamicType()->Name() << "]\n";
      out << ">>>\n";
      ait.Value()->Dump(out);
      out << "\n<<<\n";
    }
    out << "\n";
  }
  out << "===================================================\n";
}

//-----------------------------------------------------------------------------

void asiAlgo_AAG::DumpJSON(Standard_OStream& out,
                           const int         whitespaces) const
{
  std::string prefix(whitespaces, ' ');

  out << std::setprecision( std::numeric_limits<double>::max_digits10 );
  out << prefix << "{";
  out << "\n" << prefix << "  \"nodes\": {";
  //
  this->dumpNodesJSON(out, whitespaces);
  //
  out << "\n" << prefix << "  },"; // End 'nodes'.
  //
  out << "\n" << prefix << "  \"arcs\": [";
  //
  this->dumpArcsJSON(out, whitespaces);
  //
  out << "\n" << prefix << "  ]"; // End 'arcs'.
  out << "\n" << prefix << "}";
}

//-----------------------------------------------------------------------------

void asiAlgo_AAG::init(const TopoDS_Shape&               masterCAD,
                       const TopTools_IndexedMapOfShape& selectedFaces,
                       const bool                        allowSmooth,
                       const double                      smoothAngularTol,
                       const int                         cachedMaps)
{
  // Prepare allocator.
  m_alloc = new NCollection_IncAllocator;

  // Set basic props.
  m_master            = masterCAD;
  m_bAllowSmooth      = allowSmooth;
  m_fSmoothAngularTol = smoothAngularTol;

  //---------------------------------------------------------------------------

  // Put main adjacency matrix to the stack of graph states.
  m_neighborsStack.push( asiAlgo_AdjacencyMx(m_alloc) );

  //---------------------------------------------------------------------------

  // Extract all sub-shapes with unique indices from the master CAD.
  if ( cachedMaps & CachedMap_SubShapes )
    TopExp::MapShapes(masterCAD, m_subShapes);

  // Extract all faces with unique indices from the master CAD.
  if ( cachedMaps & CachedMap_Faces )
    TopExp::MapShapes(masterCAD, TopAbs_FACE, m_faces);

  // Extract all edges with unique indices from the master CAD.
  if ( cachedMaps & CachedMap_Edges )
    TopExp::MapShapes(masterCAD, TopAbs_EDGE, m_edges);

  // Extract all vertices with unique indices from the master CAD.
  if ( cachedMaps & CachedMap_Vertices )
    TopExp::MapShapes(masterCAD, TopAbs_VERTEX, m_vertices);

  // Build child-parent map for edges and their faces.
  if ( cachedMaps & CachedMap_EdgesFaces )
    TopExp::MapShapesAndAncestors(masterCAD, TopAbs_EDGE, TopAbs_FACE, m_edgesFaces);

  ShapeAnalysis_Edge sae;

  // Fill adjacency map with empty buckets and provide all required
  // treatment for each individual face.
  for ( t_topoId f = 1; f <= m_faces.Extent(); ++f )
  {
    m_neighborsStack.top().mx.Bind( f, asiAlgo_Feature() );
    //
    const TopoDS_Face& face = TopoDS::Face( m_faces(f) );

    // Special treatment deserve those faces having seam edges. Such faces
    // get their own attributes.
    for ( TopExp_Explorer exp(face, TopAbs_EDGE); exp.More(); exp.Next() )
    {
      const TopoDS_Edge& edge = TopoDS::Edge( exp.Current() );
      //
      if ( sae.IsSeam(edge, face) )
      {
        TopTools_IndexedMapOfShape edges;
        asiAlgo_CheckDihedralAngle checkDihAngle(nullptr, nullptr);

        // Notice that smooth transitions are not allowed here. This is because
        // the following treatment is designed for periodic faces, and we normally
        // have self-transition of quality C1 and better there.
        double angRad = 0.0;
        //
        const asiAlgo_FeatureAngleType
          angType = checkDihAngle.AngleBetweenFaces(face, face, false, 0.0, edges, angRad);

        // Bind attribute representing the type of dihedral angle. This is an
        // exceptional case as normally such attributes are bound to arcs.
        m_nodeAttributes.Bind( f, t_attr_set( new asiAlgo_FeatureAttrAngle(angType, angRad) ) );
      }
    }
  }

  //---------------------------------------------------------------------------

  TopTools_IndexedDataMapOfShapeListOfShape ChildParentMap;
  TopExp::MapShapesAndAncestors(masterCAD, TopAbs_EDGE, TopAbs_FACE, ChildParentMap);

  // Build adjacency graph
  for ( TopExp_Explorer exp(masterCAD, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    const TopoDS_Edge&          commonEdge    = TopoDS::Edge( exp.Current() );
    const TopTools_ListOfShape& adjacentFaces = ChildParentMap.FindFromKey(commonEdge);
    //
    this->addMates(adjacentFaces);
  }

  // Set selected faces
  this->SetSelectedFaces(selectedFaces);
}

//-----------------------------------------------------------------------------

void asiAlgo_AAG::addMates(const TopTools_ListOfShape& mateFaces)
{
  // Prepare dihedral angle calculation.
  asiAlgo_CheckDihedralAngle checkDihAngle(nullptr, nullptr);

  // Now analyze the face pairs
  for ( TopTools_ListIteratorOfListOfShape lit(mateFaces); lit.More(); lit.Next() )
  {
    const t_topoId     face_idx   = m_faces.FindIndex( lit.Value() );
    asiAlgo_Feature&   face_links = m_neighborsStack.top().mx.ChangeFind(face_idx);
    const TopoDS_Face& face       = TopoDS::Face( m_faces.FindKey(face_idx) );

    // Add all the rest faces as neighbors.
    for ( TopTools_ListIteratorOfListOfShape lit2(mateFaces); lit2.More(); lit2.Next() )
    {
      const t_topoId linked_face_idx = m_faces.FindIndex( lit2.Value() );

      if ( linked_face_idx == face_idx )
        continue; // Skip the same index to avoid loop arcs in the graph.

      if ( face_links.Contains(linked_face_idx) )
        continue;

      face_links.Add(linked_face_idx);

      // The graph is not oriented, so we do not want to compute arc
      // attribute G-F is previously we have already done F-G attribution.
      t_arc arc(face_idx, linked_face_idx);
      if ( m_arcAttributes.IsBound(arc) )
        continue;

      //-----------------------------------------------------------------------
      // Associate attributes
      //-----------------------------------------------------------------------

      const TopoDS_Face& linked_face = TopoDS::Face( m_faces.FindKey(linked_face_idx) );
      //
      TopTools_IndexedMapOfShape commonEdges;

      // Here we let client code decide whether to allow smooth transitions
      // or not. Smooth transition normally requires additional processing
      // in order to classify feature angle as concave or convex.
      double angRad = 0.0;
      //
      const asiAlgo_FeatureAngleType
        angle = checkDihAngle.AngleBetweenFaces(face,
                                                linked_face,
                                                m_bAllowSmooth,
                                                m_fSmoothAngularTol,
                                                commonEdges,
                                                angRad);

      // Convert transient edge pointers to a collection of indices
      asiAlgo_Feature commonEdgeIndices;
      //
      for ( t_topoId eidx = 1; eidx <= commonEdges.Extent(); ++eidx )
      {
        const t_topoId
          globalEdgeIdx = this->RequestMapOfEdges().FindIndex( commonEdges(eidx) );
        //
        commonEdgeIndices.Add(globalEdgeIdx);
      }

      // Create attribute
      Handle(asiAlgo_FeatureAttr)
        attrAngle = new asiAlgo_FeatureAttrAngle(angle, angRad, commonEdgeIndices);

      // Set owner
      attrAngle->setAAG(this);

      // Bind
      m_arcAttributes.Bind(arc, attrAngle);
    }
  }
}

//-----------------------------------------------------------------------------

void asiAlgo_AAG::dumpNodesJSON(Standard_OStream& out,
                                const int         whitespaces) const
{
  int nidx = 0;
  //
  for ( asiAlgo_AdjacencyMx::t_mx::Iterator nit( m_neighborsStack.top().mx );
        nit.More(); nit.Next(), ++nidx )
  {
    const t_topoId nodeId = nit.Key();
    //
    this->dumpNodeJSON(nodeId, nidx == 0, out, whitespaces);
  }
}

//-----------------------------------------------------------------------------

void asiAlgo_AAG::dumpNodeJSON(const t_topoId    node,
                               const bool        isFirst,
                               Standard_OStream& out,
                               const int         whitespaces) const
{
  std::string prefix(whitespaces, ' ');

  // One attribute which should always be dumped is the surface type.
  std::string
    surfName = asiAlgo_Utils::SurfaceName( BRep_Tool::Surface( this->GetFace(node) ) );

  if ( !isFirst )
    out << ",";
  //
  out << "\n" << prefix << "    \"" << node << "\": {";
  out << "\n" << prefix << "      \"surface\": \"" << surfName << "\"";
  //
  if ( this->HasNodeAttributes(node) )
  {
    out << ",\n" << prefix << "      \"attributes\": [";

    // Dump attributes.
    const t_attr_set& attrs = this->GetNodeAttributes(node);
    //
    int attridx = 0;
    //
    for ( t_attr_set::Iterator ait(attrs); ait.More(); ait.Next(), ++attridx )
    {
      if ( attridx != 0 )
        out << ",";

      ait.GetAttr()->DumpJSON(out, 8 + whitespaces);
    }

    out << "\n" << prefix << "      ]";
  }
  //
  out << "\n" << prefix << "    }";
}

//-----------------------------------------------------------------------------

void asiAlgo_AAG::dumpArcsJSON(Standard_OStream& out,
                               const int         whitespaces) const
{
  // Map to filter out the already visited arcs.
  NCollection_Map<t_arc, t_arc> visited;

  int arcidx = 0;
  //
  for ( asiAlgo_AdjacencyMx::t_mx::Iterator it( m_neighborsStack.top().mx );
        it.More(); it.Next() )
  {
    const t_topoId f_idx = it.Key();

    // Get neighbors.
    const asiAlgo_Feature& localNeighbors = it.Value();

    // Dump arc for each neighbor.
    for ( asiAlgo_Feature::Iterator mit(localNeighbors); mit.More(); mit.Next(), ++arcidx )
    {
      const t_topoId neighbor_f_idx = mit.Key();

      // Check if the arc was not traversed before.
      t_arc arc(f_idx, neighbor_f_idx);
      //
      if ( visited.Contains(arc) )
        continue;
      //
      visited.Add(arc);

      // Dump arc.
      this->dumpArcJSON(arc, arcidx == 0, out, whitespaces);
    }
  }
}

//-----------------------------------------------------------------------------

void asiAlgo_AAG::dumpArcJSON(const t_arc&      arc,
                              const bool        isFirst,
                              Standard_OStream& out,
                              const int         whitespaces) const
{
  std::string prefix(whitespaces, ' ');

  Handle(asiAlgo_FeatureAttr) arcAttr = this->GetArcAttribute(arc);
  //
  Handle(asiAlgo_FeatureAttrAngle)
    arcAttrAngle = Handle(asiAlgo_FeatureAttrAngle)::DownCast(arcAttr);

  // Prepare a label for the angle type.
  std::string angleTypeStr;
  //
  if ( arcAttrAngle->GetAngleType() == FeatureAngleType_Convex )
    angleTypeStr = "convex";
  else if ( arcAttrAngle->GetAngleType() == FeatureAngleType_Concave )
    angleTypeStr = "concave";
  else if ( arcAttrAngle->GetAngleType() == FeatureAngleType_Smooth )
    angleTypeStr = "smooth";
  else if ( arcAttrAngle->GetAngleType() == FeatureAngleType_SmoothConcave )
    angleTypeStr = "smooth concave";
  else if ( arcAttrAngle->GetAngleType() == FeatureAngleType_SmoothConvex )
    angleTypeStr = "smooth convex";
  else
    angleTypeStr = "undefined";

  // Prepare a label for the angle value (degrees).
  std::string angleDegStr = asiAlgo_Utils::Str::ToString<double>(arcAttrAngle->GetAngleRad() * 180. / M_PI);

  // Dump to the stream.
  if ( !isFirst )
    out << ",";
  //
  out << "\n" << prefix << "    [\"" << arc.F1 << "\", \""
                                     << arc.F2 << "\", \""
                                     << angleTypeStr << "\", "
                                     << angleDegStr << "]";
}
