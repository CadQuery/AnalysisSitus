//-----------------------------------------------------------------------------
// Created on: 18 December 2018
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
//    * Neither the name of Sergey Slyadnev nor the
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

// cmdMisc includes
#include <cmdMisc.h>

// asiData includes
#include <asiData_IVCurveNode.h>

#ifdef USE_MOBIUS
  #include <mobius/bspl_UnifyKnots.h>
  #include <mobius/cascade.h>
  #include <mobius/geom_BSplineCurve.h>
  #include <mobius/geom_CoonsSurfaceCubic.h>
  #include <mobius/geom_CoonsSurfaceLinear.h>
  #include <mobius/geom_InterpolateMultiCurve.h>
  #include <mobius/geom_MakeBicubicBSurf.h>
  #include <mobius/geom_SkinSurface.h>

  using namespace mobius;

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #pragma message("===== warning: DRAW_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

ptr<bcurve> FindBCurve(const TCollection_AsciiString& curveName,
                       const Handle(asiTcl_Interp)&   interp)
{
  // Find Node by name.
  Handle(ActAPI_INode) node = interp->GetModel()->FindNodeByName(curveName);
  //
  if ( node.IsNull() || !node->IsKind( STANDARD_TYPE(asiData_IVCurveNode) ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Node '%1' is not a curve."
                                                        << curveName);
    return NULL;
  }
  //
  Handle(asiData_IVCurveNode)
    curveNode = Handle(asiData_IVCurveNode)::DownCast(node);

  // Get B-curve.
  double f, l;
  Handle(Geom_BSplineCurve)
    occtBCurve = Handle(Geom_BSplineCurve)::DownCast( curveNode->GetCurve(f, l) );
  //
  if ( occtBCurve.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "The curve in question is not a B-spline curve.");
    return NULL;
  }

  return cascade::GetMobiusBCurve(occtBCurve);
}

//-----------------------------------------------------------------------------

int MISC_BuildCoonsLinear(const Handle(asiTcl_Interp)& interp,
                          int                          argc,
                          const char**                 argv)
{
  if ( argc != 6 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  /* =================
   *  Get rail curves
   * ================= */

  ptr<bcurve> c0 = FindBCurve(argv[2], interp);
  ptr<bcurve> c1 = FindBCurve(argv[3], interp);
  ptr<bcurve> b0 = FindBCurve(argv[4], interp);
  ptr<bcurve> b1 = FindBCurve(argv[5], interp);

  // Initialize corner points.
  xyz p00 = c0->D0( c0->GetMinParameter() );
  xyz p10 = c0->D0( c0->GetMaxParameter() );
  xyz p01 = c1->D0( c1->GetMinParameter() );
  xyz p11 = c1->D0( c1->GetMaxParameter() );

  /* ===================
   *  Build Coons patch
   * =================== */

  // Build Coons surface.
  ptr<geom_CoonsSurfaceLinear>
    coons = new geom_CoonsSurfaceLinear(c0, c1, b0, b1, p00, p01, p10, p11);

  // Sample patch.
  TCollection_AsciiString ptsName(argv[1]); ptsName += "-pts";
  cmdMisc::DrawSurfPts(interp, coons, ptsName);

  /* =================================
   *  Build components of Boolean sum
   * ================================= */

  // Build P1S.
  ptr<bsurf> P1S;
  Handle(Geom_BSplineSurface) P1Socc;
  {
    // Prepare rail curves.
    std::vector< ptr<bcurve> > rails = {c0, c1};

    // Skin ruled surface through c0 and c1 rails.
    geom_SkinSurface skinner(rails, 1, false);
    //
    if ( !skinner.Perform() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot build P1S.");
      return TCL_ERROR;
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
  ptr<bsurf> P2S;
  Handle(Geom_BSplineSurface) P2Socc;
  {
    // Prepare rail curves.
    std::vector< ptr<bcurve> > rails = {b0, b1};

    // Skin ruled surface through b0 and b1 rails.
    geom_SkinSurface skinner(rails, 1, false);
    //
    if ( !skinner.Perform() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot build P2S.");
      return TCL_ERROR;
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
  ptr<bsurf> P12S;
  Handle(Geom_BSplineSurface) P12Socc;
  {
    std::vector< std::vector<xyz> >
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

  const std::vector< std::vector<xyz> >& polesP1S  = P1S->GetPoles();
  const std::vector< std::vector<xyz> >& polesP2S  = P2S->GetPoles();
  const std::vector< std::vector<xyz> >& polesP12S = P12S->GetPoles();

  const int numPolesU = P1S->GetNumOfPoles_U();
  const int numPolesV = P1S->GetNumOfPoles_V();

  // Compute the resulting poles.
  std::vector< std::vector<xyz> > resPoles;
  //
  for ( int i = 0; i < numPolesU; ++i )
  {
    std::vector<mobius::xyz> col;
    for ( int j = 0; j < numPolesV; ++j )
    {
      xyz resPole = polesP1S[i][j] + polesP2S[i][j] - polesP12S[i][j];
      //
      col.push_back(resPole);
    }
    resPoles.push_back(col);
  }

  // Construct the resulting surface.
  ptr<bsurf>
    mobRes = new bsurf(resPoles, Ucommon, Vcommon, pcommon, qcommon);
  //
  interp->GetPlotter().REDRAW_SURFACE(argv[1],
                                      cascade::GetOpenCascadeBSurface(mobRes),
                                      Color_Default);

  return TCL_OK;
}

#endif

//-----------------------------------------------------------------------------

void cmdMisc::Commands_Coons(const Handle(asiTcl_Interp)&      interp,
                             const Handle(Standard_Transient)& data)
{
  static const char* group = "cmdMisc";

#if defined USE_MOBIUS

  //-------------------------------------------------------------------------//
  interp->AddCommand("build-coons-linear",
    //
    "build-coons-linear resSurf c0 c1 b0 b1\n"
    "\t Builds bilinear Coons patch on the passed boundary curves.",
    //
    __FILE__, group, MISC_BuildCoonsLinear);

#endif
}
