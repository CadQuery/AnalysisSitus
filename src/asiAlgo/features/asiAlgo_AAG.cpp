//-----------------------------------------------------------------------------
// Created on: 26 February 2016
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
#include <asiAlgo_AAG.h>

// Geometry includes
#include <asiAlgo_FeatureAttrAngle.h>
#include <asiAlgo_DihedralAngle.h>

// OCCT includes
#include <ShapeAnalysis_Edge.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

//-----------------------------------------------------------------------------

//! Initializes AAG from the    given master model and selected faces.
//! \param[in] masterCAD        master model (full CAD).
//! \param[in] selectedFaces    selected faces.
//! \param[in] allowSmooth      indicates whether to allow "smooth" value for
//!                             arc attribute. This value means that the
//!                             joint between faces is at least G1, so in
//!                             order to calculate dihedral angle, the neighborhood
//!                             of transition has to be analyzed. The latter
//!                             analysis introduces additional cost, so you
//!                             may disable it if you are Ok to attribute G0
//!                             joints only.
//! \param[in] smoothAngularTol angular tolerance used for recognition
//!                             of smooth dihedral angles. A smooth angle
//!                             may appear to be imperfect by construction,
//!                             but still smooth by the design intent. With
//!                             this parameter you're able to control it.
asiAlgo_AAG::asiAlgo_AAG(const TopoDS_Shape&               masterCAD,
                         const TopTools_IndexedMapOfShape& selectedFaces,
                         const bool                        allowSmooth,
                         const double                      smoothAngularTol)
{
  this->init(masterCAD, selectedFaces, allowSmooth, smoothAngularTol);
}

//-----------------------------------------------------------------------------

//! Constructor accepting master CAD only.
//! \param[in] masterCAD        master CAD.
//! \param[in] allowSmooth      indicates whether "smooth" attribution for arcs
//!                             is allowed (true) or not (false).
//! \param[in] smoothAngularTol angular tolerance used for recognition
//!                             of smooth dihedral angles. A smooth angle
//!                             may appear to be imperfect by construction,
//!                             but still smooth by the design intent. With
//!                             this parameter you're able to control it.
asiAlgo_AAG::asiAlgo_AAG(const TopoDS_Shape& masterCAD,
                         const bool          allowSmooth,
                         const double        smoothAngularTol)
{
  this->init(masterCAD, TopTools_IndexedMapOfShape(), allowSmooth, smoothAngularTol);
}

//-----------------------------------------------------------------------------

//! Destructor.
asiAlgo_AAG::~asiAlgo_AAG()
{}

//-----------------------------------------------------------------------------

//! \return copy of this AAG.
Handle(asiAlgo_AAG) asiAlgo_AAG::Copy() const
{
  Handle(asiAlgo_AAG) copy = new asiAlgo_AAG;
  //
  copy->m_master            = this->m_master;
  copy->m_selected          = this->m_selected;
  copy->m_subShapes         = this->m_subShapes;
  copy->m_faces             = this->m_faces;
  copy->m_edges             = this->m_edges;
  copy->m_vertices          = this->m_vertices;
  copy->m_neighbors         = this->m_neighbors;
  copy->m_arc_attributes    = this->m_arc_attributes;
  copy->m_node_attributes   = this->m_node_attributes;
  copy->m_bAllowSmooth      = this->m_bAllowSmooth;
  copy->m_fSmoothAngularTol = this->m_fSmoothAngularTol;
  //
  return copy;
}

//-----------------------------------------------------------------------------

//! \return master CAD.
const TopoDS_Shape& asiAlgo_AAG::GetMasterCAD() const
{
  return m_master;
}

//-----------------------------------------------------------------------------

//! \return map of faces.
const TopTools_IndexedMapOfShape& asiAlgo_AAG::GetMapOfFaces() const
{
  return m_faces;
}

//-----------------------------------------------------------------------------

//! \return map of edges.
const TopTools_IndexedMapOfShape& asiAlgo_AAG::GetMapOfEdges() const
{
  return m_edges;
}

//-----------------------------------------------------------------------------

