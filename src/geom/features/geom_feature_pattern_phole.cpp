//-----------------------------------------------------------------------------
// Created on: 10 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_feature_pattern_phole.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <Geom_Plane.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

//! Recognizes feature starting from the current position of AAG iterator.
//! \param featureFaces [out] feature faces.
//! \return true/false.
bool geom_feature_pattern_phole::Recognize(TopTools_IndexedMapOfShape& featureFaces)
{
  const TColStd_PackedMapOfInteger& neighbors = m_it->GetNeighbors();
  if ( neighbors.Extent() < 4 )
    return false;

  // Extract suspected face
  const int   suspected_face_id = m_it->GetFaceId();
  TopoDS_Face suspected_face    = m_it->GetGraph()->GetFace(suspected_face_id);

  // Check if this face has internal contour
  int nWires = 0;
  for ( TopExp_Explorer exp(suspected_face, TopAbs_WIRE); exp.More(); exp.Next() )
    ++nWires;

  // If there are no internal contours, then the suspected
  // face cannot be a seed one
  if ( nWires < 2 )
    return false;

  // The base face should be planar
  if ( !this->isTypeOf<Geom_Plane>(suspected_face) )
    return false;

  // Now check its neighbors and collect the suspected ones
  TColStd_PackedMapOfInteger suspected_neighbor_ids;
  //
  for ( TColStd_MapIteratorOfPackedMapOfInteger nit(neighbors); nit.More(); nit.Next() )
  {
    const int   neighbor_id   = nit.Key();
    TopoDS_Face neighbor_face = m_it->GetGraph()->GetFace(neighbor_id);
    //
    if ( !this->isTypeOf<Geom_Plane>(neighbor_face) )
      continue; // All neighbors should be planar

    // Count number of contours
    int nNeighborWires = 0;
    for ( TopExp_Explorer exp(neighbor_face, TopAbs_WIRE); exp.More(); exp.Next() )
      ++nNeighborWires;

    // Candidate side face should have no internal contours
    if ( nNeighborWires != 1 )
      continue;

    // Each side should have only four neighbors
    if ( m_it->GetGraph()->GetNeighbors(neighbor_id).Extent() != 4 )
      continue;

    // Eventually this neighbor becomes a candidate
    suspected_neighbor_ids.Add(neighbor_id);
  }

  // Iterate over the suspected neighbors
  TColStd_PackedMapOfInteger candidate_neighbor_ids;
  //
  for ( TColStd_MapIteratorOfPackedMapOfInteger nit(suspected_neighbor_ids); nit.More(); nit.Next() )
  {
    const int neighbor_idx = nit.Key();

    //// Keep only chains. I.e. each item should have two other items as
    //// neighbors. If not, it cannot be a part of a hole

    const TColStd_PackedMapOfInteger&
      neighbor_neighbors = m_it->GetGraph()->GetNeighbors(neighbor_idx);

    int non_candidate_brothers = 0, candidate_brothers = 0;
    for ( TColStd_MapIteratorOfPackedMapOfInteger nit2(neighbor_neighbors); nit2.More(); nit2.Next() )
    {
      if ( !suspected_neighbor_ids.Contains( nit2.Key() ) )
        ++non_candidate_brothers;
      else
        ++candidate_brothers;
    }

    //// Look for two candidates
    //int occurences = 0;
    //for ( TColStd_MapIteratorOfPackedMapOfInteger nit2(suspected_neighbor_ids); nit2.More(); nit2.Next() )
    //{
    //  if ( nit2.Key() == neighbor_idx )
    //    continue;

    //  if ( neighbor_neighbors.Contains( nit2.Key() ) )
    //    ++occurences;
    //}

    if ( non_candidate_brothers == 2 && candidate_brothers > 1 )
      candidate_neighbor_ids.Add(neighbor_idx);
      //featureFaces.Add( TopoDS::Face(neighbor) );
  }

  for ( TColStd_MapIteratorOfPackedMapOfInteger nit(candidate_neighbor_ids); nit.More(); nit.Next() )
  {
    const int          neighbor_idx = nit.Key();
    const TopoDS_Face& neighbor     = m_it->GetGraph()->GetFace(neighbor_idx);

    const TColStd_PackedMapOfInteger&
      neighbor_neighbors = m_it->GetGraph()->GetNeighbors(neighbor_idx);

    int candidate_brothers = 0;
    for ( TColStd_MapIteratorOfPackedMapOfInteger nit2(neighbor_neighbors); nit2.More(); nit2.Next() )
    {
      if ( candidate_neighbor_ids.Contains( nit2.Key() ) )
        ++candidate_brothers;
    }

    if ( candidate_brothers )
      featureFaces.Add( TopoDS::Face(neighbor) );
  }

  // Set radius
  m_fRadius = 0.0; // TODO
  return true;
}
