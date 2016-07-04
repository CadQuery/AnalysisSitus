//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_intersect_curves.h>

// OCCT includes
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>

//! Constructs intersection tool initializing it with the
//! user-driven precision.
//! \param prec     [in] precision of intersection.
//! \param uLimit   [in] parametric bound to limit infinite curves.
//! \param progress [in] Progress Entry.
//! \param plotter  [in] Imperative Plotter.
geom_intersect_curves::geom_intersect_curves(const double         prec,
                                             const double         uLimit,
                                             ActAPI_ProgressEntry progress,
                                             ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter),
  m_fPrec(prec),
  m_fLimit(uLimit)
{}

//! Performs curve-curve intersections on the given inputs.
//! \param curves [in]  curves to intersect.
//! \param result [out] resulting intersection points.
void geom_intersect_curves::operator()(const geom_intersection_curves_ss& curves,
                                       geom_intersection_points_cc&       result)
{
  for ( int s1 = 1; s1 <= curves.Length(); ++s1 )
  {
    double f1 = Max( -m_fLimit, curves(s1)->C->FirstParameter() );
    double l1 = Min(  m_fLimit, curves(s1)->C->LastParameter() );
    //
    Handle(Geom_Curve) C1 = new Geom_TrimmedCurve(curves(s1)->C, f1, l1);

    for ( int s2 = s1 + 1; s2 <= curves.Length(); ++s2 )
    {
      double f2 = Max( -m_fLimit, curves(s2)->C->FirstParameter() );
      double l2 = Min(  m_fLimit, curves(s2)->C->LastParameter() );
      //
      Handle(Geom_Curve) C2 = new Geom_TrimmedCurve(curves(s2)->C, f2, l2);

      // Find extrema
      GeomAPI_ExtremaCurveCurve Ex(C1, C2, f1, l1, f2, l2);
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
          gp_Vec T1, T2;
          C1->D1(W1, dummy_P, T1);
          C2->D1(W1, dummy_P, T2);

          // Construct intersection point
          Handle(geom_intersection_point_cc)
            ipt = new geom_intersection_point_cc( P, uncertainty,
                                                  W1, W2, T1, T2,
                                                  curves(s1), curves(s2) );
          result.Append(ipt);

          this->Plotter().DRAW_POINT(P, Color_Red, "geom_intersect_curves.P");
        }
      }
    }
  }
}
