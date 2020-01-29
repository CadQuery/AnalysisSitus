//-----------------------------------------------------------------------------
// Created on: 28 June 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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
#include <asiAlgo_PatchJointAdaptor.h>

// OCCT includes
#include <ShapeAnalysis_Surface.hxx>

#if defined USE_MOBIUS
  // Mobius includes
  #include <mobius/bspl_UnifyKnots.h>
  #include <mobius/cascade.h>
  #include <mobius/core_Precision.h>
  #include <mobius/geom_BSplineCurve.h>

  // Mobius namespace.
  using namespace mobius;
#endif

//-----------------------------------------------------------------------------

bool asiAlgo_PatchJointAdaptor::AnalyzeJoint(const Handle(Geom_Curve)&   curve,
                                             const Handle(Geom_Surface)& surf,
                                             bool&                       isSurfGoesU,
                                             bool&                       isLeftBound,
                                             double&                     uMin,
                                             double&                     uMax,
                                             double&                     vMin,
                                             double&                     vMax)
{
  if ( curve.IsNull() )
    return false;

  // Get sample point (midpoint) on the joint curve.
  const double f = curve->FirstParameter();
  const double l = curve->LastParameter();
  const double m = (f + l)*0.5;
  //
  gp_Pnt C = curve->Value(m);

  // Projection precision.
  const double projPrec = 1e-4;

  // Invert point to the left surface.
  ShapeAnalysis_Surface sas(surf);
  gp_Pnt2d uv = sas.ValueOfUV(C, projPrec);

  // Step for small perturbation.
  const double d = (l - f)*0.01;

  // Give the sample paramter a small perturbation to define another point
  // on the surface and determine which curvilinear coordinate is effective.
  const double tShifted = m + d;
  gp_Pnt       CShifted = curve->Value(tShifted);

  // Invert the shifted point.
  gp_Pnt2d uvShifted = sas.ValueOfUV(CShifted, projPrec);

  // Get projection coordinates in (U,V) space.
  double Cproj_U      = uv.X();
  double Cproj_V      = uv.Y();
  double Cproj_U_next = uvShifted.X();
  double Cproj_V_next = uvShifted.Y();

  // Initialize output values.
  surf->Bounds(uMin, uMax, vMin, vMax);
  //
  isSurfGoesU = false;
  isLeftBound = false;

  // Define which curvilinear axis works out this shift. Here we rely on the
  // fact that the joint curve is an isoparametric curve for both surfaces.
  const double confPrec    = 1e-5;
  //bool         sameSense   = true;
  //
  if ( fabs(Cproj_U_next - Cproj_U) < confPrec &&
       fabs(Cproj_V_next - Cproj_V) > confPrec )
  {
    isSurfGoesU = false;
    //sameSense   = (Cproj_V_next - Cproj_V) > 0;

    if ( fabs(Cproj_U - uMin) < projPrec )
      isLeftBound = true;
  }
  else if ( fabs(Cproj_U_next - Cproj_U) > confPrec &&
            fabs(Cproj_V_next - Cproj_V) < confPrec )
  {
    isSurfGoesU = true;
    //sameSense   = (Cproj_U_next - Cproj_U) > 0;

    if ( fabs(Cproj_V - vMin) < projPrec )
      isLeftBound = true;
  }
  else
    return false; // Singularity.

  return true;
}

//-----------------------------------------------------------------------------

asiAlgo_PatchJointAdaptor::asiAlgo_PatchJointAdaptor(const Handle(Geom_Curve)&          curve,
                                                     const Handle(Geom_BSplineSurface)& surfLeft,
                                                     const Handle(Geom_BSplineSurface)& surfRight,
                                                     ActAPI_ProgressEntry               progress,
                                                     ActAPI_PlotterEntry                plotter)
: ActAPI_IAlgorithm(progress, plotter)
{
  this->Init(curve, surfLeft, surfRight);
}

//-----------------------------------------------------------------------------

asiAlgo_PatchJointAdaptor::asiAlgo_PatchJointAdaptor(ActAPI_ProgressEntry progress,
                                                     ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter)
{}

//-----------------------------------------------------------------------------

void asiAlgo_PatchJointAdaptor::Init(const Handle(Geom_Curve)&          curve,
                                     const Handle(Geom_BSplineSurface)& surfLeft,
                                     const Handle(Geom_BSplineSurface)& surfRight)
{
  m_curve     = curve;
  m_surfLeft  = surfLeft;
  m_surfRight = surfRight;
}

//-----------------------------------------------------------------------------

