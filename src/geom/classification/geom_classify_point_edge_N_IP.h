//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_classify_point_edge_N_IP_h
#define geom_classify_point_edge_N_IP_h

// Geometry includes
#include <geom_classify_point_edge.h>
#include <geom_epsilon_base.h>

//! Point-Edge classifier enriched with in-plane neighborhood information.
class geom_classify_point_edge_N_IP : public geom_classify_point_edge
{
public:

  ASitus_EXPORT
    geom_classify_point_edge_N_IP(const TopoDS_Edge& E,
                                  const TopoDS_Face& F,
                                  const double       inaccuracy,
                                  const double       precision);

public:

  ASitus_EXPORT geom_membership
    operator()(const geom_intersection_point_probe_curve& PonCC,
               Handle(geom_epsilon_base)&                 N,
               ActAPI_ProgressEntry                       Journal);

  ASitus_EXPORT geom_membership
    operator()(const gp_Pnt&              P,
               Handle(geom_epsilon_base)& N,
               ActAPI_ProgressEntry       Journal);

  ASitus_EXPORT geom_membership
    operator()(const gp_Pnt&              P,
               const double               W,
               Handle(geom_epsilon_base)& N,
               ActAPI_ProgressEntry       Journal);

private:

  TopoDS_Face m_F; //!< Base face.

};

#endif
