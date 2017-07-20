//-----------------------------------------------------------------------------
// Created on: 21 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_ClassifyPointFace.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <Geom_BoundedSurface.hxx>
#include <GeomLib.hxx>
#include <ShapeAnalysis_Surface.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

//! Constructor.
//! \param F          [in] face to classify the point against.
//! \param inaccuracy [in] inaccuracy of the input geometry.
//! \param precision  [in] numerical precision for optimization.
asiAlgo_ClassifyPointFace::asiAlgo_ClassifyPointFace(const TopoDS_Face& F,
                                                     const double       inaccuracy,
                                                     const double       precision)
: m_F(F), m_fInaccuracy(inaccuracy), m_fPrecision(precision)
{
  m_fclass.Init(m_F, precision);
}

//! Performs point-face classification.
//! \param PonS    [in]     point to classify.
//! \param Journal [in/out] journal instance.
//! \return classification result.
asiAlgo_Membership
  asiAlgo_ClassifyPointFace::operator()(const gp_Pnt2d&      PonS,
                                        ActAPI_ProgressEntry Journal)
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
//! \param P        [in]     point to classify.
//! \param checkGap [in]     indicates whether to check gap.
//! \param Journal  [in/out] journal instance.
//! \return classification result.
asiAlgo_Membership
  asiAlgo_ClassifyPointFace::operator()(const gp_Pnt&        P,
                                        const bool           checkGap,
                                        ActAPI_ProgressEntry Journal)
{
  gp_Pnt2d UV;
  return this->operator()(P, checkGap, UV, Journal);
}

//! Performs point-face classification.
//! \param P        [in]     point to classify.
//! \param checkGap [in]     indicates whether to check gap.
//! \param UV       [out]    image of point on a surface.
//! \param Journal  [in/out] journal instance.
//! \return classification result.
asiAlgo_Membership
  asiAlgo_ClassifyPointFace::operator()(const gp_Pnt&        P,
                                        const bool           checkGap,
                                        gp_Pnt2d&            UV,
                                        ActAPI_ProgressEntry Journal)
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

  ShapeAnalysis_Surface sas(S);
  UV = sas.ValueOfUV(P, m_fPrecision);
  if ( checkGap && (sas.Gap() > 100*m_fInaccuracy) ) // We are quite tolerant here
    return Membership_Out;

#if defined DRAW_DEBUG
  DRAW_INITGROUP(asiAlgo_ClassifyPointFace)
  DRAW_POINT(UV, asiAlgo_ClassifyPointFace, Draw_rouge, Draw_Plus)
#endif

  return this->operator()(UV, Journal);

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
