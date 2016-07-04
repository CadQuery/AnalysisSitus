//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_project_point_curve.h>

// OCCT includes
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <ShapeAnalysis_Curve.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

//! Constructor.
//! \param Crv       [in] working curve.
//! \param f         [in] first parameter on the working curve.
//! \param l         [in] last parameter on the working curve.
//! \param precision [in] precision for the underlying projection method.
geom_project_point_curve::geom_project_point_curve(const Handle(Geom_Curve)& Crv,
                                                   const double              f,
                                                   const double              l,
                                                   const double              precision)
{
  m_curve = new Geom_TrimmedCurve(Crv, f, l);
  m_fPrec = precision;
}

//! Performs point-to-curve projection.
//! \param P        [in]     point to project to the working curve.
//! \param Result_P [out]    resulting point.
//! \param Result_w [out]    resulting parameter.
//! \param gap      [out]    max distance between curve and point.
//! \param Journal  [in/out] journal instance.
void geom_project_point_curve::operator()(const gp_Pnt&        P,
                                          gp_Pnt&              Result_P,
                                          double&              Result_w,
                                          double&              gap,
                                          ActAPI_ProgressEntry Journal)
{
  GeomAdaptor_Curve gac(m_curve);
  ShapeAnalysis_Curve sac;
  gap = sac.Project(gac, P, m_fPrec, Result_P, Result_w, 0);
}
