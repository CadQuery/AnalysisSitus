//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_intersect_surfaces.h>

// OCCT includes
#include <GeomAPI_IntSS.hxx>

//! Constructs intersection tool initializing it with the
//! user-driven precision.
//! \param prec [in] precision of intersection.
geom_intersect_surfaces::geom_intersect_surfaces(const double prec)
: m_fPrec(prec)
{}

//! Performs surface-surface intersections on the given inputs.
//! \param surfaces [in]  surfaces to intersect.
//! \param result   [out] intersection curves.
void geom_intersect_surfaces::operator()(const TColGeom_SequenceOfSurface& surfaces,
                                         geom_intersection_curves_ss&      result) const
{
  for ( int s1 = 1; s1 <= surfaces.Length(); ++s1 )
  {
    for ( int s2 = s1 + 1; s2 <= surfaces.Length(); ++s2 )
    {
      GeomAPI_IntSS IntSS(surfaces(s1), surfaces(s2), m_fPrec);
      //
      if ( !IntSS.IsDone() )
        continue;

      // Add intersection results to the output
      const int nLines = IntSS.NbLines();
      for ( int k = 1; k <= nLines; ++k )
      {
        Handle(geom_intersection_curve_ss)
          icurve = new geom_intersection_curve_ss( m_fPrec, IntSS.Line(k),
                                                   surfaces(s1), surfaces(s2) );
        result.Append(icurve);
      }
    }
  }
}
