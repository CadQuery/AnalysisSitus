//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_project_curve_surface.h>

// OCCT includes
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI_ExtremaCurveSurface.hxx>
#include <Precision.hxx>
#include <ShapeConstruct_ProjectCurveOnSurface.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

//! Constructor.
//! \param Surf      [in] surface to project the curve to.
//! \param precision [in] precision for the underlying intersection method.
geom_project_curve_surface::geom_project_curve_surface(const Handle(Geom_Surface)& Surf,
                                                       const double                precision)
: m_surf(Surf), m_fPrec(precision), m_distTool(Surf)
{
}

//! Performs curve-to-surface projection.
//! \param Curve   [in]     curve to project to the surface.
//! \param f       [in]     first parameter value.
//! \param l       [in]     last parameter value.
//! \param Result  [out]    projection result.
//! \param gap     [out]    max distance between curve and surface.
//! \param Journal [in/out] journal instance.
void geom_project_curve_surface::operator()(Handle(Geom_Curve)&   Curve,
                                            const double          f,
                                            const double          l,
                                            Handle(Geom2d_Curve)& Result,
                                            double&               gap,
                                            ActAPI_ProgressEntry  Journal)
{
  // Compute gap
  m_distTool(Curve, gap, Journal);

  // Run projection
  ShapeConstruct_ProjectCurveOnSurface Projector;
  Projector.Init(m_surf, m_fPrec);
  if ( !Projector.Perform(Curve, f, l, Result) )
    Result.Nullify();

#if defined DRAW_DEBUG
  DRAW_INITGROUP(geom_project_curve_surface)
  DRAW_CURVE(Curve, geom_project_curve_surface, Draw_blanc)
#endif
}
