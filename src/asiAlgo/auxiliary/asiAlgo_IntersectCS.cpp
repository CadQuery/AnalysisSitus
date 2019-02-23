//-----------------------------------------------------------------------------
// Created on: 02 March 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018, Sergey Slyadnev
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
#include <asiAlgo_IntersectCS.h>

// OCCT includes
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <IntCurveSurface_HInter.hxx>
#include <Precision.hxx>

//-----------------------------------------------------------------------------

asiAlgo_IntersectCS::asiAlgo_IntersectCS(ActAPI_ProgressEntry progress,
                                         ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm ( progress, plotter ),
  m_fTangencyTolRad ( Precision::Angular() )
{}

//-----------------------------------------------------------------------------

bool asiAlgo_IntersectCS::operator()(const Handle(Geom_Surface)&   S,
                                     const Handle(Geom_Curve)&     C,
                                     asiAlgo_IntersectionPointsCS& result,
                                     bool&                         hasTangentialPoints)
{
  Handle(GeomAdaptor_HCurve)   HC = new GeomAdaptor_HCurve(C);
  Handle(GeomAdaptor_HSurface) HS = new GeomAdaptor_HSurface(S);

  hasTangentialPoints = false;

  // Perform intersection.
  IntCurveSurface_HInter intCS;
  //
  intCS.Perform(HC, HS);
  //
  if ( !intCS.IsDone() )
  {
    m_progress.SendLogMessage( LogErr(Normal) << "Curvature/surface intersection failed" );
    return false;
  }

  // Collect intersection results.
  for ( int k = 1; k <= intCS.NbPoints(); ++k )
  {
    // Get parameters of the intersection point.
    const IntCurveSurface_IntersectionPoint&
      iPoint = intCS.Point(k);

    const double u = iPoint.U();
    const double v = iPoint.V();
    const double w = iPoint.W();

    // Compute direction on curve at the intersection point.
    bool isTangent = false;
    //
    gp_Pnt HC_P;
    gp_Vec HC_D1;
    //
    HC->D1(w, HC_P, HC_D1);
    //
    if ( HC_D1.Magnitude() > RealEpsilon() )
    {
      // Compute surface normal at the intersection point.
      gp_Pnt HS_P;
      gp_Vec HS_D1U, HS_D1V;
      //
      HS->D1(u, v, HS_P, HS_D1U, HS_D1V);
      //
      gp_Vec HS_N = HS_D1U ^ HS_D1V;
      //
      if ( HS_N.Magnitude() > RealEpsilon() )
      {
        gp_Dir dcurve(HC_D1), nsurf(HS_N);

        const double ang     = ACos( dcurve.Dot(nsurf) );
        const double angDiff = Abs(ang - M_PI/2.);

        if ( angDiff < m_fTangencyTolRad )
        {
          isTangent = true;
          if ( !hasTangentialPoints ) hasTangentialPoints = true;
        }
      }
    }

    // Initialize intersection point structure. Notice that the uncertainty
    // is initialized as precision confusion since API of OpenCascade we use
    // does not provide the reached numerical precision.
    Handle(asiAlgo_IntersectionPointCS)
      ipoint = new asiAlgo_IntersectionPointCS( intCS.Point(k).Pnt(),
                                                w,
                                                gp_Pnt2d(u, v),
                                                Precision::Confusion(),
                                                isTangent );
    //
    result.Append(ipoint);
  }
  return true;
}
