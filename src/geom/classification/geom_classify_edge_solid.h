//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_classify_edge_solid_h
#define geom_classify_edge_solid_h

// Geometry includes
#include <geom_classifier.h>
#include <geom_intersection_point_probe_surface.h>

// OCCT includes
#include <TopoDS_Edge.hxx>
#include <TopoDS_Solid.hxx>

//! Edge-Solid classifier.
class geom_classify_edge_solid
{
public:

  ASitus_EXPORT
    geom_classify_edge_solid(const TopoDS_Solid& S);

public:

  ASitus_EXPORT geom_membership
    operator()(const TopoDS_Edge&                      E,
               geom_intersection_points_probe_surface& IntListNC,
               ActAPI_ProgressEntry                    Journal) const;

protected:

  TopoDS_Solid m_S; //!< Working solid.

};

#endif
