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
#include <asiAlgo_IneqOpt.h>

// Standard includes
#include <algorithm>

#define COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

#define DRAW_DEBUG
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
  const int M = m_params->GetM();

  /* ===================================================================
   *  Prepare a grid of M-dimensional points corresponding to different
   *  sub-domains of the constraint polytope.
   * =================================================================== */

  // Grid.
  std::vector< asiAlgo_IneqSystem::t_ncoord<int> > MGrid;
  std::vector<int>                                 indices;

  // Create M-dimensional origin point and add it to the grid.
  asiAlgo_IneqSystem::t_ncoord<int> MOrigin(M, 0);
  //
  MGrid.push_back(MOrigin);

  // Calculate all permutations of all possible combinations of 1's and 0's.
  for ( int j = 1; j < M; ++j )
  {
    this->computePermutations(M, j, MGrid);
  }

  // Add all 1's.
  asiAlgo_IneqSystem::t_ncoord<int> MOnes(M, 1);
  //
  MGrid.push_back(MOnes);
  //
  indices.push_back( int( indices.size() ) );

  // Check.
  const int numPtsExpected = int( Pow(2, M) );
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
  if ( M == 2 || M == 3 )
  {
    for ( size_t k = 0; k < MGrid.size(); ++k )
    {
      if ( M == 2 )
        m_plotter.DRAW_POINT( gp_Pnt2d(MGrid[k].V[0], MGrid[k].V[1]), Color_White, "MGrid" );
      else
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
  if ( N == 2 || N ==3 )
  {
    std::vector<Quantity_Color> colors = {Quantity_NOC_GREEN,
                                          Quantity_NOC_YELLOW,
                                          Quantity_NOC_ORANGE,
                                          Quantity_NOC_RED};

    int    colorIndex  = -1;
    double prevModulus = -1;
    //
    for ( size_t k = 0; k < MGrid.size(); ++k )
    {
      const double modulus = MGrid[k].Modulus();
      //
      if ( modulus > prevModulus )
      {
        prevModulus = modulus;
        colorIndex++;
      }

#if defined COUT_DEBUG
      std::cout << "Next modulus: " << modulus << std::endl;
#endif

      // Get system for a particular sub-domain.
      Handle(asiAlgo_IneqSystem) system = m_params->GetSystem(MGrid[k]);
      //
      system->Dump(m_plotter, (colorIndex >= colors.size() ? colors[colors.size() - 1] : colors[colorIndex]));
    }
  }
#endif

  /* ===================================
   *  Search for the optimal grid point
   * =================================== */

  asiAlgo_IneqSystem::t_ncoord<double> x0(N, 0), xsol;
  asiAlgo_IneqSystem::t_ncoord<int>    r(M, 1);
  bool isOk = true;

  for ( int nu = 1; nu <= M; ++nu )
  {
    r.V[nu-1] = 0;
    bool isSolved = m_params->GetSystem(r)->Solve(x0, xsol, m_progress, m_plotter);
    //
    if ( !isSolved )
    {
      r.V[nu-1] = 1;
      isSolved = m_params->GetSystem(r)->Solve(x0, xsol, m_progress, m_plotter);
    }

    if ( !isSolved )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "Cannot solve.");
      isOk = false;
      break; // If cannot solve for outmost polytope, there is no sense to precise.
    }
  }

  //// Check best possible system.
  //asiAlgo_IneqSystem::t_ncoord<int> r(M, 0);
  //bool isOk = m_params->GetSystem(r)->Solve(x0, xsol, m_progress, m_plotter);

  // Set result.
  m_R = r;
  m_X = xsol;

  return isOk;
}

//-----------------------------------------------------------------------------

void asiAlgo_IneqOpt::computePermutations(const int                                         dim,
                                          const int                                         numOnes,
                                          std::vector< asiAlgo_IneqSystem::t_ncoord<int> >& gridPts) const
{
  std::string bitmask(numOnes, 1);
  bitmask.resize(dim, 0);

  do
  {
    asiAlgo_IneqSystem::t_ncoord<int> nextGridPt(dim);

#if defined COUT_DEBUG
    std::cout << " \t";
#endif

    for ( int i = 0; i < dim; ++i )
    {
      if ( bitmask[i] )
        nextGridPt.V[i] = 1;
      else
        nextGridPt.V[i] = 0;

#if defined COUT_DEBUG
      std::cout << " " << nextGridPt.V[i];
#endif
    }

#if defined COUT_DEBUG
    std::cout << std::endl;
#endif

    // Add to the result.
    gridPts.push_back(nextGridPt);
  }
  while ( std::prev_permutation( bitmask.begin(), bitmask.end() ) );
}
