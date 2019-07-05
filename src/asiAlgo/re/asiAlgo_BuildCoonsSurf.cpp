//-----------------------------------------------------------------------------
// Created on: 26 December 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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
#include <asiAlgo_BuildCoonsSurf.h>

#ifdef USE_MOBIUS
  #include <mobius/bspl_UnifyKnots.h>
  #include <mobius/cascade.h>
  #include <mobius/geom_BSplineCurve.h>
  #include <mobius/geom_CoonsSurfaceCubic.h>
  #include <mobius/geom_CoonsSurfaceLinear.h>
  #include <mobius/geom_MakeBicubicBSurf.h>
  #include <mobius/geom_SkinSurface.h>
  #include <mobius/geom_UnifyBCurves.h>

  using namespace mobius;
#endif

//-----------------------------------------------------------------------------

asiAlgo_BuildCoonsSurf::asiAlgo_BuildCoonsSurf(const Handle(Geom_BSplineCurve)& c0,
                                               const Handle(Geom_BSplineCurve)& c1,
                                               const Handle(Geom_BSplineCurve)& b0,
                                               const Handle(Geom_BSplineCurve)& b1,
                                               ActAPI_ProgressEntry             progress,
                                               ActAPI_PlotterEntry              plotter)
: ActAPI_IAlgorithm(progress, plotter)
{
  m_c0 = c0;
  m_c1 = c1;
  m_b0 = b0;
  m_b1 = b1;
}

//-----------------------------------------------------------------------------

bool asiAlgo_BuildCoonsSurf::Perform()
{
  return this->performBilinear();
}

//-----------------------------------------------------------------------------

