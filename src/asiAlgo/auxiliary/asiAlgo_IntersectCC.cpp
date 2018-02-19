//-----------------------------------------------------------------------------
// Created on: 18 February 2018
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

// Own include
#include <asiAlgo_IntersectCC.h>

// OCCT includes
#include <BSplCLib.hxx>
#include <Extrema_ExtCC.hxx>
#include <Extrema_POnCurv.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <GeomConvert.hxx>
#include <Precision.hxx>
#include <Quantity_Parameter.hxx>
#include <TColStd_Array1OfReal.hxx>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param[in] progress progress notifier.
//! \param[in] plotter  imperative plotter.
asiAlgo_IntersectCC::asiAlgo_IntersectCC(ActAPI_ProgressEntry progress,
                                         ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter)
{}

//-----------------------------------------------------------------------------

//! Performs curve-curve intersection.
//! \param[in] C         first curve to intersect.
//! \param[in] cf        first parameter value for the first curve.
//! \param[in] cl        last parameter value for the first curve.
//! \param[in] G         second curve to intersect.
//! \param[in] gf        first parameter value for the second curve.
//! \param[in] gl        last parameter value for the second curve.
//! \param[in] precision intersection precision.
//! \param[out] result   intersection points.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_IntersectCC::operator()(const Handle(Geom_Curve)&     C,
                                     const double                  cf,
                                     const double                  cl,
                                     const Handle(Geom_Curve)&     G,
                                     const double                  gf,
                                     const double                  gl,
                                     const double                  precision,
                                     asiAlgo_IntersectionPointsCC& result)
{
  if ( C.IsNull() || G.IsNull() )
    return false;

  if ( cf > cl || Abs(cf - cl) < RealEpsilon() )
    return false;

  if ( gf > gl || Abs(gf - gl) < RealEpsilon() )
    return false;

  GeomAPI_ExtremaCurveCurve Ex(C, G, cf, cl, gf, gl);
  gp_Pnt P1, P2;
  if ( Ex.TotalNearestPoints(P1, P2) )
  {
    const double uncertainty = P1.Distance(P2);

    if ( uncertainty < precision )
    {
      double W1, W2;
      Ex.TotalLowerDistanceParameters(W1, W2);
      gp_Pnt P = 0.5*( P1.XYZ() + P2.XYZ() );

      Handle(asiAlgo_IntersectionPointCC)
        ipoint = new asiAlgo_IntersectionPointCC(P, W1, W2);

      // Prepare intersection point
      result.Append(ipoint);
    }
  }
  return true;
}
