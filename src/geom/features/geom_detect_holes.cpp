//-----------------------------------------------------------------------------
// Created on: 01 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_detect_holes.h>

// Geometry includes
#include <geom_aag_iterator.h>
#include <geom_feature_pattern_chole.h>
#include <geom_feature_pattern_phole.h>

// OCCT includes
#include <TopoDS.hxx>

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
  Handle(geom_aag_random_iterator)   it        = new geom_aag_random_iterator(m_aag);
  Handle(geom_feature_pattern_chole) cdetector = new geom_feature_pattern_chole(it, radius);
  Handle(geom_feature_pattern_phole) pdetector = new geom_feature_pattern_phole(it, radius);
  //
  for ( ; it->More(); it->Next() )
  {
    // TODO: skip traversed faces (!!!)

    // Cylindrical holes
    if ( cdetector->Recognize(m_result) )
    {
      std::cout << "\tDetected cylindrical hole" << std::endl;
    }

    // Planar holes
    if ( pdetector->Recognize(m_result) )
    {
      std::cout << "\tDetected planar hole" << std::endl;
    }
  }

  return true; // Success
}
