//-----------------------------------------------------------------------------
// Created on: 19 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_intersect_ray_shape_h
#define geom_intersect_ray_shape_h

// Geometry includes
#include <geom_intersection_point_ray_face.h>

// OCCT includes
#include <gp_Ax1.hxx>
#include <IntCurvesFace_ShapeIntersector.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TopoDS_Shape.hxx>

//! Ray-Shape intersection tool.
class geom_intersect_ray_shape
{
public:

  geom_intersect_ray_shape(const TopoDS_Shape& shape,
                           const double        precision);

public:

  void
    operator()(const gp_Ax1&                      ray,
               geom_intersection_points_ray_face& result);

private:

  IntCurvesFace_ShapeIntersector m_tool;  //!< Intersection tool.
  TopoDS_Shape                   m_shape; //!< Working body.
  double                         m_fPrec; //!< Required precision.

};

#endif
