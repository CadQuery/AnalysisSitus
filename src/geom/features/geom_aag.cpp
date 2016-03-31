//-----------------------------------------------------------------------------
// Created on: 26 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_aag.h>

// Geometry includes
#include <geom_feature_angle_attr.h>
#include <geom_utils.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

// Qr includes
#include <QrCore_Types.h>

//! Initializes AAG from the given master model and selected faces.
//! \param masterCAD     [in] master model (full CAD).
//! \param selectedFaces [in] selected faces.
geom_aag::geom_aag(const TopoDS_Shape&               masterCAD,
                   const TopTools_IndexedMapOfShape& selectedFaces)
{
  this->init(masterCAD, selectedFaces);
}

//! Constructor accepting master CAD only.
//! \param masterCAD [in] master CAD.
geom_aag::geom_aag(const TopoDS_Shape& masterCAD)
{
  this->init( masterCAD, TopTools_IndexedMapOfShape() );
}

//! Destructor.
geom_aag::~geom_aag()
{}

//! Returns topological face by its internal index (e.g. coming from iterator).
//! \param face_idx [in] face index.
//! \return topological face.
const TopoDS_Face& geom_aag::GetFace(const int face_idx) const
{
  return TopoDS::Face( m_faces.FindKey(face_idx) );
}

//! Returns face ID.
//! \param face [in] face of interest.
//! \return face ID.
int geom_aag::GetFaceId(const TopoDS_Shape& face) const
{
  return m_faces.FindIndex(face);
}

//! Returns neighbors for the face having the given its internal index.
//! \param face_idx [in] face index.
//! \return indices of the neighbor faces.
const TColStd_PackedMapOfInteger& geom_aag::GetNeighbors(const int face_idx) const
{
  return m_neighbors.Find(face_idx);
}

//! Returns full collection of neighbor faces.
//! \return neighborhood data.
const geom_aag::t_adjacency& geom_aag::GetNeighborhood() const
{
  return m_neighbors;
}

//! Returns all selected faces.
//! \return indices of the selected faces.
const TColStd_PackedMapOfInteger& geom_aag::GetSelectedFaces() const
{
  return m_selected;
}

//! Returns all faces of AAG.
//! \return all AAG faces.
const TopTools_IndexedMapOfShape& geom_aag::GetFaces() const
{
  return m_faces;
}

//! \return attributes associated with graph arcs.
const geom_aag::t_attributes& geom_aag::GetAttributes() const
{
  return m_attributes;
}

//! \return attribute associated with the given arc.
const Handle(geom_feature_attribute)& geom_aag::GetAttribute(const t_arc& arc) const
{
  return m_attributes.Find(arc);
}

//! Searches for those faces having ALL neighbors attributed with convex link.
//! \param resultFaces [out] found faces (if any).
//! \return true if anything has been found, false -- otherwise.
bool geom_aag::FindConvexOnly(TopTools_IndexedMapOfShape& resultFaces) const
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
      Handle(geom_feature_angle_attr)
        attr = Handle(geom_feature_angle_attr)::DownCast( this->GetAttribute( t_arc(current_face_idx, neighbor_face_idx) ) );

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

//! Removes the passed faces with all corresponding arcs from AAG.
//! \param faces [in] faces to remove.
void geom_aag::Remove(const TopTools_IndexedMapOfShape& faces)
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

    // Find all neighbors
    const TColStd_PackedMapOfInteger& neighbor_indices = m_neighbors.Find(face_idx);
    for ( TColStd_MapIteratorOfPackedMapOfInteger nit(neighbor_indices); nit.More(); nit.Next() )
    {
      const int neighbor_idx = nit.Key();

      // Unbind arc attributes
      m_attributes.UnBind( t_arc(face_idx, neighbor_idx) );

      // Kill the faces being removed from the list of neighbors
      if ( m_neighbors.IsBound(neighbor_idx) )
        m_neighbors.ChangeFind(neighbor_idx).Subtract(toRemove);
    }

    // Unbind node
    m_neighbors.UnBind(face_idx);
  }
}

//-----------------------------------------------------------------------------

//! Initializes graph tool with master CAD and selected faces.
//! \param masterCAD     [in] master model (full CAD).
//! \param selectedFaces [in] selected faces.
void geom_aag::init(const TopoDS_Shape&               masterCAD,
                    const TopTools_IndexedMapOfShape& selectedFaces)
{
  //---------------------------------------------------------------------------

  // Extract all faces with unique indexes from the master CAD.
  TopExp::MapShapes(masterCAD, TopAbs_FACE, m_faces);

  // Fill adjacency map with empty buckets
  for ( int f = 1; f <= m_faces.Extent(); ++f )
  {
    m_neighbors.Bind( f, TColStd_PackedMapOfInteger() );
  }

  //---------------------------------------------------------------------------

  TopTools_IndexedDataMapOfShapeListOfShape ChildParentMap;
  TopExp::MapShapesAndAncestors(masterCAD, TopAbs_EDGE, TopAbs_FACE, ChildParentMap);

  // Build adjacency graph
  for ( TopExp_Explorer exp(masterCAD, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    const TopTools_ListOfShape& adjacentFaces = ChildParentMap.FindFromKey( exp.Current() );
    this->addMates(adjacentFaces);
  }

  //---------------------------------------------------------------------------

  // Save selected faces for future filtering
  for ( int s = 1; s <= selectedFaces.Extent(); ++s )
  {
    m_selected.Add( m_faces.FindIndex( selectedFaces.FindKey(s) ) );
  }
}

//! Fills graph with nodes for mate faces.
//! \param mateFaces [in] faces to add (if not yet added).
void geom_aag::addMates(const TopTools_ListOfShape& mateFaces)
{
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

      face_links.Add(linked_face_idx);

      //-----------------------------------------------------------------------
      // Associate attributes
      //-----------------------------------------------------------------------

      const TopoDS_Face& linked_face = TopoDS::Face( m_faces.FindKey(linked_face_idx) );
      //
      TopTools_IndexedMapOfShape commonEdges;
      const geom_angle angle = geom_utils::AngleBetweenFaces(face, linked_face, commonEdges);

      // Bind attribute
      m_attributes.Bind( t_arc(face_idx, linked_face_idx), new geom_feature_angle_attr(angle) );
    }
  }
}