//! \return map of vertices.
const TopTools_IndexedMapOfShape& asiAlgo_AAG::GetMapOfVertices() const
{
  return m_vertices;
}

//-----------------------------------------------------------------------------

//! \return map of sub-shapes.
const TopTools_IndexedMapOfShape& asiAlgo_AAG::GetMapOfSubShapes() const
{
  return m_subShapes;
}

//-----------------------------------------------------------------------------

//! Returns true if the index is in range.
//! \param[in] face_idx face index.
//! \return true/false.
bool asiAlgo_AAG::HasFace(const int face_idx) const
{
  return face_idx > 0 && face_idx <= m_faces.Extent();
}

//-----------------------------------------------------------------------------

//! Returns topological face by its internal index (e.g. coming from iterator).
//! \param[in] face_idx face index.
//! \return topological face.
const TopoDS_Face& asiAlgo_AAG::GetFace(const int face_idx) const
{
  return TopoDS::Face( m_faces.FindKey(face_idx) );
}

//-----------------------------------------------------------------------------

//! Returns face ID.
//! \param[in] face face of interest.
//! \return face ID.
int asiAlgo_AAG::GetFaceId(const TopoDS_Shape& face) const
{
  return m_faces.FindIndex(face);
}

//-----------------------------------------------------------------------------

//! Checks whether the given face has any neighbors recorded in the AAG.
//! Normally it has, but in some abnormal situations no neighbors could
//! be there.
//! \param[in] face_idx face index.
//! \return true in case if at least one neighbor presents, false -- otherwise.
bool asiAlgo_AAG::HasNeighbors(const int face_idx) const
{
  return m_neighbors.IsBound(face_idx);
}

//-----------------------------------------------------------------------------

//! Returns neighbors for the face having the given internal index.
//! \param[in] face_idx face index.
//! \return indices of the neighbor faces.
const TColStd_PackedMapOfInteger& asiAlgo_AAG::GetNeighbors(const int face_idx) const
{
  return m_neighbors.Find(face_idx);
}

//-----------------------------------------------------------------------------

//! Returns only those neighbor faces which share the given edge with the
//! passed face of interest.
//! \param[in] face_idx ID of the face of interest.
//! \param[in] edge     common edge.
//! \return indices of the neighbor faces sharing the given edge.
TColStd_PackedMapOfInteger
  asiAlgo_AAG::GetNeighborsThru(const int face_idx, const TopoDS_Edge& edge) const
{
  TColStd_PackedMapOfInteger result;

  // Get all neighbors of the face of interest
  const TColStd_PackedMapOfInteger& neighbors = this->GetNeighbors(face_idx);

  // Traverse all neighborhood arcs to see if there are any containing
  // the edge of interest in the list of common edges
  for ( TColStd_MapIteratorOfPackedMapOfInteger nit(neighbors); nit.More(); nit.Next() )
  {
    const int neighbor_idx = nit.Key();

    // Get neighborhood attribute
    Handle(asiAlgo_FeatureAttrAdjacency)
      adjAttr = Handle(asiAlgo_FeatureAttrAdjacency)::DownCast( this->GetArcAttribute( t_arc(face_idx, neighbor_idx) ) );
    //
    if ( adjAttr.IsNull() )
      continue;

    // Check the collection of common edges
    const TopTools_IndexedMapOfShape& commonEdges = adjAttr->GetEdges();
    //
    if ( commonEdges.Contains(edge) )
      result.Add(neighbor_idx);
  }

  return result;
}

//-----------------------------------------------------------------------------

