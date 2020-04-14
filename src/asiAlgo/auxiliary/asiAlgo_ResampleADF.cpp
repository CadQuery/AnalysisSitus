//-----------------------------------------------------------------------------
// Created on: 11 April 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Sergey Slyadnev
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
#include <asiAlgo_ResampleADF.h>

// asiAlgo includes
#include <asiAlgo_Timer.h>

// Intel TBB includes
#include <blocked_range3d.h>
#include <parallel_for.h>

#ifdef USE_MOBIUS

//! Intel TBB functor for parallel evaluation of distance field hosted in
//! adaptive octree (ADF).
template <typename T>
class ParallelEvalFunctor
{
public:

  //! Ctor initializing the functor.
  ParallelEvalFunctor(poly_SVO*                             pSVO,
                      const T                               xStep,
                      const T                               yStep,
                      const T                               zStep,
                      const Handle(asiAlgo_UniformGrid<T>)& grid)
  {
    m_pSVO = pSVO;

    // Steps.
    m_fXStep = xStep;
    m_fYStep = yStep;
    m_fZStep = zStep;

    // Domain.
    m_fXMin = (T) pSVO->GetP0().X();
    m_fYMin = (T) pSVO->GetP0().Y();
    m_fZMin = (T) pSVO->GetP0().Z();

    // Array of scalars.
    m_grid = grid;
  }

  //! Body of parallel evaluation.
  //! \param[in] range three-dimensional range for task stealing.
  void operator()(const tbb::blocked_range3d<int>& range) const
  {
    const int begPag = range.pages().begin();
    const int endPag = range.pages().end();
    const int begRow = range.rows().begin();
    const int endRow = range.rows().end();
    const int begCol = range.cols().begin();
    const int endCol = range.cols().end();

    T x = 0., y = 0., z = 0.;

    // Process the block of data.
    for ( int i = begPag; i != endPag; ++i )
    {
      x = m_fXMin + m_fXStep*i;
      for ( int j = begRow; j != endRow; ++j )
      {
        y = m_fYMin + m_fYStep*j;
        for ( int k = begCol; k != endCol; ++k )
        {
          z = m_fZMin + m_fZStep*k;

          // Evaluate scalar.
          const T s = (T) m_pSVO->Eval( t_xyz(x, y, z) );

          // Put in the grid.
          m_grid->pArray[i][j][k] = s;
        }
      }
    }
  }

private:

  //! Calculates unique ID for the three-dimensional data grain by the
  //! given row & column indices. These indices normally represent
  //! the starting indices of the block.
  //! \param[in] i index along OX axis.
  //! \param[in] j index along OY axis.
  //! \param[in] k index along OZ axis.
  //! \return unique identifier of the data grain.
  inline int rangeId(const int i,
                     const int j,
                     const int k) const
  {
    return m_grid->Nx*m_grid->Ny*k + m_grid->Nx*j + i;
  }

private:

  /* Octree. */
  poly_SVO* m_pSVO;

  /* Steps. */
  T m_fXStep, m_fYStep, m_fZStep;

  /* Domain. */
  T m_fXMin, m_fYMin, m_fZMin;

  /* The three-dimensional array of scalars to populate. */
  Handle(asiAlgo_UniformGrid<T>) m_grid;

};

#endif // USE_MOBIUS

//-----------------------------------------------------------------------------

asiAlgo_ResampleADF::asiAlgo_ResampleADF(void*                pSVO,
                                         ActAPI_ProgressEntry progress,
                                         ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm (progress, plotter),
  m_pSVO            (pSVO),
  m_bIsParallel     (true)
{}

//-----------------------------------------------------------------------------

bool asiAlgo_ResampleADF::Perform(const float step)
{
#ifdef USE_MOBIUS
  if ( m_pSVO == nullptr )
    return false;

  poly_SVO* pSVO = static_cast<poly_SVO*>(m_pSVO);

  // Domain.
  const double xMin = pSVO->GetP0().X();
  const double yMin = pSVO->GetP0().Y();
  const double zMin = pSVO->GetP0().Z();
  const double xMax = pSVO->GetP7().X();
  const double yMax = pSVO->GetP7().Y();
  const double zMax = pSVO->GetP7().Z();

  m_plotter.REDRAW_POINT("P0", gp_Pnt(xMin, yMin, zMin), Color_Red);
  m_plotter.REDRAW_POINT("P7", gp_Pnt(xMax, yMax, zMax), Color_Blue);

  // Steps along axes.
  const float xStep = step;
  const float yStep = step;
  const float zStep = step;

  // Number of cells in each dimension.
  const int nx = int( (xMax - xMin) / xStep ) + 1;
  const int ny = int( (yMax - yMin) / yStep ) + 1;
  const int nz = int( (zMax - zMin) / zStep ) + 1;

  // Prepare the output grid.
  m_grid = new asiAlgo_UniformGrid<float>( (float) xMin,
                                           (float) yMin,
                                           (float) zMin,
                                            nx, ny, nz,
                                           (float) step );

  // Compute scalars.
  if ( m_bIsParallel )
  {
    tbb::parallel_for(
      tbb::blocked_range3d<int>(0, nx + 1, 0, ny + 1, 0, nz + 1),
      ParallelEvalFunctor<float>(pSVO, xStep, yStep, zStep, m_grid)
    );
  }
  else
  {
    ParallelEvalFunctor<float> Eval(pSVO, xStep, yStep, zStep, m_grid);
    Eval( tbb::blocked_range3d<int>(0, nx + 1, 0, ny + 1, 0, nz + 1) );
  }

  return true;
#else
  m_progress.SendLogMessage(LogErr(Normal) << "Mobius is not available.");
  return false;
#endif
}

//-----------------------------------------------------------------------------

const Handle(asiAlgo_UniformGrid<float>)& asiAlgo_ResampleADF::GetResult() const
{
  return m_grid;
}

//-----------------------------------------------------------------------------

void asiAlgo_ResampleADF::SetParallelMode(const bool on)
{
  m_bIsParallel = on;
}
