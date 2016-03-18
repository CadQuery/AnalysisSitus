//-----------------------------------------------------------------------------
// Created on: 10 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_feature_pattern_chole.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

//! Recognizes feature starting from the current position of AAG iterator.
//! \param featureFaces [out] feature faces.
//! \return true/false.
bool geom_feature_pattern_chole::Recognize(TopTools_IndexedMapOfShape& featureFaces)
{
  const TColStd_PackedMapOfInteger& neighbors = m_it->GetNeighbors();
  if ( neighbors.Extent() != 2 )
    return false;

  // Extract suspected face and check if it is a cylinder
  bool        isCylindrical     = false;
  double      suspected_radius  = 0.0;
  const int   suspected_face_id = m_it->GetFaceId();
  TopoDS_Face suspected_face    = m_it->GetGraph()->GetFace(suspected_face_id);
  //
  if ( this->isTypeOf<Geom_CylindricalSurface>(suspected_face) )
  {
    isCylindrical = true;

    // Get radius
    Handle(Geom_CylindricalSurface)
      hole_surf = Handle(Geom_CylindricalSurface)::DownCast( BRep_Tool::Surface(suspected_face) );
    //
    suspected_radius = hole_surf->Radius();
  }
  else if ( this->isTypeOf<Geom_RectangularTrimmedSurface>(suspected_face) )
  {
    Handle(Geom_RectangularTrimmedSurface)
      RT = Handle(Geom_RectangularTrimmedSurface)::DownCast( BRep_Tool::Surface(suspected_face) );

    if ( RT->BasisSurface()->IsInstance( STANDARD_TYPE(Geom_CylindricalSurface) ) )
    {
      isCylindrical = true;

      // Get radius
      Handle(Geom_CylindricalSurface)
        hole_surf = Handle(Geom_CylindricalSurface)::DownCast( RT->BasisSurface() );
      //
      suspected_radius = hole_surf->Radius();
    }
  }

  if ( !isCylindrical )
    return false;

  // Check radius of the cylinder against the requested barrier value
  if ( suspected_radius > m_fTargetRadius )
    return false;

  // Check if the neighbor faces are planes
  bool isPlanarSupport = true;
  for ( TColStd_MapIteratorOfPackedMapOfInteger nit(neighbors); nit.More(); nit.Next() )
  {
    const int   neighbor_id   = nit.Key();
    TopoDS_Face neighbor_face = m_it->GetGraph()->GetFace(neighbor_id);
    //
    if ( !this->isTypeOf<Geom_Plane>(neighbor_face) )
    {
      isPlanarSupport = false;
      break;
    }
  }
  //
  if ( !isPlanarSupport )
    return false;

  // Add cylindrical face of the feature to the list of detected features
  featureFaces.Add(suspected_face);

  // Find a neighbor plane which is isolated
  for ( TColStd_MapIteratorOfPackedMapOfInteger nit(neighbors); nit.More(); nit.Next() )
  {
    const int                         neighbor_id        = nit.Key();
    const TColStd_PackedMapOfInteger& neighbor_neighbors = m_it->GetGraph()->GetNeighbors(neighbor_id);

    if ( neighbor_neighbors.Extent() == 1 && neighbor_neighbors.Contains(suspected_face_id) )
    {
      // Add bottom face of the feature to the list of detected features
      featureFaces.Add( m_it->GetGraph()->GetFace(neighbor_id) );
    }
  }

  // Set radius
  m_fRadius = suspected_radius;
  return true;
}