//! Returns neighbor faces for the given face of interest with additional
//! filter on edges realizing the neighborhood.
//! \param[in] face_idx index of the face of interest.
//! \param[in] edge_ids indices of edges of interest.
//! \return indices of the neighbor faces.
TColStd_PackedMapOfInteger
  asiAlgo_AAG::GetNeighbors(const int                         face_idx,
                            const TColStd_PackedMapOfInteger& edge_ids) const
{
  TColStd_PackedMapOfInteger result;

  // Get all edges
  TopTools_IndexedMapOfShape allEdges;
  TopExp::MapShapes(m_master, TopAbs_EDGE, allEdges);

  // Get neighbor faces
  const TColStd_PackedMapOfInteger& neighbor_ids = this->GetNeighbors(face_idx);
  //
  for ( TColStd_MapIteratorOfPackedMapOfInteger nit(neighbor_ids); nit.More(); nit.Next() )
  {
    const int neighbor_idx = nit.Key();

    // Check arc attribute
    Handle(asiAlgo_FeatureAttr) attr = this->GetArcAttribute( t_arc(face_idx, neighbor_idx) );
    if ( !attr->IsKind( STANDARD_TYPE(asiAlgo_FeatureAttrAdjacency) ) )
      continue;

    // Convert to adjacency attribute
    Handle(asiAlgo_FeatureAttrAdjacency)
      adjAttr = Handle(asiAlgo_FeatureAttrAdjacency)::DownCast(attr);
    //
    const TopTools_IndexedMapOfShape& edges = adjAttr->GetEdges();

    // Take the index of each edge and check if this edge is of interest
    for ( int eidx = 1; eidx <= edges.Extent(); ++eidx )
    {
      const int globalIdx = allEdges.FindIndex( edges(eidx) );
      //
      if ( edge_ids.Contains(globalIdx) )
      {
        result.Add(neighbor_idx);
        break;
      }
    }
  }

  return result;
}

//-----------------------------------------------------------------------------

//! Returns full collection of neighbor faces.
//! \return neighborhood data.
const asiAlgo_AAG::t_adjacency& asiAlgo_AAG::GetNeighborhood() const
{
  return m_neighbors;
}

//-----------------------------------------------------------------------------

//! Returns all selected faces.
//! \return indices of the selected faces.
const TColStd_PackedMapOfInteger& asiAlgo_AAG::GetSelectedFaces() const
{
  return m_selected;
}

//-----------------------------------------------------------------------------

//! \return attributes associated with graph arcs.
const asiAlgo_AAG::t_arc_attributes& asiAlgo_AAG::GetArcAttributes() const
{
  return m_arc_attributes;
}

//-----------------------------------------------------------------------------

//! \return attribute associated with the given arc.
const Handle(asiAlgo_FeatureAttr)& asiAlgo_AAG::GetArcAttribute(const t_arc& arc) const
{
  return m_arc_attributes.Find(arc);
}

//-----------------------------------------------------------------------------

//! Checks whether the given node has any attributes or not.
//! \param[in] node ID of the graph node to check.
//! \return true/false.
bool asiAlgo_AAG::HasNodeAttributes(const int node) const
{
  return m_node_attributes.IsBound(node);
}

//-----------------------------------------------------------------------------

//! Accessor for the collection of nodal attributes.
//! \param[in] node ID of the graph node of interest.
//! \return attributes associated with the given graph node.
const asiAlgo_AAG::t_attr_set& asiAlgo_AAG::GetNodeAttributes(const int node) const
{
  return m_node_attributes(node);
}

//-----------------------------------------------------------------------------

//! Returns attribute associated with the given graph node.
//! \param[in] node    ID of the graph node of interest.
//! \param[in] attr_id ID of the attribute to access.
//! \return attribute associated with the given node.
Handle(asiAlgo_FeatureAttr)
  asiAlgo_AAG::GetNodeAttribute(const int            node,
                                const Standard_GUID& attr_id) const
{
  if ( !m_node_attributes.IsBound(node) )
    return NULL;

  if ( !m_node_attributes(node).GetMap().IsBound(attr_id) )
    return NULL;

  return m_node_attributes(node)(attr_id);
}

//-----------------------------------------------------------------------------

//! Sets the given attribute for a node in AAG. If an attribute of this type
//! is already there, this method does nothing and returns false.
//! \param[in] node ID of the graph node of interest.
//! \param[in] attr attribute to set.
bool asiAlgo_AAG::SetNodeAttribute(const int                          node,
                                   const Handle(asiAlgo_FeatureAttr)& attr)
{
  if ( attr.IsNull() )
    return false;

  Handle(asiAlgo_FeatureAttr) existing = this->GetNodeAttribute( node, attr->GetGUID() );
  //
  if ( !existing.IsNull() )
    return false; // Already there

  if ( !m_node_attributes.IsBound(node) )
    m_node_attributes.Bind( node, t_attr_set(attr) );
  else
  {
    t_attr_set& attr_set = m_node_attributes.ChangeFind(node);
    attr_set.Add(attr);
  }

  return true;
}

