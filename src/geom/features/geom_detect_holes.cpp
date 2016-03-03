//-----------------------------------------------------------------------------
// Created on: 01 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_detect_holes.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

//! Constructor.
//! \param masterCAD [in] full CAD model.
geom_detect_holes::geom_detect_holes(const TopoDS_Shape& masterCAD)
{
  // Build master AAG
  m_aag = new geom_aag(masterCAD);
}

//! Performs hole detection.
//! \param radius [in] radius of holes to detect.
//! \return true in case of success, false -- otherwise.
bool geom_detect_holes::Perform(const double radius)
{
  // Iterate over the entire AAG in a random manner looking for
  // specific patterns
  for ( geom_aag::random_iterator it(m_aag); it.more(); it.next() )
  {
    const TColStd_PackedMapOfInteger& neighbors = it.neighbors();
    if ( neighbors.Extent() != 2 )
      continue;

    // Extract suspected face and check if it is a cylinder
    const int   suspected_face_id = it.face_id();
    TopoDS_Face suspected_face    = m_aag->GetFace(suspected_face_id);
    //
    if ( !this->isTypeOf<Geom_CylindricalSurface>(suspected_face) )
      continue;

    // Check if the neighbor faces are planes
    bool isPlanarSupport = true;
    for ( TColStd_MapIteratorOfPackedMapOfInteger nit(neighbors); nit.More(); nit.Next() )
    {
      const int   neighbor_id   = nit.Key();
      TopoDS_Face neighbor_face = m_aag->GetFace(neighbor_id);
      //
      if ( !this->isTypeOf<Geom_Plane>(neighbor_face) )
      {
        isPlanarSupport = false;
        break;
      }
    }
    //
    if ( !isPlanarSupport )
      continue;

    // Check radius of the cylinder against the requested barrier value
    Handle(Geom_CylindricalSurface)
      hole_surf = Handle(Geom_CylindricalSurface)::DownCast( BRep_Tool::Surface(suspected_face) );
    //
    if ( hole_surf->Radius() > radius )
      continue;

    // Add cylindrical face of the feature to the list of detected features
    m_result.Append(suspected_face);

    // Find a neighbor plane which is isolated
    for ( TColStd_MapIteratorOfPackedMapOfInteger nit(neighbors); nit.More(); nit.Next() )
    {
      const int                         neighbor_id        = nit.Key();
      const TColStd_PackedMapOfInteger& neighbor_neighbors = m_aag->GetNeighbors(neighbor_id);

      if ( neighbor_neighbors.Extent() == 1 && neighbor_neighbors.Contains(suspected_face_id) )
      {
        // Add bottom face of the feature to the list of detected features
        m_result.Append( m_aag->GetFace(neighbor_id) );
      }
    }
  }

  return true; // Success
}
