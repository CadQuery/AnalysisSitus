//-----------------------------------------------------------------------------
// Created on: 19 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_intersect_curve_surface_h
#define geom_intersect_curve_surface_h

// Geometry includes
#include <geom_intersection_point_cs.h>
#include <geom_intersection_point_probe_surface.h>

// OCCT includes
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <TColgp_SequenceOfDir.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TColgp_SequenceOfPnt2d.hxx>
#include <TColStd_SequenceOfReal.hxx>

//! Curve-Surface intersection tool.
class geom_intersect_curve_surface
{
public:

  geom_intersect_curve_surface();

public:

  void
    operator()(const Handle(Geom_Curve)&    curve,
               const double                 f,
               const double                 l,
               const Handle(Geom_Surface)&  surf,
               geom_intersection_points_cs& result);

  void
    operator()(const Handle(Geom_Curve)&               curve,
               const double                            f,
               const double                            l,
               const Handle(Geom_Surface)&             surf,
               geom_intersection_points_probe_surface& result);

  void
    operator()(const Handle(Geom_Curve)&   curve,
               const double                f,
               const double                l,
               const Handle(Geom_Surface)& surf,
               TColgp_SequenceOfPnt&       Points,
               TColgp_SequenceOfDir&       Tangencies,
               TColgp_SequenceOfPnt2d&     SurfaceParams,
               TColStd_SequenceOfReal&     LinearParams);

};

#endif
