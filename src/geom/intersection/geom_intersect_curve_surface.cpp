//-----------------------------------------------------------------------------
// Created on: 19 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_intersect_curve_surface.h>

// OCCT includes
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI_IntCS.hxx>
#include <Precision.hxx>

//! Constructor.
geom_intersect_curve_surface::geom_intersect_curve_surface()
{}

//! Performs curve-surface intersection.
//! \param curve  [in]  curve to intersect with the surface.
//! \param f      [in]  first parameter value.
//! \param l      [in]  last parameter value.
//! \param surf   [in]  surface to intersect.
//! \param result [out] intersection points.
void geom_intersect_curve_surface::operator()(const Handle(Geom_Curve)&    curve,
                                              const double                 f,
                                              const double                 l,
                                              const Handle(Geom_Surface)&  surf,
                                              geom_intersection_points_cs& result)
{
  TColgp_SequenceOfPnt   Points;
  TColgp_SequenceOfDir   Tangencies;
  TColgp_SequenceOfPnt2d SurfaceParams;
  TColStd_SequenceOfReal LinearParams;

  // Evaluate intersections
  this->operator()(curve, f, l, surf, Points, Tangencies, SurfaceParams, LinearParams);

  // Populate result list
  for ( int p = 1; p <= Points.Length(); ++p )
  {
    const gp_Pnt&   P   = Points(p);
    const gp_Dir&   T   = Tangencies(p);
    const gp_Pnt2d& P2d = SurfaceParams(p);
    const double    w   = LinearParams(p);

    Handle(geom_intersection_point_cs)
      ipoint = new geom_intersection_point_cs(P, T, 0.0, P2d, w);
    //
    result.Append(ipoint);
  }
}

//! Performs curve-surface intersection.
//! \param curve  [in]  curve to intersect with the surface.
//! \param f      [in]  first parameter value.
//! \param l      [in]  last parameter value.
//! \param surf   [in]  surface to intersect.
//! \param result [out] intersection points.
void geom_intersect_curve_surface::operator()(const Handle(Geom_Curve)&               curve,
                                              const double                            f,
                                              const double                            l,
                                              const Handle(Geom_Surface)&             surf,
                                              geom_intersection_points_probe_surface& result)
{
  TColgp_SequenceOfPnt   Points;
  TColgp_SequenceOfDir   Tangencies;
  TColgp_SequenceOfPnt2d SurfaceParams;
  TColStd_SequenceOfReal LinearParams;

  // Evaluate intersections
  this->operator()(curve, f, l, surf, Points, Tangencies, SurfaceParams, LinearParams);

  // Populate result list
  for ( int p = 1; p <= Points.Length(); ++p )
  {
    const gp_Pnt&   P   = Points(p);
    const gp_Dir&   T   = Tangencies(p);
    const gp_Pnt2d& P2d = SurfaceParams(p);
    const double    w   = LinearParams(p);

    geom_intersection_point_probe_surface ipoint(P, T, P2d, w);
    //
    result.Append(ipoint);
  }
}

//! Performs curve-surface intersection.
//! \param curve [in]  curve to intersect with the surface.
//! \param f     [in]  first parameter value.
//! \param l     [in]  last parameter value.
//! \param surf  [in]  surface to intersect.
//! \param P     [out] intersection point in 3D.
//! \param T     [out] tangency direction on the curve at its intersection point.
//! \param P2d   [out] intersection parameters on the surface.
//! \param w     [out] intersection parameter on the curve.
void geom_intersect_curve_surface::operator()(const Handle(Geom_Curve)&   curve,
                                              const double                f,
                                              const double                l,
                                              const Handle(Geom_Surface)& surf,
                                              TColgp_SequenceOfPnt&       Points,
                                              TColgp_SequenceOfDir&       Tangencies,
                                              TColgp_SequenceOfPnt2d&     SurfaceParams,
                                              TColStd_SequenceOfReal&     LinearParams)
{
  // Intersect and populate the result list
  Handle(Geom_TrimmedCurve) tcurve = new Geom_TrimmedCurve(curve, f, l);
  GeomAPI_IntCS Inter(tcurve, surf);
  if ( Inter.IsDone() )
  {
    for ( int iSol = 1; iSol <= Inter.NbPoints(); ++iSol )
    {
      double u, v, w;
      Inter.Parameters(iSol, u, v, w);

      // Evaluate differential properties
      gp_Pnt   P;
      gp_Pnt2d P2d;
      gp_Vec   T;
      //
      tcurve->D1(w, P, T);
      P2d.SetCoord(u, v);

      // Set results
      Points.Append(P);
      Tangencies.Append( gp_Dir(T) );
      SurfaceParams.Append(P2d);
      LinearParams.Append(w);
    }
  }
}