bool asiAlgo_BuildCoonsSurf::performBilinear()
{
#ifdef USE_MOBIUS
  // Convert curves to Mobius form.
  t_ptr<t_bcurve> c0 = cascade::GetMobiusBCurve(m_c0);
  t_ptr<t_bcurve> c1 = cascade::GetMobiusBCurve(m_c1);
  t_ptr<t_bcurve> b0 = cascade::GetMobiusBCurve(m_b0);
  t_ptr<t_bcurve> b1 = cascade::GetMobiusBCurve(m_b1);

  // Draw.
  m_plotter.REDRAW_CURVE("c0", m_c0, Color_Red);
  m_plotter.REDRAW_CURVE("c1", m_c1, Color_Red);
  m_plotter.REDRAW_CURVE("b0", m_b0, Color_Blue);
  m_plotter.REDRAW_CURVE("b1", m_b1, Color_Blue);

  // Make `c` curves compatible.
  {
    geom_UnifyBCurves unifyCurves(NULL, NULL);
    //
    unifyCurves.AddCurve(c0);
    unifyCurves.AddCurve(c1);

    if ( !unifyCurves.Perform() )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "Failed to unify 'c' curves.");
      return false;
    }

    // Update curves.
    c0 = unifyCurves.GetResult()[0];
    c1 = unifyCurves.GetResult()[1];
  }

  // Make `b` curves compatible.
  {
    geom_UnifyBCurves unifyCurves(NULL, NULL);
    //
    unifyCurves.AddCurve(b0);
    unifyCurves.AddCurve(b1);

    if ( !unifyCurves.Perform() )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "Failed to unify 'b' curves.");
      return false;
    }

    // Update curves.
    b0 = unifyCurves.GetResult()[0];
    b1 = unifyCurves.GetResult()[1];
  }

  // Initialize corner points.
  t_xyz p00 = c0->D0( c0->GetMinParameter() );
  t_xyz p10 = c0->D0( c0->GetMaxParameter() );
  t_xyz p01 = c1->D0( c1->GetMinParameter() );
  t_xyz p11 = c1->D0( c1->GetMaxParameter() );

  // Draw.
  m_plotter.REDRAW_POINT("p00", cascade::GetOpenCascadePnt(p00), Color_Yellow);
  m_plotter.REDRAW_POINT("p01", cascade::GetOpenCascadePnt(p10), Color_Yellow);
  m_plotter.REDRAW_POINT("p10", cascade::GetOpenCascadePnt(p01), Color_Yellow);
  m_plotter.REDRAW_POINT("p11", cascade::GetOpenCascadePnt(p11), Color_Yellow);

  /* =================================
   *  Build components of Boolean sum
   * ================================= */

  // Build P1S.
  t_ptr<t_bsurf> P1S;
  Handle(Geom_BSplineSurface) P1Socc;
  {
    // Prepare rail curves.
    std::vector< t_ptr<t_bcurve> > rails = {c0, c1};

    // Skin ruled surface through c0 and c1 rails.
    geom_SkinSurface skinner(rails, 1, false);
    //
    if ( !skinner.Perform() )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "Cannot build P1S.");
      return false;
    }
    //
    P1S = skinner.GetResult();

    // Convert to OCC form to run IncreaseDegree() method which does not
    // currently exist in Mobius.
    P1Socc = cascade::GetOpenCascadeBSurface(P1S);

    // Elevate degree.
    P1Socc->IncreaseDegree(3, 3);

    // Convert back to Mobius.
    P1S = cascade::GetMobiusBSurface(P1Socc);
  }

  // Build P2S.
  t_ptr<t_bsurf> P2S;
  Handle(Geom_BSplineSurface) P2Socc;
  {
    // Prepare rail curves.
    std::vector< t_ptr<t_bcurve> > rails = {b0, b1};

    // Skin ruled surface through b0 and b1 rails.
    geom_SkinSurface skinner(rails, 1, false);
    //
    if ( !skinner.Perform() )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "Cannot build P2S.");
      return false;
    }
    //
    P2S = skinner.GetResult();

    // Flip UV coordinates of P2S to be compliant with other surfaces.
    P2S->ExchangeUV();

    // Convert to OCC form to run IncreaseDegree() method which does not
    // currently exist in Mobius.
    P2Socc = cascade::GetOpenCascadeBSurface(P2S);

    // Elevate degree.
    P2Socc->IncreaseDegree(3, 3);

    // Convert back to Mobius.
    P2S = cascade::GetMobiusBSurface(P2Socc);
  }

  // Build P12S.
  t_ptr<t_bsurf> P12S;
  Handle(Geom_BSplineSurface) P12Socc;
  {
    std::vector< std::vector<t_xyz> >
      poles = { {p00, p01},
                {p10, p11} };

    // Define knot vector.
    std::vector<double> T = {0, 0, 1, 1};

    // Construct directly.
    P12S = new geom_BSplineSurface(poles, T, T, 1, 1);

    // Convert to OCC form to run IncreaseDegree() method which does not
    // currently exist in Mobius.
    P12Socc = cascade::GetOpenCascadeBSurface(P12S);

    // Elevate degree.
    P12Socc->IncreaseDegree(3, 3);

    // Convert back to Mobius.
    P12S = cascade::GetMobiusBSurface(P12Socc);
  }

  /* =================================
   *  Unify components of Boolean sum
   * ================================= */

  enum PS
  {
    PS_P1S = 0,
    PS_P2S,
    PS_P12S
  };

  // All U knots.
  std::vector< std::vector<double> >
    knotVectors_U = { P1S->GetKnots_U(),
                      P2S->GetKnots_U(),
                      P12S->GetKnots_U() };

  // All V knots.
  std::vector< std::vector<double> >
    knotVectors_V = { P1S->GetKnots_V(),
                      P2S->GetKnots_V(),
                      P12S->GetKnots_V() };

  // Compute addendum knots.
  bspl_UnifyKnots unifyKnots;
  //
  std::vector< std::vector<double> > U_addendums = unifyKnots(knotVectors_U);
  std::vector< std::vector<double> > V_addendums = unifyKnots(knotVectors_V);

  // Insert U knots to P1S.
  for ( size_t ii = 0; ii < U_addendums[PS_P1S].size(); ++ii )
    P1S->InsertKnot_U(U_addendums[PS_P1S][ii]);

  // Insert V knots to P1S.
  for ( size_t ii = 0; ii < V_addendums[PS_P1S].size(); ++ii )
    P1S->InsertKnot_V(V_addendums[PS_P1S][ii]);

  // Insert U knots to P2S.
  for ( size_t ii = 0; ii < U_addendums[PS_P2S].size(); ++ii )
    P2S->InsertKnot_U(U_addendums[PS_P2S][ii]);

  // Insert V knots to P2S.
  for ( size_t ii = 0; ii < V_addendums[PS_P2S].size(); ++ii )
    P2S->InsertKnot_V(V_addendums[PS_P2S][ii]);

  // Insert U knots to P12S.
  for ( size_t ii = 0; ii < U_addendums[PS_P12S].size(); ++ii )
    P12S->InsertKnot_U(U_addendums[PS_P12S][ii]);

  // Insert V knots to P12S.
  for ( size_t ii = 0; ii < V_addendums[PS_P12S].size(); ++ii )
    P12S->InsertKnot_V(V_addendums[PS_P12S][ii]);

  // Draw.
  m_plotter.REDRAW_SURFACE("P1S",  cascade::GetOpenCascadeBSurface(P1S),  Color_Default);
  m_plotter.REDRAW_SURFACE("P2S",  cascade::GetOpenCascadeBSurface(P2S),  Color_Default);
  m_plotter.REDRAW_SURFACE("P12S", cascade::GetOpenCascadeBSurface(P12S), Color_Default);

  // Common knots.
  const std::vector<double>& Ucommon = P1S->GetKnots_U();
  const std::vector<double>& Vcommon = P1S->GetKnots_V();

  // Common degrees.
  const int pcommon = P1S->GetDegree_U();
  const int qcommon = P1S->GetDegree_V();

  /* =============================
   *  Compute Boolean sum surface
   * ============================= */

  // Now all patches are of the same degrees and defined on identical knot
  // vectors. It means that all patches are defined on the same basis. Therefore,
  // we can now produce a Boolean sum.

  const std::vector< std::vector<t_xyz> >& polesP1S  = P1S->GetPoles();
  const std::vector< std::vector<t_xyz> >& polesP2S  = P2S->GetPoles();
  const std::vector< std::vector<t_xyz> >& polesP12S = P12S->GetPoles();

  const int numPolesU = P1S->GetNumOfPoles_U();
  const int numPolesV = P1S->GetNumOfPoles_V();

  // Compute the resulting poles.
  std::vector< std::vector<t_xyz> > resPoles;
  //
  for ( int i = 0; i < numPolesU; ++i )
  {
    std::vector<t_xyz> col;
    for ( int j = 0; j < numPolesV; ++j )
    {
      t_xyz resPole = polesP1S[i][j] + polesP2S[i][j] - polesP12S[i][j];
      //
      col.push_back(resPole);
    }
    resPoles.push_back(col);
  }

  // Construct the resulting surface.
  t_ptr<t_bsurf>
    mobRes = new t_bsurf(resPoles, Ucommon, Vcommon, pcommon, qcommon);

  // Convert to OCCT form.
  m_result = cascade::GetOpenCascadeBSurface(mobRes);
  return true;
#else
  m_progress.SendLogMessage(LogErr(Normal) << "This functionality is not available.");
  return false;
#endif
}
