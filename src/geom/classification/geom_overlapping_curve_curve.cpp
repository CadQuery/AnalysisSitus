//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_overlapping_curve_curve.h>

//! Constructor.
//! \param Curve     [in] curve to check overlapping with.
//! \param f         [in] first parameter on the working curve.
//! \param l         [in] last parameter on the working curve.
//! \param precision [in] precision for the underlying classification method.
geom_overlapping_curve_curve::geom_overlapping_curve_curve(const Handle(Geom_Curve)& Curve,
                                                           const double              f,
                                                           const double              l,
                                                           const double              precision)
: m_fPrec(precision), m_curve(Curve), m_fFirst(f), m_fLast(l)
{
}

//! Performs curve-curve overlapping analysis.
//! \param Curve   [in]     curve to classify.
//! \param f       [in]     first parameter on the probe curve.
//! \param l       [in]     last parameter on the probe curve.
//! \param Journal [in/out] journal instance.
//! \return classification result.
geom_overlapping
  geom_overlapping_curve_curve::operator()(const Handle(Geom_Curve)& ASitus_NotUsed(Curve),
                                           const double              ASitus_NotUsed(f),
                                           const double              ASitus_NotUsed(l),
                                           ActAPI_ProgressEntry      ASitus_NotUsed(Journal))
{
  // TODO: overlapping has to be checked !!!

  return Overlapping_No;
}
