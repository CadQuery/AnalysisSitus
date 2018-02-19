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
#include <asiAlgo_IntersectSS.h>

// OCCT includes
#include <GeomAPI_IntSS.hxx>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param[in] progress progress notifier.
//! \param[in] plotter  imperative plotter.
asiAlgo_IntersectSS::asiAlgo_IntersectSS(ActAPI_ProgressEntry progress,
                                         ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter)
{}

//-----------------------------------------------------------------------------

//! Performs surface-surface intersection.
//! \param[in]  S1        curve to intersect with the surface.
//! \param[in]  S2        first parameter value.
//! \param[in]  precision last parameter value.
//! \param[out] result    intersection points.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_IntersectSS::operator()(const Handle(Geom_Surface)&   S1,
                                     const Handle(Geom_Surface)&   S2,
                                     const double                  precision,
                                     asiAlgo_IntersectionCurvesSS& result)
{
  GeomAPI_IntSS intSS(S1, S2, precision);
  //
  if ( !intSS.IsDone() )
  {
    m_progress.SendLogMessage( LogErr(Normal) << "Surface/surface intersection failed" );
    return false;
  }

  // Collect intersection results
  for ( int k = 1; k <= intSS.NbLines(); ++k )
  {
    Handle(asiAlgo_IntersectionCurveSS)
      icurve = new asiAlgo_IntersectionCurveSS(precision, intSS.Line(k), S1, S2);
    //
    result.Append(icurve);
  }
  return true;
}