//-----------------------------------------------------------------------------

//! Searches for those faces having ALL neighbors attributed with convex link.
//! \param[out] resultFaces found faces (if any).
//! \return true if anything has been found, false -- otherwise.
bool asiAlgo_AAG::FindConvexOnly(TopTools_IndexedMapOfShape& resultFaces) const
{
  TColStd_PackedMapOfInteger traversed;
  for ( t_adjacency::Iterator it(m_neighbors); it.More(); it.Next() )
  {
    const int                         current_face_idx       = it.Key();
    const TColStd_PackedMapOfInteger& current_face_neighbors = it.Value();

    // Mark face as traversed
    if ( !traversed.Contains(current_face_idx) )
      traversed.Add(current_face_idx);
    else
      continue;

    // Loop over the neighbors
    bool isAllConvex = true;
    for ( TColStd_MapIteratorOfPackedMapOfInteger nit(current_face_neighbors); nit.More(); nit.Next() )
    {
      const int neighbor_face_idx = nit.Key();

      // Get angle attribute
      Handle(asiAlgo_FeatureAttrAngle)
        attr = Handle(asiAlgo_FeatureAttrAngle)::DownCast( this->GetArcAttribute( t_arc(current_face_idx,
                                                                                        neighbor_face_idx) ) );

      if ( attr->GetAngle() != Angle_Convex )
      {
        isAllConvex = false;

        // Mark face as traversed as we don't want to check concave neighbors
        traversed.Add(neighbor_face_idx);
      }
    }

    if ( isAllConvex )
      resultFaces.Add( this->GetFace(current_face_idx) );
  }

  return resultFaces.Extent() > 0;
}

//-----------------------------------------------------------------------------

//! Searches for those faces having ALL neighbors attributed with concave link.
//! \param[out] resultFaces found faces (if any).
//! \return true if anything has been found, false -- otherwise.
bool asiAlgo_AAG::FindConcaveOnly(TopTools_IndexedMapOfShape& resultFaces) const
{
  TColStd_PackedMapOfInteger traversed;
  for ( t_adjacency::Iterator it(m_neighbors); it.More(); it.Next() )
  {
    const int                         current_face_idx       = it.Key();
    const TColStd_PackedMapOfInteger& current_face_neighbors = it.Value();

    // Mark face as traversed
    if ( !traversed.Contains(current_face_idx) )
      traversed.Add(current_face_idx);
    else
      continue;

    // Loop over the neighbors
    bool isAllConcave = true;
    for ( TColStd_MapIteratorOfPackedMapOfInteger nit(current_face_neighbors); nit.More(); nit.Next() )
    {
      const int neighbor_face_idx = nit.Key();

      // Get angle attribute
      Handle(asiAlgo_FeatureAttrAngle)
        attr = Handle(asiAlgo_FeatureAttrAngle)::DownCast( this->GetArcAttribute( t_arc(current_face_idx,
                                                                                        neighbor_face_idx) ) );

      if ( attr->GetAngle() != Angle_Concave )
      {
        isAllConcave = false;

        // Mark face as traversed as we don't want to check concave neighbors
        traversed.Add(neighbor_face_idx);
      }
    }

    if ( isAllConcave )
      resultFaces.Add( this->GetFace(current_face_idx) );
  }

  return resultFaces.Extent() > 0;
}


//-----------------------------------------------------------------------------

