//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_classify_point_surface.h>

//! Constructor.
//! \param Surf      [in] surface to classify the point against.
//! \param precision [in] precision for the underlying numerical method.
geom_classify_point_surface::geom_classify_point_surface(const Handle(Geom_Surface)& Surf,
                                                         const double                precision)
: m_fPrec(precision), m_SAS(Surf)
{
}

//! Performs point-surface classification.
//! \param P       [in]     point to classify.
//! \param PonS    [out]    point on surface.
//! \param Journal [in/out] journal instance.
//! \return classification result.
geom_membership geom_classify_point_surface::operator()(const gp_Pnt&        P,
                                                        gp_Pnt2d&            PonS,
                                                        ActAPI_ProgressEntry ASitus_NotUsed(Journal))
{
  geom_membership result;
  gp_Pnt2d UV = m_SAS.ValueOfUV(P, m_fPrec);
  if ( m_SAS.Gap() < m_fPrec )
  {
    result = Membership_In;
    PonS   = UV;
  }
  else
    result = Membership_Out;

  return result;
}

//! Performs point-surface classification.
//! \param P       [in]     point to classify.
//! \param Journal [in/out] journal instance.
//! \return classification result.
geom_membership
  geom_classify_point_surface::operator()(const gp_Pnt&        P,
                                          ActAPI_ProgressEntry Journal)
{
  gp_Pnt2d dummy_PonS;
  return this->operator()(P, dummy_PonS, Journal);
}
