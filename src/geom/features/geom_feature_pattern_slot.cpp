//-----------------------------------------------------------------------------
// Created on: 21 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_feature_pattern_slot.h>

// Geometry includes
#include <geom_feature_angle_attr.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

//! Recognizes feature starting from the current position of AAG iterator.
//! \param featureFaces [out] feature faces.
//! \return true/false.
bool geom_feature_pattern_slot::recognize(TopTools_IndexedMapOfShape& featureFaces)
{
  // Get seed face
  const int seed_face_id = m_it->GetFaceId();

  // Iterate by neighbors starting from the seed face
  bool isCyclic        = false;
  bool stop            = false;
  bool isConvex        = false;
  int  current_face_id = seed_face_id;
  int  numItems        = 0;
  do
  {
    ++numItems;
    this->SetTraversed(current_face_id);

    // Get next tuple of neighbors
    const TColStd_PackedMapOfInteger& neighbor_ids = m_it->GetGraph()->GetNeighbors(current_face_id);

    // Check that all neighbors are concave-adjacent
    for ( TColStd_MapIteratorOfPackedMapOfInteger nit(neighbor_ids); nit.More(); nit.Next() )
    {
      // Get angle attribute
      Handle(geom_feature_angle_attr)
        attr = Handle(geom_feature_angle_attr)::DownCast( m_it->GetGraph()->GetAttribute( geom_aag::t_arc( current_face_id, nit.Key() ) ) );
      //
      if ( attr->GetAngle() == Angle_Convex )
      {
        if ( !isConvex )
          isConvex = true;
      }
    }

    // Get those neighbors which haven't been traversed yet
    TColStd_PackedMapOfInteger new_neighbor_ids;
    new_neighbor_ids.Subtraction( neighbor_ids, this->JustTraversed() );

    if ( new_neighbor_ids.Extent() )
      current_face_id = new_neighbor_ids.GetMinimalMapped();
    else
    {
      stop = true;

      // Check if the graph is cyclic
      if ( neighbor_ids.Contains(seed_face_id) )
        isCyclic = true;
    }
  }
  while ( !stop );

  const bool isFeature = (isCyclic && !isConvex && numItems > 2);

  // A sub-graph has to be cyclic, contain more that 2 faces and all
  // arcs should be concave
  if ( isFeature )
  {
    // Fill collection of feature faces
    for ( TColStd_MapIteratorOfPackedMapOfInteger mit( this->JustTraversed() ); mit.More(); mit.Next() )
      featureFaces.Add( m_it->GetGraph()->GetFace( mit.Key() ) );
  }

  return isFeature;
}
