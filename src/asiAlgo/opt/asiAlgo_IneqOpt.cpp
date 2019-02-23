//-----------------------------------------------------------------------------
// Created on: 26 April 2018
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
#include <asiAlgo_IneqOpt.h>

// asiAlgo includes
#include <asiAlgo_Timer.h>

// Standard includes
#include <algorithm>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #pragma message("===== warning: DRAW_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiAlgo_IneqOpt::asiAlgo_IneqOpt(ActAPI_ProgressEntry progress,
                                 ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter)
{}

//-----------------------------------------------------------------------------

asiAlgo_IneqOpt::asiAlgo_IneqOpt(const Handle(asiAlgo_IneqOptParams)& params,
                                 ActAPI_ProgressEntry                 progress,
                                 ActAPI_PlotterEntry                  plotter)
: ActAPI_IAlgorithm(progress, plotter)
{
  m_params = params;
}

//-----------------------------------------------------------------------------

bool asiAlgo_IneqOpt::Perform()
{
  // Get dimensions of the problem.
  const int N = m_params->GetN();
  int       M = m_params->GetM();

  /* ===================================================================
   *  Prepare a grid of M-dimensional points corresponding to different
   *  sub-domains of the constraint polytope.
   * =================================================================== */

  // Grid.
  std::vector< t_ineqNCoord<int> > MGrid;
  std::vector<int>                 indices;

  TIMER_NEW
  TIMER_GO

  const int pMin =  (m_params->IsInversionMode() ? -2 : 0);
  const int pMax =  1;

  this->populateLexicographic(M, pMin, pMax, MGrid);

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(m_progress, "Populate M-dimensional grid")

  for ( int k = 0; k < MGrid.size(); ++k )
  {
    indices.push_back(k);

#if defined COUT_DEBUG
    TCollection_AsciiString rName;

    for ( size_t j = 0; j < MGrid[k].Dim; ++j )
    {
      rName += MGrid[k].V[j];

      if ( j < MGrid[k].Dim - 1 )
        rName += " ";
    }

    std::cout << "Next grid point: (" << rName << ")" << std::endl;
#endif
  }

  // Check.
  const int numPtsExpected = int( Pow(pMax - pMin + 1, M) );
  //
  if ( MGrid.size() != numPtsExpected )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Unexpected number of grid points.");
    return false;
  }
  //
  m_progress.SendLogMessage(LogInfo(Normal) << "%1 grid points are prepared in %2-dimensional space."
                                            << int( MGrid.size() ) << M);

#if defined DRAW_DEBUG
  if ( M == 3 )
  {
    for ( size_t k = 0; k < MGrid.size(); ++k )
    {
      m_plotter.DRAW_POINT( gp_Pnt(MGrid[k].V[0], MGrid[k].V[1], MGrid[k].V[2]), Color_White, "MGrid" );
    }
  }
#endif

  // Sort grid points by their modulus.
  std::sort( indices.begin(), indices.end(),
              [&](const int a, const int b)
              {
                return MGrid[a].Modulus() < MGrid[b].Modulus();
              } );

#if defined DRAW_DEBUG
  if ( N == 2 || N == 3 )
  {
    std::vector<Quantity_Color> colors = {Quantity_NOC_GREEN,
                                          Quantity_NOC_YELLOW,
                                          Quantity_NOC_ORANGE,
                                          Quantity_NOC_RED};

    int    colorIndex  = -1;
    double prevModulus = -1;
    //
    for ( size_t k = 0; k < indices.size(); ++k )
    {
      const double modulus = MGrid[ indices[k] ].Modulus();
      //
      if ( modulus > prevModulus )
      {
        prevModulus = modulus;
        colorIndex++;
      }

#if defined COUT_DEBUG
      std::cout << "Next modulus: " << modulus << std::endl;
#endif

      TCollection_AsciiString rName, polytopeName;
      //
      for ( size_t j = 0; j < MGrid[k].Dim; ++j )
      {
        rName += MGrid[k].V[j];

        if ( j < MGrid[k].Dim - 1 )
          rName += " ";
      }
      //
      polytopeName = "polytope ("; polytopeName += rName; polytopeName += ")";

      // Get system for a particular sub-domain.
      Handle(asiAlgo_IneqSystem) system = m_params->GetSystem(MGrid[k]);
      //
      system->Dump( m_plotter,
                   (colorIndex >= colors.size() ? colors[colors.size() - 1] : colors[colorIndex]),
                    polytopeName,
                    false );
    }
  }
#endif

  /* ===================================
   *  Search for the optimal grid point
   * =================================== */

  t_ineqNCoord<double>       x0(N, 0), xsol;
  t_ineqNCoord<int>          r(M, pMax);
  bool                       isOk = true;
  Handle(asiAlgo_IneqSystem) convergedSys;

  for ( int nu = 1; nu <= M; ++nu )
  {
    bool isSolved = false;
    int  pVal     = pMin;

    do
    {
      r.V[nu-1] = pVal;
      //
      Handle(asiAlgo_IneqSystem) nextSys = m_params->GetSystem(r);
      //
      isSolved = nextSys->Solve(x0, xsol, m_progress, m_plotter);

      if ( isSolved )
        convergedSys = nextSys;
      else
        pVal++;
    }
    while ( !isSolved && pVal <= pMax );

    if ( !isSolved )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "Cannot solve.");
      isOk = false;
      break; // If cannot solve for outmost polytope, there is no sense to precise.
    }
  }

#if defined DRAW_DEBUG
  if ( isOk && N == 3 )
  {
    const std::vector< t_ineqNCoord<double> >&
      trace = convergedSys->GetTraceOfConvergence();

    std::vector<gp_XYZ> tracePts;
    for ( size_t jj = 0; jj < trace.size(); ++jj )
      tracePts.push_back( gp_XYZ(trace[jj].V[0], trace[jj].V[1], trace[jj].V[2]) );

    m_plotter.DRAW_POLYLINE(tracePts, Color_Yellow, "convergence" );
  }
#endif

  //// Check best possible system.
  //t_ineqNCoord<int> r(M, 0);
  //bool isOk = m_params->GetCentralSystem(r)->Solve(x0, xsol, m_progress, m_plotter);

  // Set result.
  m_R = r;
  m_X = xsol;

  return isOk;
}

//-----------------------------------------------------------------------------

void asiAlgo_IneqOpt::populateLexicographic(const int                         dim,
                                            const int                         minVal,
                                            const int                         maxVal,
                                            std::vector< t_ineqNCoord<int> >& gridPts) const
{
  if ( dim <= 0 )
    return;

  t_ineqNCoord<int> gridPt(dim, minVal);
  gridPts.push_back(gridPt);

  // Loop over the vector components.
  for ( int k = dim - 1; k >= 0; --k )
  {
    gridPt.Init(minVal);

    // Loop over the possible component values.
    int currentVal = minVal + 1;
    //
    while ( currentVal <= maxVal )
    {
      gridPt.V[k] = currentVal;

      std::vector< t_ineqNCoord<int> > restPositions;
      this->populateLexicographic(dim - k - 1, minVal, maxVal, restPositions);
      //
      for ( int j = 0; j < restPositions.size(); ++j )
      {
        for ( int jj = 0; jj < restPositions[j].Dim; ++jj )
        {
          gridPt.V[k + jj + 1] = restPositions[j].V[jj];
        }
        gridPts.push_back(gridPt);
      }
      if ( !restPositions.size() )
        gridPts.push_back(gridPt);

      currentVal++;
    }
  }
}
