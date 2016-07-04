//-----------------------------------------------------------------------------
// Created on: 19 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_intersect_ray_shape.h>

// OCCT includes
#include <gp_Lin.hxx>
#include <Precision.hxx>

#define MAX_MEANINGFUL_DIST 10000

//! Constructor.
//! \param shape     [in] shape to intersect.
//! \param precision [in] precision for the underlying intersection method.
geom_intersect_ray_shape::geom_intersect_ray_shape(const TopoDS_Shape& shape,
                                                   const double        precision)
: m_shape(shape), m_fPrec(precision)
{
  m_tool.Load(m_shape, m_fPrec);
}

//! Performs ray-shape intersection.
//! \param ray    [in]  ray to intersect with the body shape.
//! \param result [out] intersection points.
void geom_intersect_ray_shape::operator()(const gp_Ax1&                      ray,
                                          geom_intersection_points_ray_face& result)
{
  gp_Lin rayLine(ray);

  // Intersect and populate the result list
  m_tool.Perform( rayLine, -MAX_MEANINGFUL_DIST, MAX_MEANINGFUL_DIST );
  if ( m_tool.IsDone() )
  {
    for ( int iSol = 1; iSol <= m_tool.NbPnt(); ++iSol )
    {
      result.Append( geom_intersection_point_ray_face( m_tool.Pnt(iSol), m_tool.Face(iSol) ) );
    }
  }
}
