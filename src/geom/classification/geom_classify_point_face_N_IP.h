//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_classify_point_face_N_IP_h
#define geom_classify_point_face_N_IP_h

// Geometry includes
#include <geom_classify_point_face.h>
#include <geom_epsilon_base.h>

//! Point-Face classifier enriched with in-plane neighborhood information.
class geom_classify_point_face_N_IP : public geom_classify_point_face
{
public:

  ASitus_EXPORT
    geom_classify_point_face_N_IP(const TopoDS_Face& F,
                                  const double       inaccuracy,
                                  const double       precision);

public:

  ASitus_EXPORT geom_membership
    operator()(const gp_Pnt&              P,
               Handle(geom_epsilon_base)& N,
               ActAPI_ProgressEntry       Journal);

};

#endif
