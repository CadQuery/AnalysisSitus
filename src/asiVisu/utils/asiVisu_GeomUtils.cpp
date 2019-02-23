//-----------------------------------------------------------------------------
// Created on: 13 August 2018
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
#include <asiVisu_GeomUtils.h>

// OCCT includes
#include <Precision.hxx>

//-----------------------------------------------------------------------------

static void PlotCurve(const Adaptor3d_Curve& C,
                      double&                f,
                      const double           h,
                      const gp_Pnt&          Pf,
                      const gp_Pnt&          Pl,
                      std::vector<gp_Pnt>&   result)
{
  double IsoRatio = 1.001;
  gp_Pnt Pm = C.Value(f + h);

  double dfLength = Pf.Distance(Pl);

  if ( dfLength < Precision::Confusion() || 
       Pm.Distance(Pf) + Pm.Distance(Pl) <= IsoRatio*dfLength )
  {
    result.push_back(Pl);
  }
  else
  {
    PlotCurve(C, f, h/2., Pf, Pm, result);
    double t = f + h;
    PlotCurve(C, t, h/2., Pm, Pl, result);
  }
}

//-----------------------------------------------------------------------------

void asiVisu_GeomUtils::DiscretizeCurve(const Adaptor3d_Curve& C,
                                        const int              discrete,
                                        std::vector<gp_Pnt>&   result)
{
  const int numIntervals = C.NbIntervals(GeomAbs_CN);
  TColStd_Array1OfReal TI(1, numIntervals + 1);
  C.Intervals(TI, GeomAbs_CN);

  gp_Pnt P = C.Value( C.FirstParameter() );
  gp_Pnt firstP = P, nextP;

  // Add first point.
  result.push_back(P);

  // Process intervals of constant continuity.
  for ( int intrv = 1; intrv <= numIntervals; ++intrv )
  {
    double       t     =  TI(intrv);
    const double step  = (TI(intrv + 1) - t) / discrete;
    const int    nIter =  discrete / 2;

    for ( int j = 1; j < nIter; ++j )
    {
      const double t1 = t + step*2.;
      nextP = C.Value(t1);
      PlotCurve(C, t, step, firstP, nextP, result);
      firstP = nextP;
      t = t1;
    }
  }

  // Add last point.
  result.push_back( C.Value( C.LastParameter() ) );
}
