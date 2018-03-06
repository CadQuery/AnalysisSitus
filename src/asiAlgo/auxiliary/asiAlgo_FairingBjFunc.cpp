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
#include <asiAlgo_FairingBjFunc.h>

// OCCT includes
#include <BSplCLib.hxx>
#include <math_Matrix.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiAlgo_FairingBjFunc::asiAlgo_FairingBjFunc(const Handle(Geom_BSplineCurve)& curve,
                                             const int                        coord,
                                             const TColStd_Array1OfReal&      U,
                                             const int                        p,
                                             const int                        i,
                                             const double                     lambda)
: asiAlgo_FairingCoeffFunc(lambda), m_U(U)
{
  m_curve   = curve;
  m_iCoord  = coord;
  m_iDegree = p;
  m_iIndex  = i;
}

//-----------------------------------------------------------------------------

bool asiAlgo_FairingBjFunc::Value(const double u, double& f)
{
  /* ==================================================================
   *  Calculate 2-nd derivative of basis spline at the given parameter
   * ================================================================== */

  const int order = m_iDegree + 1;

  int firstNonZeroIdx;
  math_Matrix N_mx(1, 3, 1, order);
  BSplCLib::EvalBsplineBasis(2, order, m_U, u, firstNonZeroIdx, N_mx);

#if defined COUT_DEBUG
  // Dump
  std::cout << "\tFirst Non-Zero Index for " << u << " is " << firstNonZeroIdx << std::endl;
  for ( int kk = 1; kk <= p + 1; ++kk )
  {
    std::cout << "\t" << N_mx(3, kk);
  }
  std::cout << std::endl;
#endif

  double d2N = 0.0;
  const int oneBasedIndex = m_iIndex + 1;

  // For indices in a band of width (p + 1), we can query what
  // OpenCascade returns. Otherwise, the derivative vanishes.
  if ( (oneBasedIndex >= firstNonZeroIdx) && (oneBasedIndex < firstNonZeroIdx + m_iDegree + 1) )
    d2N = N_mx(3, oneBasedIndex - firstNonZeroIdx + 1);

  /* ====================================================
   *  Calculate 2-nd derivative of the curve in question
   * ==================================================== */

  gp_Pnt P;
  gp_Vec d1C, d2C;
  m_curve->D2(u, P, d1C, d2C);

  // Get projection of the second derivative.
  const double d2C_proj = d2C.Coord(m_iCoord + 1); // As the passed coord is 0-based,
                                                   // while OCCT operates with 1-based indices.

  // Calculate the result.
  f = this->GetLambda()*d2N*d2C_proj;

  return true;
}