//! Removes the passed faces with all corresponding arcs from AAG.
//! \param[in] faces faces to remove.
void asiAlgo_AAG::Remove(const TopTools_IndexedMapOfShape& faces)
{
  // NOTICE: indexed map of shapes is not affected as we want to keep
  //         using the original indices of faces

  // Find IDs of the faces to remove
  TColStd_PackedMapOfInteger toRemove;
  for ( int f = 1; f <= faces.Extent(); ++f )
  {
    const int face_idx = this->GetFaceId( faces.FindKey(f) );
    toRemove.Add(face_idx);
  }

  // Loop over the target faces
  for ( TColStd_MapIteratorOfPackedMapOfInteger fit(toRemove); fit.More(); fit.Next() )
  {
    const int face_idx = fit.Key();

    // Unbind node attributes
    m_node_attributes.UnBind(face_idx);

    // Find all neighbors
    const TColStd_PackedMapOfInteger& neighbor_indices = m_neighbors.Find(face_idx);
    for ( TColStd_MapIteratorOfPackedMapOfInteger nit(neighbor_indices); nit.More(); nit.Next() )
    {
      const int neighbor_idx = nit.Key();

      // Unbind arc attributes
      m_arc_attributes.UnBind( t_arc(face_idx, neighbor_idx) );

      // Kill the faces being removed from the list of neighbors
      if ( m_neighbors.IsBound(neighbor_idx) )
        m_neighbors.ChangeFind(neighbor_idx).Subtract(toRemove);
    }

    // Unbind node
    m_neighbors.UnBind(face_idx);
  }
}

//-----------------------------------------------------------------------------