bool asiAlgo_PatchJointAdaptor::ExtractIsos(Handle(Geom_BSplineCurve)& isoLeft,
                                            bool&                      isoLeftU,
                                            bool&                      isoLeftMin,
                                            Handle(Geom_BSplineCurve)& isoRight,
                                            bool&                      isoRightU,
                                            bool&                      isoRightMin,
                                            bool&                      areOpposite) const
{
  if ( !this->getIso(m_surfLeft, isoLeft, isoLeftU, isoLeftMin) )
    return false;

  if ( !this->getIso(m_surfRight, isoRight, isoRightU, isoRightMin) )
    return false;

  // Check if the isos are opposite.
  gp_Pnt isoLeftOrigin  = isoLeft->Value  ( isoLeft->FirstParameter() );
  gp_Pnt isoRightOrigin = isoRight->Value ( isoRight->FirstParameter() );
  //
  if ( isoLeftOrigin.Distance(isoRightOrigin) < Precision::Confusion() )
    areOpposite = false;
  else
    areOpposite = true;

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_PatchJointAdaptor::UnifySurfaces(const Handle(Geom_BSplineCurve)& isoLeft,
                                              const bool                       isoLeftU,
                                              const Handle(Geom_BSplineCurve)& isoRight,
                                              const bool                       isoRightU,
                                              const bool                       areOpposite)
{
  this->insertKnotsLeft  (isoLeft, isoRight, isoLeftU, areOpposite);
  this->insertKnotsRight (isoLeft, isoRight, isoRightU, areOpposite);

  m_plotter.DRAW_CURVE(isoLeft,  (isoLeftU  ? Color_Green : Color_Red), true, "isoLeft");
  m_plotter.DRAW_CURVE(isoRight, (isoRightU ? Color_Green : Color_Red), true, "isoRight");
  //
  m_progress.SendLogMessage( LogInfo(Normal) << "Iso-curves are %1."
                                             << (areOpposite ? "opposite" : "non-opposite") );
  //
  m_plotter.DRAW_SURFACE(m_surfLeft,  Color_White, "surfLeftAfter");
  m_plotter.DRAW_SURFACE(m_surfRight, Color_White, "surfRightAfter");

  return true; // Success.
}

//-----------------------------------------------------------------------------

bool asiAlgo_PatchJointAdaptor::IsJointCompatible(const Handle(Geom_BSplineCurve)& isoLeft,
                                                  const Handle(Geom_BSplineCurve)& isoRight,
                                                  const bool                       areOpposite) const
{
#if defined USE_MOBIUS
  // Convert curves to Mobius structures.
  t_ptr<t_bcurve> mbIsoLeft  = cascade::GetMobiusBCurve(isoLeft);
  t_ptr<t_bcurve> mbIsoRight = cascade::GetMobiusBCurve(isoRight);

  // Check the number of poles.
  if ( mbIsoLeft->GetNumOfPoles() != mbIsoRight->GetNumOfPoles() )
  {
    m_progress.SendLogMessage(LogWarn(Normal) << "Different number of poles at joint curves.");
    return false;
  }

  const double tol3d = core_Precision::Resolution3D()*10;

  // Common number of poles can be taken from any curve.
  const int numPoles = mbIsoLeft->GetNumOfPoles();

  // Check if the poles are coincident with the prescribed precision.
  for ( int i = 0; i < numPoles; ++i )
  {
    const t_xyz& P_left  = mbIsoLeft->GetPole(i);
    const t_xyz& P_right = mbIsoRight->GetPole(areOpposite ? numPoles - 1 - i : i);

    // Check distance.
    const double dist = (P_left - P_right).Modulus();
    //
    if ( dist > tol3d )
    {
      m_progress.SendLogMessage(LogWarn(Normal) << "Distance %1 between poles with indices %2 "
                                                   "is greater than confusion tolerance."
                                                << dist << i);

      m_plotter.DRAW_POINT(cascade::GetOpenCascadePnt(P_left),  Color_Red, "P_left");
      m_plotter.DRAW_POINT(cascade::GetOpenCascadePnt(P_right), Color_Red, "P_right");

      return false;
    }
  }

  return true;
#else
  asiAlgo_NotUsed(isoLeft);
  asiAlgo_NotUsed(isoRight);
  asiAlgo_NotUsed(areOpposite);

  m_progress.SendLogMessage(LogErr(Normal) << "Mobius is not available.");
  return false;
#endif
}

//-----------------------------------------------------------------------------

bool asiAlgo_PatchJointAdaptor::AlignControlPoles(const Handle(Geom_BSplineCurve)& isoLeft,
                                                  const bool                       isoLeftU,
                                                  const bool                       isoLeftMin,
                                                  const Handle(Geom_BSplineCurve)& isoRight,
                                                  const bool                       isoRightU,
                                                  const bool                       isoRightMin,
                                                  const bool                       areOpposite)
{
#if defined USE_MOBIUS
  // Contract check.
  if ( !this->IsJointCompatible(isoLeft, isoRight, areOpposite) )
    return false;

  // Convert curves to Mobius structures.
  t_ptr<t_bcurve> mbIsoLeft = cascade::GetMobiusBCurve(isoLeft);

  // Convert surfaces to Mobius structures.
  t_ptr<t_bsurf> mbSurfLeft  = cascade::GetMobiusBSurface(m_surfLeft);
  t_ptr<t_bsurf> mbSurfRight = cascade::GetMobiusBSurface(m_surfRight);

  const int numPoles = isoLeft->NbPoles();

  // Get middle poles.
  const std::vector<t_xyz>& polesMid = mbIsoLeft->GetPoles();

  // Collect poles to change.
  std::vector<t_xyz*> polesLeft, polesRight;
  //
  for ( int k = 0; k < numPoles; ++k )
  {
    t_xyz* pPoleLeft = nullptr, *pPoleRight = nullptr;

    // Get row or column of poles from the left surface.
    if ( isoLeftU && isoLeftMin )
      pPoleLeft = &mbSurfLeft->ChangePole(1, k);
    else if ( isoLeftU && !isoLeftMin )
      pPoleLeft = &mbSurfLeft->ChangePole(mbSurfLeft->GetNumOfPoles_U() - 2, k);
    else if ( !isoLeftU && isoLeftMin )
      pPoleLeft = &mbSurfLeft->ChangePole(k, 1);
    else
      pPoleLeft = &mbSurfLeft->ChangePole(k, mbSurfLeft->GetNumOfPoles_V() - 2);

    // Get row or column of poles from the right surface.
    if ( isoRightU && isoRightMin )
      pPoleRight = &mbSurfRight->ChangePole(1, areOpposite ? numPoles - 1 - k : k);
    else if ( isoRightU && !isoRightMin )
      pPoleRight = &mbSurfRight->ChangePole(mbSurfRight->GetNumOfPoles_U() - 2, areOpposite ? numPoles - 1 - k : k);
    else if ( !isoRightU && isoRightMin )
      pPoleRight = &mbSurfRight->ChangePole(areOpposite ? numPoles - 1 - k : k, 1);
    else
      pPoleRight = &mbSurfRight->ChangePole(areOpposite ? numPoles - 1 - k : k, mbSurfRight->GetNumOfPoles_V() - 2);

    polesLeft.push_back(pPoleLeft);
    polesRight.push_back(pPoleRight);

    m_plotter.DRAW_POINT(cascade::GetOpenCascadePnt(*pPoleLeft),  Color_Red,   "pleft");
    m_plotter.DRAW_POINT(cascade::GetOpenCascadePnt(*pPoleRight), Color_Green, "pright");
    m_plotter.DRAW_POINT(cascade::GetOpenCascadePnt(polesMid[k]), Color_Blue,  "pmiddle");
  }

  // Align poles.
  for ( int k = 0; k < numPoles; ++k )
  {
    t_xyz vLeft2Right = (*polesRight[k] - *polesLeft[k]).Normalized();

    const double dr = (*polesRight[k] - polesMid[k]).Modulus();
    const double dl = (*polesLeft[k] - polesMid[k]).Modulus();

    t_xyz newPoleRight = polesMid[k] + vLeft2Right*dr;
    t_xyz newPoleLeft  = polesMid[k] - vLeft2Right*dl;

    // Update control points.
    polesRight[k]->SetXYZ(newPoleRight);
    polesLeft[k]->SetXYZ(newPoleLeft);
  }

  // Update surfaces.
  m_surfLeft  = cascade::GetOpenCascadeBSurface(mbSurfLeft);
  m_surfRight = cascade::GetOpenCascadeBSurface(mbSurfRight);

  return true; // Success.
#else
  asiAlgo_NotUsed(isoLeft);
  asiAlgo_NotUsed(isoLeftU);
  asiAlgo_NotUsed(isoLeftMin);
  asiAlgo_NotUsed(isoRight);
  asiAlgo_NotUsed(isoRightU);
  asiAlgo_NotUsed(isoRightMin);
  asiAlgo_NotUsed(areOpposite);

  m_progress.SendLogMessage(LogErr(Normal) << "Mobius is not available.");
  return false;
#endif
}

//-----------------------------------------------------------------------------

bool asiAlgo_PatchJointAdaptor::getIso(const Handle(Geom_BSplineSurface)& surf,
                                       Handle(Geom_BSplineCurve)&         iso,
                                       bool&                              isoU,
                                       bool&                              isoMin) const
{
  bool isSurfGoesU, isLeftBound;
  double uMin, uMax, vMin, vMax;
  //
  if ( !AnalyzeJoint(m_curve, surf, isSurfGoesU, isLeftBound, uMin, uMax, vMin, vMax) )
    return false;

  if ( isSurfGoesU )
  {
    iso  = Handle(Geom_BSplineCurve)::DownCast( surf->VIso(isLeftBound ? vMin : vMax) );
    isoU = false;
  }
  else
  {
    iso  = Handle(Geom_BSplineCurve)::DownCast( surf->UIso(isLeftBound ? uMin : uMax) );
    isoU = true;
  }

  isoMin = isLeftBound;

  return true;
}

//-----------------------------------------------------------------------------

void asiAlgo_PatchJointAdaptor::insertKnotsLeft(const Handle(Geom_BSplineCurve)& isoLeft,
                                                const Handle(Geom_BSplineCurve)& isoRight,
                                                const bool                       isoLeftU,
                                                const bool                       areOpposite)
{
#if defined USE_MOBIUS
  // Convert curves to Mobius structures.
  t_ptr<t_bcurve> mbIsoLeft  = cascade::GetMobiusBCurve(isoLeft);
  t_ptr<t_bcurve> mbIsoRight = cascade::GetMobiusBCurve(isoRight);

  // Convert surfaces to Mobius structures.
  t_ptr<t_bsurf> mbSurfLeft  = cascade::GetMobiusBSurface(m_surfLeft);
  t_ptr<t_bsurf> mbSurfRight = cascade::GetMobiusBSurface(m_surfRight);

  std::vector<double> leftKnots  = mbIsoLeft  ->GetKnots();
  std::vector<double> rightKnots = mbIsoRight ->GetKnots();

  if ( areOpposite )
    for ( size_t k = 0; k < rightKnots.size(); ++k )
      rightKnots[k] = 1 - rightKnots[k];

  // Collect knots.
  std::vector< std::vector<double> > U_all;
  U_all.push_back( leftKnots );
  U_all.push_back( rightKnots );

  // Compute complementary knots.
  bspl_UnifyKnots Unify;
  std::vector< std::vector<double> > X = Unify(U_all);

  // Insert knots to the left surface.
  for ( size_t k = 0; k < X[0].size(); ++k )
  {
    if ( isoLeftU )
      mbSurfLeft->InsertKnot_V(X[0][k]);
    else
      mbSurfLeft->InsertKnot_U(X[0][k]);
  }

  // Update left surface.
  m_surfLeft = cascade::GetOpenCascadeBSurface(mbSurfLeft);
#else
  asiAlgo_NotUsed(isoLeft);
  asiAlgo_NotUsed(isoRight);
  asiAlgo_NotUsed(isoLeftU);
  asiAlgo_NotUsed(areOpposite);

  m_progress.SendLogMessage(LogErr(Normal) << "Mobius is not available.");
#endif
}

//-----------------------------------------------------------------------------

void asiAlgo_PatchJointAdaptor::insertKnotsRight(const Handle(Geom_BSplineCurve)& isoLeft,
                                                 const Handle(Geom_BSplineCurve)& isoRight,
                                                 const bool                       isoRightU,
                                                 const bool                       areOpposite)
{
#if defined USE_MOBIUS
  // Convert curves to Mobius structures.
  t_ptr<t_bcurve> mbIsoLeft  = cascade::GetMobiusBCurve(isoLeft);
  t_ptr<t_bcurve> mbIsoRight = cascade::GetMobiusBCurve(isoRight);

  // Convert surfaces to Mobius structures.
  t_ptr<t_bsurf> mbSurfLeft  = cascade::GetMobiusBSurface(m_surfLeft);
  t_ptr<t_bsurf> mbSurfRight = cascade::GetMobiusBSurface(m_surfRight);

  std::vector<double> leftKnots  = mbIsoLeft  ->GetKnots();
  std::vector<double> rightKnots = mbIsoRight ->GetKnots();

  if ( areOpposite )
    for ( size_t k = 0; k < leftKnots.size(); ++k )
      leftKnots[k] = 1 - leftKnots[k];

  // Collect knots.
  std::vector< std::vector<double> > U_all;
  U_all.push_back( leftKnots );
  U_all.push_back( rightKnots );

  // Compute complementary knots.
  bspl_UnifyKnots Unify;
  std::vector< std::vector<double> > X = Unify(U_all);

  // Insert knots to the right surface.
  for ( size_t k = 0; k < X[1].size(); ++k )
  {
    if ( isoRightU )
      mbSurfRight->InsertKnot_V(X[1][k]);
    else
      mbSurfRight->InsertKnot_U(X[1][k]);
  }

  // Update right surface.
  m_surfRight = cascade::GetOpenCascadeBSurface(mbSurfRight);
#else
  asiAlgo_NotUsed(isoLeft);
  asiAlgo_NotUsed(isoRight);
  asiAlgo_NotUsed(isoRightU);
  asiAlgo_NotUsed(areOpposite);

  m_progress.SendLogMessage(LogErr(Normal) << "Mobius is not available.");
#endif
}
