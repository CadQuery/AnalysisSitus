//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_intersect_curve_curve_2D.h>

// OCCT includes
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAPI_InterCurveCurve.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <Precision.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

//! Constructor.
//! \param Curve     [in] working curve.
//! \param Surface   [in] host surface.
//! \param precision [in] precision for the underlying intersection method.
geom_intersect_curve_curve_2D::geom_intersect_curve_curve_2D(const Handle(Geom2d_Curve)& Curve,
                                                             const Handle(Geom_Surface)& Surface,
                                                             const double                precision)
: m_curve(Curve), m_surf(Surface), m_fPrec(precision)
{
}

//! Performs curve-curve intersection.
//! \param Curve   [in]     curve to intersect with the working one.
//! \param f       [in]     first parameter value.
//! \param l       [in]     last parameter value.
//! \param Result  [out]    intersection points.
//! \param Journal [in/out] journal instance.
void geom_intersect_curve_curve_2D::operator()(const Handle(Geom2d_Curve)&     Curve,
                                               const double                    f,
                                               const double                    l,
                                               geom_intersection_points_cc_2D& Result,
                                               ActAPI_ProgressEntry            PEntry)
{
  // Intersect and populate the result list
  Handle(Geom2d_TrimmedCurve) TCurve = new Geom2d_TrimmedCurve(Curve, f, l);
  Geom2dAPI_InterCurveCurve Inter(m_curve, Curve, m_fPrec);

#if defined DRAW_DEBUG
  DRAW_INITGROUP(geom_intersect_curves)
  DRAW_CURVE2D(Curve, geom_intersect_curves, Draw_rouge)
  DRAW_CURVE2D(m_curve, geom_intersect_curves, Draw_vert)
#endif

  for ( int iSol = 1; iSol <= Inter.NbPoints(); ++iSol )
  {
    double w1, w2;
    w1 = Inter.Intersector().Point(iSol).ParamOnFirst();
    w2 = Inter.Intersector().Point(iSol).ParamOnSecond();

    // Set local curve tangency as direction at intersection point
    gp_Pnt2d UV;
    gp_Vec2d T;
    TCurve->D1(w1, UV, T);

#if defined DRAW_DEBUG
    DRAW_POINT(UV, geom_intersect_curves, Draw_rouge, Draw_X)
#endif

    // Evaluate host surface to obtain the 3D point
    gp_Pnt P = m_surf->Value( UV.X(), UV.Y() );

    // Append result
    Result.Append( geom_intersection_point_cc_2D(Inter.Intersector().Point(iSol).Value(), w1, w2, T, P) );
  }
}
