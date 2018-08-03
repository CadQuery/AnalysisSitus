//-----------------------------------------------------------------------------
// Created on: 03 March 2018
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
#include <asiAlgo_FairBCurve.h>

// asiAlgo includes
#include <asiAlgo_FairingAijFunc.h>
#include <asiAlgo_FairingBjFunc.h>
#include <asiAlgo_Utils.h>

// OCCT includes
#include <BSplCLib.hxx>

// Eigen includes
#pragma warning(disable : 4701 4702)
#include <Eigen/Dense>
#pragma warning(default : 4701 4702)

//-----------------------------------------------------------------------------

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #pragma message("===== warning: DRAW_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

#define NUM_CONSTRAINED_POLES_LEADING  1
#define NUM_CONSTRAINED_POLES_TRAILING 1
#define NUM_INTEGRATION_BINS           500

//-----------------------------------------------------------------------------

asiAlgo_FairBCurve::asiAlgo_FairBCurve(const Handle(Geom_BSplineCurve)& curve,
                                       const double                     lambda,
                                       ActAPI_ProgressEntry             progress,
                                       ActAPI_PlotterEntry              plotter)
: ActAPI_IAlgorithm(progress, plotter)
{
  m_inputCurve = curve;
  m_fLambda    = lambda;
}

//-----------------------------------------------------------------------------

bool asiAlgo_FairBCurve::Perform()
{
  // Prepare flat sequence of knots and other working vars.
  const TColStd_Array1OfReal& U   = m_inputCurve->KnotSequence();
  const int                   p   = m_inputCurve->Degree();
  const int                   m   = U.Length() - 1;
  const int                   n   = m - p - 1;
  const int                   dim = n + 1 - NUM_CONSTRAINED_POLES_LEADING - NUM_CONSTRAINED_POLES_TRAILING;

  // Initialize matrix from the passed row pointer.
  Eigen::MatrixXd eigen_A_mx(dim, dim);
  for ( int r = 0; r < dim; ++r )
  {
    for ( int c = 0; c < dim; ++c )
    {
      asiAlgo_FairingAijFunc N2(U, p, r + NUM_CONSTRAINED_POLES_LEADING, c + NUM_CONSTRAINED_POLES_LEADING, m_fLambda);

      // Compute integral.
      const double val = asiAlgo_Utils::IntegralRect(N2, U.First(), U.Last(), NUM_INTEGRATION_BINS);

      eigen_A_mx(r, c) = val;
    }
  }

#if defined COUT_DEBUG
  std::cout << "Here is the matrix A:\n" << eigen_A_mx << std::endl;
#endif

  // Initialize vector of right hand side.
  Eigen::MatrixXd eigen_B_mx(dim, 3);
  for ( int r = 0; r < dim; ++r )
  {
    asiAlgo_FairingBjFunc rhs_x(m_inputCurve, 0, U, p, r + NUM_CONSTRAINED_POLES_LEADING, m_fLambda);
    asiAlgo_FairingBjFunc rhs_y(m_inputCurve, 1, U, p, r + NUM_CONSTRAINED_POLES_LEADING, m_fLambda);
    asiAlgo_FairingBjFunc rhs_z(m_inputCurve, 2, U, p, r + NUM_CONSTRAINED_POLES_LEADING, m_fLambda);

    // Compute integrals.
    const double val_x = asiAlgo_Utils::IntegralRect(rhs_x, U.First(), U.Last(), NUM_INTEGRATION_BINS);
    const double val_y = asiAlgo_Utils::IntegralRect(rhs_y, U.First(), U.Last(), NUM_INTEGRATION_BINS);
    const double val_z = asiAlgo_Utils::IntegralRect(rhs_z, U.First(), U.Last(), NUM_INTEGRATION_BINS);

    eigen_B_mx(r, 0) = -val_x;
    eigen_B_mx(r, 1) = -val_y;
    eigen_B_mx(r, 2) = -val_z;
  }

#if defined COUT_DEBUG
  std::cout << "Here is the matrix B:\n" << eigen_B_mx << std::endl;
#endif

  // Solve.
  Eigen::ColPivHouseholderQR<Eigen::MatrixXd> QR(eigen_A_mx);
  Eigen::MatrixXd eigen_X_mx = QR.solve(eigen_B_mx);

#if defined COUT_DEBUG
  std::cout << "Here is the matrix X (solution):\n" << eigen_X_mx << std::endl;
#endif

  m_resultCurve = Handle(Geom_BSplineCurve)::DownCast( m_inputCurve->Copy() );

  // Apply perturbations to poles.
  const TColgp_Array1OfPnt& poles = m_resultCurve->Poles();
  int                       r     = 0;
  //
  for ( int p = poles.Lower() + NUM_CONSTRAINED_POLES_LEADING; p <= poles.Upper() - NUM_CONSTRAINED_POLES_TRAILING; ++p, ++r )
  {
    gp_XYZ P = poles(p).XYZ();
    gp_XYZ D = gp_XYZ( eigen_X_mx(r, 0), eigen_X_mx(r, 1), eigen_X_mx(r, 2) );
    //
    m_resultCurve->SetPole(p, P + D);
  }

#if defined COUT_DEBUG
  std::cout << "Num poles: " << m_resultCurve->Poles().Length() << std::endl;
#endif

#if defined DRAW_DEBUG
  m_plotter.REDRAW_CURVE("faired", m_resultCurve, Color_Green);
#endif

  return true;
}