//! Dumps AAG structure to the passed output stream.
//! \param[in, out] out target stream.
void asiAlgo_AAG::Dump(Standard_OStream& out) const
{
  out << "===================================================\n";
  out << "***AAG structure\n";
  out << "---------------------------------------------------\n";
  out << " Adjacency\n";
  out << "---------------------------------------------------\n";

  // Dump neighborhood
  for ( int f = 1; f <= m_faces.Extent(); ++f )
  {
    out << "\t" << f << " -> ";
    const TColStd_PackedMapOfInteger& neighbors = this->GetNeighbors(f);
    //
    for ( TColStd_MapIteratorOfPackedMapOfInteger nit(neighbors); nit.More(); nit.Next() )
    {
      out << nit.Key() << " ";
    }
    out << "\n";
  }

  // Dump arc attributes
  out << "---------------------------------------------------\n";
  out << " Node attributes\n";
  out << "---------------------------------------------------\n";
  for ( int f = 1; f <= m_faces.Extent(); ++f )
  {
    if ( !this->HasNodeAttributes(f) )
      continue;

    const NCollection_DataMap<Standard_GUID, Handle(asiAlgo_FeatureAttr), Standard_GUID>& attrs = this->GetNodeAttributes(f).GetMap();
    //
    if ( attrs.IsEmpty() )
      continue;

    out << "\t" << f << " ~ ";
    //
    for ( NCollection_DataMap<Standard_GUID, Handle(asiAlgo_FeatureAttr), Standard_GUID>::Iterator ait(attrs); ait.More(); ait.Next() )
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

//! Initializes graph tool with master CAD and selected faces.
//! \param[in] masterCAD        master model (full CAD).
//! \param[in] selectedFaces    selected faces.
//! \param[in] allowSmooth      indicates whether "smooth" attribution for arcs
//!                             is allowed (true) or not (false).
//! \param[in] smoothAngularTol angular tolerance used for recognition
//!                             of smooth dihedral angles. A smooth angle
//!                             may appear to be imperfect by construction,
//!                             but still smooth by the design intent. With
//!                             this parameter you're able to control it.
void asiAlgo_AAG::init(const TopoDS_Shape&               masterCAD,
                       const TopTools_IndexedMapOfShape& selectedFaces,
                       const bool                        allowSmooth,
                       const double                      smoothAngularTol)
{
  m_master            = masterCAD;
  m_bAllowSmooth      = allowSmooth;
  m_fSmoothAngularTol = smoothAngularTol;

  if ( masterCAD.IsNull() )
    return;

  //---------------------------------------------------------------------------

  ShapeAnalysis_Edge sae;

  // Extract all sub-shapes with unique indices from the master CAD
  TopExp::MapShapes(masterCAD, m_subShapes);

  // Extract all faces with unique indices from the master CAD
  TopExp::MapShapes(masterCAD, TopAbs_FACE, m_faces);

  // Extract all edges with unique indices from the master CAD
  TopExp::MapShapes(masterCAD, TopAbs_EDGE, m_edges);

  // Extract all vertices with unique indices from the master CAD
  TopExp::MapShapes(masterCAD, TopAbs_VERTEX, m_vertices);

  // Fill adjacency map with empty buckets and provide all required
  // treatment for each individual face
  for ( int f = 1; f <= m_faces.Extent(); ++f )
  {
    m_neighbors.Bind( f, TColStd_PackedMapOfInteger() );
    //
    const TopoDS_Face& face = TopoDS::Face( m_faces(f) );

    // Special treatment deserve those faces having seam edges. Such faces
    // get their own attributes
    for ( TopExp_Explorer exp(face, TopAbs_EDGE); exp.More(); exp.Next() )
    {
      const TopoDS_Edge& edge = TopoDS::Edge( exp.Current() );
      //
      if ( sae.IsSeam(edge, face) )
      {
        TopTools_IndexedMapOfShape edges;
        asiAlgo_DihedralAngle solid_angle(NULL, NULL);

        // Notice that smooth transitions are not allowed here. This is because
        // the following treatment is designed for periodic faces, and we normally
        // have self-transition of quality C1 and better there
        const asiAlgo_FeatureAngle
          face_angle = solid_angle.AngleBetweenFaces(face, face, false, 0.0, edges);

        // Bind attribute
        m_node_attributes.Bind( f, t_attr_set( new asiAlgo_FeatureAttrAngle(face_angle) ) );
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

  //---------------------------------------------------------------------------

  // Save selected faces for future filtering
  for ( int s = 1; s <= selectedFaces.Extent(); ++s )
  {
    m_selected.Add( m_faces.FindIndex( selectedFaces.FindKey(s) ) );
  }
}

//-----------------------------------------------------------------------------

//! Fills graph with nodes for mate faces.
//! \param[in] mateFaces faces to add (if not yet added).
void asiAlgo_AAG::addMates(const TopTools_ListOfShape& mateFaces)
{
  // Create solid angle calculation tool
  asiAlgo_DihedralAngle solid_angle(NULL, NULL);

  // Now analyze the face pairs
  for ( TopTools_ListIteratorOfListOfShape lit(mateFaces); lit.More(); lit.Next() )
  {
    const int                   face_idx   = m_faces.FindIndex( lit.Value() );
    TColStd_PackedMapOfInteger& face_links = m_neighbors.ChangeFind(face_idx);
    const TopoDS_Face&          face       = TopoDS::Face( m_faces.FindKey(face_idx) );

    // Add all the rest faces as neighbors
    for ( TopTools_ListIteratorOfListOfShape lit2(mateFaces); lit2.More(); lit2.Next() )
    {
      const int linked_face_idx = m_faces.FindIndex( lit2.Value() );

      if ( linked_face_idx == face_idx )
        continue; // Skip the same index to avoid loop arcs in the graph

      if ( face_links.Contains(linked_face_idx) )
        continue;

      face_links.Add(linked_face_idx);

      // The graph is not oriented, so we do not want to compute arc
      // attribute G-F is previously we have already done F-G attribution
      t_arc arc(face_idx, linked_face_idx);
      if ( m_arc_attributes.IsBound(arc) )
        continue;

      //-----------------------------------------------------------------------
      // Associate attributes
      //-----------------------------------------------------------------------

      const TopoDS_Face& linked_face = TopoDS::Face( m_faces.FindKey(linked_face_idx) );
      //
      TopTools_IndexedMapOfShape commonEdges;

      // Here we let client code decide whether to allow smooth transitions
      // or not. Smooth transition normally requires additional processing
      // in order to classify feature angle as concave or convex
      const asiAlgo_FeatureAngle
        angle = solid_angle.AngleBetweenFaces(face,
                                              linked_face,
                                              m_bAllowSmooth,
                                              m_fSmoothAngularTol,
                                              commonEdges);

      // Bind attribute
      m_arc_attributes.Bind( arc, new asiAlgo_FeatureAttrAngle(angle, commonEdges) );
    }
  }
}
