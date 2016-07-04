//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_intersection_point_edges_h
#define geom_intersection_point_edges_h

// Analysis Situs includes
#include <analysis_situs.h>

// Geometry includes
#include <geom_intersection_point_probe_curve.h>

// OCCT includes
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

//! Auxiliary structure representing interaction of two edges.
struct geom_intersection_point_edges : public geom_intersection_point_probe_curve
{
public:

  //! Default constructor.
  inline geom_intersection_point_edges()
  : geom_intersection_point_probe_curve()
  {}

  //! Copy constructor.
  //! \param Inter [in] another intersection structure.
  inline geom_intersection_point_edges(const geom_intersection_point_edges& Inter)
  : geom_intersection_point_probe_curve(Inter.P,
                                        Inter.Uncertainty,
                                        Inter.W1,
                                        Inter.W2,
                                        Inter.T,
                                        Inter.Neighborhood,
                                        Inter.Membership),
    Face(Inter.Face), E1(Inter.E1), E2(Inter.E2)
  {}

  //! Constructor.
  //! \param PonC [in] intersection point (pure geometric).
  inline geom_intersection_point_edges(const geom_intersection_point_probe_curve& PonC)
  : geom_intersection_point_probe_curve(PonC.P,
                                        PonC.Uncertainty,
                                        PonC.W1,
                                        PonC.W2,
                                        PonC.T,
                                        PonC.Neighborhood,
                                        PonC.Membership)
  {}

  //! Constructor.
  //! \param PonC [in] intersection point (pure geometric).
  //! \param _F   [in] host face.
  //! \param _E1  [in] first edge (corresponds to W1 parameter).
  //! \param _E2  [in] second edge (corresponds to W2 parameter).
  inline geom_intersection_point_edges(const geom_intersection_point_probe_curve& PonC,
                                       const TopoDS_Face&                         _F,
                                       const TopoDS_Edge&                         _E1,
                                       const TopoDS_Edge&                         _E2)
  : geom_intersection_point_probe_curve(PonC.P,
                                        PonC.Uncertainty,
                                        PonC.W1,
                                        PonC.W2,
                                        PonC.T,
                                        PonC.Neighborhood,
                                        PonC.Membership),
    Face(_F), E1(_E1), E2(_E2)
  {}

public:

  TopoDS_Face Face; //!< Host face (neighborhood is computed relative to this face)
  TopoDS_Edge E1;   //!< First interacting edge.
  TopoDS_Edge E2;   //!< Second interacting edge.

};

//! Collection of intersection structures.
typedef NCollection_Sequence<geom_intersection_point_edges> geom_intersection_points_edges;

#endif
