//-----------------------------------------------------------------------------
// Created on: 21 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_ClassifyPointFace.h>

// OCCT includes
#include <Bnd_Box.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepBndLib.hxx>
#include <Geom_BoundedSurface.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomLib.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #pragma message("===== warning: DRAW_DEBUG is enabled")
#endif

//! Constructor.
//! \param F          [in] face to classify the point against.
//! \param inaccuracy [in] inaccuracy of the input geometry.
//! \param precision  [in] numerical precision for optimization.
//! \param progress   [in] progress notifier.
//! \param plotter    [in] imperative plotter.
asiAlgo_ClassifyPointFace::asiAlgo_ClassifyPointFace(const TopoDS_Face&   F,
                                                     const double         inaccuracy,
                                                     const double         precision,
                                                     ActAPI_ProgressEntry progress,
                                                     ActAPI_PlotterEntry  plotter)
: m_F           (F),
  m_fInaccuracy (inaccuracy),
  m_fPrecision  (precision),
  m_progress    (progress),
  m_plotter     (plotter)
{
  m_fclass.Init(m_F, precision);

  // Get U-V bounds.
  BRepTools::UVBounds(m_F, m_fUmin, m_fUmax, m_fVmin, m_fVmax);
}

//! Performs point-face classification.
//! \param PonS [in] point to classify.
//! \return classification result.
asiAlgo_Membership
  asiAlgo_ClassifyPointFace::operator()(const gp_Pnt2d& PonS)
{
  const TopAbs_State state = m_fclass.Perform(PonS);

  if ( state == TopAbs_IN )
    return Membership_In;
  if ( state == TopAbs_ON )
    return Membership_On;
  if ( state == TopAbs_OUT )
    return Membership_Out;

  return Membership_Unknown;
}

//! Performs point-face classification.
//! \param P             [in] point to classify.
//! \param checkDistance [in] indicates whether to check distance from the
//!                           three-dimensional point (the one being classified)
//!                           and the host surface of the face. As point inversion
//!                           is essentially a projection, even a distant point
//!                           may yield UV coordinates lying INSIDE the face
//!                           domain. To avoid this, <checkGap> argument should be
//!                           turned ON. If you are Ok with such false-positives,
//!                           or if you a priori know that the point is close to
//!                           the face (i.e. in visually lies on the face), then
//!                           this flag can be turned off.
//! \return classification result.
asiAlgo_Membership
  asiAlgo_ClassifyPointFace::operator()(const gp_Pnt& P,
                                        const bool    checkDistance)
{
  gp_Pnt2d UV;
  return this->operator()(P, checkDistance, UV);
}

//! Performs point-face classification.
//! \param P             [in]  point to classify.
//! \param checkDistance [in]  indicates whether to check distance from the
//!                            three-dimensional point (the one being classified)
//!                            and the host surface of the face. As point inversion
//!                            is essentially a projection, even a distant point
//!                            may yield UV coordinates lying INSIDE the face
//!                            domain. To avoid this, <checkGap> argument should be
//!                            turned ON. If you are Ok with such false-positives,
//!                            or if you a priori know that the point is close to
//!                            the face (i.e. in visually lies on the face), then
//!                            this flag can be turned off.
//! \param UV            [out] image of point on a surface.
//! \return classification result.
asiAlgo_Membership
  asiAlgo_ClassifyPointFace::operator()(const gp_Pnt& P,
                                        const bool    checkDistance,
                                        gp_Pnt2d&     UV)
{
  Handle(Geom_Surface) S = Handle(Geom_Surface)::DownCast( BRep_Tool::Surface(m_F)->Copy() );

  // TODO: is the following useful?
  if ( S->IsKind( STANDARD_TYPE(Geom_BoundedSurface) ) )
  {
    Handle(Geom_BoundedSurface) BS = Handle(Geom_BoundedSurface)::DownCast(S);

    Bnd_Box B;
    BRepBndLib::Add(m_F, B);

    // That's just an empiric value. Theoretically you may use just any
    // positive number above resolution to avoid boundary snapping problem
    const double L = ( ( B.CornerMax().XYZ() - B.CornerMin().XYZ() ) ).Modulus();

    // Extend in all parametric directions
    GeomLib::ExtendSurfByLength(BS, L, 1, 1, 0);
    GeomLib::ExtendSurfByLength(BS, L, 1, 1, 1);
    GeomLib::ExtendSurfByLength(BS, L, 1, 0, 0);
    GeomLib::ExtendSurfByLength(BS, L, 1, 0, 1);

    // Use extended surface
    S = BS;
  }

  // Invert point
  double PU, PV;
  GeomAPI_ProjectPointOnSurf proj(P, S, m_fUmin, m_fUmax, m_fVmin, m_fVmax);
  //
  if ( !proj.IsDone() || proj.NbPoints() < 1 )
    return Membership_Out;

  proj.Parameters(1, PU, PV);
  UV = gp_Pnt2d(PU, PV);
  //
  if ( checkDistance && (proj.Distance(1) > 100*m_fInaccuracy) ) // We are quite tolerant here
    return Membership_Out;

  return this->operator()(UV);

  // NOTICE: it is not easy to classify a 3D point even if we know how to classify
  //         the 2D one. Indeed, if we attempt simply to project the 3D point
  //         onto the surface in order to proceed with the known 2D case, it
  //         will not work for the bounded surfaces (rectangular, triangular
  //         of whatever else) if the projected point falls out of the surface
  //         domain. Some OCCT tools like ShapeAnalysis_Surface will snap
  //         the projection image to the surface boundary, so it will always
  //         get ON membership, which is obviously not correct.
  //
  //         Having said this, we may consider two options: 1) either extend
  //         the surface domain, but it is not straightforward to estimate
  //         the required length for the curved surfaces 2) or to apply
  //         different point-face classification routine which is essentially
  //         independent from 2D classification.
}
