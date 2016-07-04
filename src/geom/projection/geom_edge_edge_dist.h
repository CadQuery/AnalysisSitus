//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_edge_edge_dist_h
#define geom_edge_edge_dist_h

// Analysis Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <TColgp_SequenceOfPnt.hxx>
#include <TopoDS_Edge.hxx>

//! Computes edge-edge distance.
class geom_edge_edge_dist
{
public:

  ASitus_EXPORT
    geom_edge_edge_dist();

  ASitus_EXPORT
    geom_edge_edge_dist(const TopoDS_Edge& baseEdge,
                        const double       precision);

public:

  ASitus_EXPORT void
    Init(const TopoDS_Edge& baseEdge,
         const double       precision);

public:

  ASitus_EXPORT bool
    operator()(const TopoDS_Edge&   probeEdge,
               const bool           stopIfExceeded,
               double&              dist,
               const double         max_height_dist,
               const double         max_running_dist,
               bool&                hasOverlapping,
               ActAPI_ProgressEntry Journal);

private:

  TColgp_SequenceOfPnt m_basePts; //!< Discrete representation of the working edge.
  double               m_fPrec;   //!< Required precision.

};

#endif
