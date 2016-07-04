//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_project_vertex_edge_h
#define geom_project_vertex_edge_h

// Analysis Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

//! Computes vertex-edge distance.
class geom_project_vertex_edge
{
public:

  ASitus_EXPORT
    geom_project_vertex_edge(const TopoDS_Edge& edge,
                             const double       precision);

public:

  ASitus_EXPORT bool
    operator()(const TopoDS_Vertex& probe,
               double&              dist,
               double&              param,
               gp_Pnt&              proj_P,
               ActAPI_ProgressEntry Journal);

private:

  TopoDS_Edge m_edge;  //!< Working edge.
  double      m_fPrec; //!< Internal precision for extrema.

};

#endif
