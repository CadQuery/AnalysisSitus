//-----------------------------------------------------------------------------
// Created on: 27 October 2018
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
#include <asiAlgo_MeshProjectLine.h>

// OCCT includes
#include <Precision.hxx>

// STL includes
#include <deque>

//-----------------------------------------------------------------------------

asiAlgo_MeshProjectLine::asiAlgo_MeshProjectLine(const Handle(asiAlgo_BVHFacets)& facets,
                                                 ActAPI_ProgressEntry             progress,
                                                 ActAPI_PlotterEntry              plotter)
: ActAPI_IAlgorithm(progress, plotter), m_facets(facets)
{
  // Construct internal algorithm used to project individual points on mesh.
  m_projectPtMesh = new asiAlgo_ProjectPointOnMesh(m_facets, progress, plotter);
}

//-----------------------------------------------------------------------------

bool asiAlgo_MeshProjectLine::Perform(const gp_XYZ&        P1,
                                      const gp_XYZ&        P2,
                                      std::vector<gp_XYZ>& result,
                                      const double         diagRatio) const
{
  // Choose precision.
  const double aabbDiag = m_facets->GetBoundingDiag();
  const double prec     = aabbDiag*diagRatio;

  // Add the first point.
  result.push_back(P1);

  // Prepare deque of points.
  std::deque<gp_XYZ> ptsDeque;
  ptsDeque.push_back(P1);
  ptsDeque.push_back(P2);

  // Limit max iterations.
  const int maxIter = 10000;

  // Subdivide and project.
  int currentIter = 0;
  do
  {
    currentIter++;
    //
    if ( currentIter > maxIter )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "Limit of iterations (%1) was exceeded."
                                               << maxIter);
      return false; // Protect from too long processing.
    }

    const gp_XYZ& prevPt = ptsDeque[0];
    const gp_XYZ& nextPt = ptsDeque[ptsDeque.size() - 1];
    //
    if ( (nextPt - prevPt).Modulus() < Precision::Confusion() )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "Cannot determine next point along the line.");
      return false; // Protect from stucking in the deque with identical points.
    }

    // Compute midpoint.
    const gp_XYZ midPt = 0.5*(prevPt + nextPt);

    // Hit facet for the midpoint.
    gp_Pnt hitMidPt        = m_projectPtMesh->Perform(midPt);
    int    midPtFacetIndex = (m_projectPtMesh->GetTriIdx().size() ? m_projectPtMesh->GetTriIdx()[0] : -1);
    //
    const bool isProjected = (midPtFacetIndex != -1);
    //
    if ( !isProjected )
    {
      m_plotter.DRAW_POINT(midPt, Color_Red, "unproj");
      m_progress.SendLogMessage(LogErr(Normal) << "Cannot project next point on mesh.");
      return false;
    }

    // Check if distance precision is reached.
    if ( (nextPt - prevPt).Modulus() < prec )
    {
      result.push_back(nextPt);

      // Rearrange deque.
      ptsDeque.pop_back();
      ptsDeque.pop_front();
      ptsDeque.push_front( hitMidPt.XYZ() );
    }
    else
      ptsDeque.push_back( hitMidPt.XYZ() );
  }
  while ( ptsDeque.size() > 1 );

  return true;
}
