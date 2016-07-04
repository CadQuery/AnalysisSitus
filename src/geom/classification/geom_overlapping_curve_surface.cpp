//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_overlapping_curve_surface.h>

// Geometry includes
#include <geom_classify_point_surface.h>

// OCCT includes
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>

//! Constructor.
//! \param Surf      [in] surface to classify the curve against.
//! \param precision [in] precision for the underlying classification method.
geom_overlapping_curve_surface::geom_overlapping_curve_surface(const Handle(Geom_Surface)& Surf,
                                                               const double                precision)
: m_fPrec(precision), m_surf(Surf)
{
}

//! Performs curve-surface classification.
//! \param Curve   [in]     curve to classify.
//! \param Journal [in/out] journal instance.
//! \return classification result.
geom_overlapping
  geom_overlapping_curve_surface::operator()(const Handle(Geom_Curve)& Curve,
                                             ActAPI_ProgressEntry      Journal)
{
  geom_classify_point_surface ClassPtSurf(m_surf, m_fPrec);

  // In case of a plane, it is enough to check 2 points
  if ( m_surf->IsInstance( STANDARD_TYPE(Geom_Plane) ) )
  {
    if ( Curve->IsInstance( STANDARD_TYPE(Geom_Line) ) )
    {
      Handle(Geom_Line) Line = Handle(Geom_Line)::DownCast(Curve);

      const gp_Pnt& P1 = Line->Position().Location();
      gp_Pnt        P2 = Line->Position().Direction().XYZ();

      if ( ( ClassPtSurf(P1, Journal) & Membership_InOn ) &&
           ( ClassPtSurf(P2, Journal) & Membership_InOn ) )
        return Overlapping_Yes;
    }
    // TODO: there are more cases to consider for curves
  }
  // TODO: there are more cases to consider for surfaces

  return Overlapping_No;
}
