//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_curve_surface_dist.h>

// OCCT includes
#include <GeomAPI_ExtremaCurveSurface.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

//! Constructor.
//! \param Surf [in] working surface.
geom_curve_surface_dist::geom_curve_surface_dist(const Handle(Geom_Surface)& Surf)
: m_surf(Surf)
{
}

//! Measures curve-to-surface distance.
//! \param Curve   [in]     curve to check the distance for.
//! \param gap     [out]    max distance between curve and surface.
//! \param Journal [in/out] journal instance.
void geom_curve_surface_dist::operator()(Handle(Geom_Curve)&  Curve,
                                         double&              gap,
                                         ActAPI_ProgressEntry Journal)
{
  // Compute distance
  GeomAPI_ExtremaCurveSurface ExtCS(Curve, m_surf);
  double maxDist = 0.0;
  //
  for ( int i = 1; i <= ExtCS.NbExtrema(); ++i )
  {
    if ( ExtCS.Distance(i) > maxDist )
      maxDist = ExtCS.Distance(i);
  }
  gap = maxDist;
}
