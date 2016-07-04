//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_project_vertex_edge.h>

// Geometry includes
#include <geom_project_point_curve.h>

// OCCT include
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

//! Constructor.
//! \param edge      [in] working edge.
//! \param precision [in] precision for the underlying extrema.
geom_project_vertex_edge::geom_project_vertex_edge(const TopoDS_Edge& edge,
                                                   const double       precision)
: m_edge(edge), m_fPrec(precision)
{}

//! Computes distance between the passed probe vertex and the working edge.
//! \param probe   [in]     probe vertex to check against the edge.
//! \param dist    [out]    distance between two objects.
//! \param param   [out]    projection parameter on the working edge.
//! \param proj_P  [out]    projected point.
//! \param Journal [in/out] journal instance.
//! \return true in case of success, false -- otherwise.
bool geom_project_vertex_edge::operator()(const TopoDS_Vertex& probe,
                                          double&              dist,
                                          double&              param,
                                          gp_Pnt&              proj_P,
                                          ActAPI_ProgressEntry Journal)
{
  // Query the spatial curve
  double f, l;
  Handle(Geom_Curve) C = BRep_Tool::Curve(m_edge, f, l);

  // Check curve
  if ( C.IsNull() )
  {
    Journal.SendLogMessage( LogWarn(Normal) << "geom_project_vertex_edge.NullCurve" << m_edge );
    return false;
  }

  // Project point to the edge's host curve
  geom_project_point_curve ProjectPC(C, f, l, m_fPrec);

  // Do projection
  ProjectPC(BRep_Tool::Pnt(probe), proj_P, param, dist, Journal);
  return true;
}
