//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_classify_point_edge_h
#define geom_classify_point_edge_h

// Geometry includes
#include <geom_classifier.h>

// OCCT includes
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

//! Point-Edge classifier.
class geom_classify_point_edge
{
public:

  ASitus_EXPORT
    geom_classify_point_edge(const TopoDS_Edge& E,
                             const double       inaccuracy,
                             const double       precision);

public:

  ASitus_EXPORT geom_membership
    operator()(const gp_Pnt&        P,
               ActAPI_ProgressEntry Journal);

  ASitus_EXPORT geom_membership
    operator()(const gp_Pnt&        P,
               TopoDS_Vertex&       extremity,
               ActAPI_ProgressEntry Journal);

protected:

  TopoDS_Edge m_edge;        //!< Working edge.
  double      m_fPrecision;  //!< Precision of the employed optimization method.
  double      m_fInaccuracy; //!< Inaccuracy in the input geometry.

};

#endif
