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
#include <asiAlgo_IntersectCS.h>

// OCCT includes
#include <GeomAPI_IntCS.hxx>
#include <Precision.hxx>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param[in] progress progress notifier.
//! \param[in] plotter  imperative plotter.
asiAlgo_IntersectCS::asiAlgo_IntersectCS(ActAPI_ProgressEntry progress,
                                         ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter)
{}

//-----------------------------------------------------------------------------

//! Performs curve-surface intersection.
//! \param[in]  S      surface.
//! \param[in]  C      curve to intersect with the surface.
//! \param[out] result intersection points.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_IntersectCS::operator()(const Handle(Geom_Surface)&   S,
                                     const Handle(Geom_Curve)&     C,
                                     asiAlgo_IntersectionPointsCS& result)
{
  GeomAPI_IntCS intCS(C, S);
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
    double u, v, w;
    intCS.Parameters(k, u, v, w);

    // Initialize intersection point structure. Notice that the uncertainty
    // is initialized as precision confusion since API of OpenCascade we use
    // does not provide the reached numerical precision.
    Handle(asiAlgo_IntersectionPointCS)
      ipoint = new asiAlgo_IntersectionPointCS( intCS.Point(k),
                                                w,
                                                gp_Pnt2d(u, v),
                                                Precision::Confusion() );
    //
    result.Append(ipoint);
  }
  return true;
}
