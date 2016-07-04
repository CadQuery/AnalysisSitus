//-----------------------------------------------------------------------------
// Created on: 22 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_intersect_curve_curve.h>

// OCCT includes
#include <BSplCLib.hxx>
#include <Extrema_ExtCC.hxx>
#include <Extrema_POnCurv.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <GeomConvert.hxx>
#include <Precision.hxx>
#include <Quantity_Parameter.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <TColStd_Array1OfReal.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

//! Constructor.
//! \param Curve     [in] working curve.
//! \param f         [in] first parameter value.
//! \param l         [in] last parameter value.
//! \param precision [in] precision for the underlying intersection method.
geom_intersect_curve_curve::geom_intersect_curve_curve(const Handle(Geom_Curve)& Curve,
                                                       const double              f,
                                                       const double              l,
                                                       const double              precision)
: m_fFirst(f), m_fLast(l), m_fPrec(precision)
{
  m_curve = new Geom_TrimmedCurve(Curve, f, l);
}

//! Performs curve-curve intersection.
//! \param Curve   [in]     curve to intersect with the working one.
//! \param f       [in]     first parameter value.
//! \param l       [in]     last parameter value.
//! \param Result  [out]    intersection points.
//! \param Journal [in/out] journal instance.
void geom_intersect_curve_curve::operator()(const Handle(Geom_Curve)&             Curve,
                                            const double                          f,
                                            const double                          l,
                                            geom_intersection_points_probe_curve& Result,
                                            ActAPI_ProgressEntry                  ASitus_NotUsed(Journal))
{
  Handle(Geom_Curve) Probe = new Geom_TrimmedCurve(Curve, f, l);

  // Prepare adapters representing the curves to intersect
  GeomAdaptor_Curve C1(m_curve);
  GeomAdaptor_Curve C2(Probe);

  double U1f, U1l, U2f, U2l;
  U1f = C1.FirstParameter();
  U1l = C1.LastParameter();
  U2f = C2.FirstParameter();
  U2l = C2.LastParameter();

  GeomAPI_ExtremaCurveCurve Ex(m_curve, Probe, U1f, U1l, U2f, U2l);
  gp_Pnt P1, P2;
  if ( Ex.TotalNearestPoints(P1, P2) )
  {
    const double uncertainty = P1.Distance(P2);

    if ( uncertainty < m_fPrec )
    {
      Quantity_Parameter W1, W2;
      Ex.TotalLowerDistanceParameters(W1, W2);

      // Get local properties
      gp_Pnt dummy_P, P = 0.5*( P1.XYZ() + P2.XYZ() );
      gp_Vec T;
      C1.D1(W1, dummy_P, T);

      // Prepare intersection point
      Result.Append( geom_intersection_point_probe_curve(P, uncertainty, W1, W2, T) );

#if defined DRAW_DEBUG
      DRAW_INITGROUP(geom_intersect_curve_curve)
      DRAW_POINT(P, geom_intersect_curve_curve, Draw_rouge, Draw_X)
      DRAW_POINT(C1.Value(W1), geom_intersect_curve_curve, Draw_orange, Draw_Losange)
      DRAW_LINE3D(dummy_P, gp_Pnt( dummy_P.XYZ() + 100*T.XYZ() ), geom_intersect_curve_curve, Draw_blanc)
#endif
    }
  }
}
