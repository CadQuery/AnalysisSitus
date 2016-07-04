//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_classify_edge_face_h
#define geom_classify_edge_face_h

// Geometry includes
#include <geom_classifier.h>
#include <geom_intersection_point_edges.h>

// OCCT includes
#include <Bnd_Box.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

//! Edge-Face classifier.
class geom_classify_edge_face
{
public:

  ASitus_EXPORT
    geom_classify_edge_face(const TopoDS_Face& F,
                            const double       inaccuracy);

public:

  ASitus_EXPORT geom_membership
    operator()(const TopoDS_Edge&              Probe,
               geom_intersection_points_edges& IntListNC,
               ActAPI_ProgressEntry            Journal);

  ASitus_EXPORT geom_membership
    operator()(const TopoDS_Edge&              Probe,
               Bnd_Box&                        ProbeBounds,
               geom_intersection_points_edges& IntListNC,
               ActAPI_ProgressEntry            Journal);

protected:

  TopoDS_Face m_F;           //!< Working face.
  Bnd_Box     m_FBounds;     //!< Bounding box of the working face.
  double      m_fInaccuracy; //!< Inaccuracy of geometry.

};

#endif
