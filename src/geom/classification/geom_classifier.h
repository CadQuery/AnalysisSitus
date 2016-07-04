//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_classifier_h
#define geom_classifier_h

// Geometry includes
#include <geom_intersection_point_edges.h>
#include <geom_intersection_point_probe_surface.h>
#include <geom_intersection_point_ray_surface.h>
#include <geom_membership.h>
#include <geom_overlapping.h>

// OCCT includes
#include <gp_Ax1.hxx>
#include <gp_Pnt.hxx>
#include <math_BullardGenerator.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_HSequenceOfShape.hxx>

//! Common classification functions.
namespace geom_classifier
{
  ASitus_EXPORT gp_Ax1
    CastRay(const gp_Pnt&          pos,
            math_BullardGenerator& rng);

  ASitus_EXPORT geom_intersection_point_ray_surface
    ClosestPoint(const geom_intersection_points_ray_surface& pts);

  //------------------------------------------------------------
  // TODO: make RemoveCoincidentPoints() methods template-based
  //------------------------------------------------------------

  ASitus_EXPORT void
    RemoveCoincidentPoints(const double                    prec,
                           geom_intersection_points_edges& IntListNC);

  ASitus_EXPORT void
    RemoveCoincidentPoints(const double                            prec,
                           geom_intersection_points_probe_surface& IntListNC);

};

#endif
