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
#include <asiAlgo_FairingAijFunc.h>

// OCCT includes
#include <BSplCLib.hxx>
#include <math_Matrix.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiAlgo_FairingAijFunc::asiAlgo_FairingAijFunc(const TColStd_Array1OfReal& U,
                                               const int                   p,
                                               const int                   i,
                                               const int                   j,
                                               const double                lambda)
: asiAlgo_FairingCoeffFunc(lambda), m_U(U)
{
  m_iDegree = p;
  m_iIndex1 = i;
  m_iIndex2 = j;
}

//-----------------------------------------------------------------------------

bool asiAlgo_FairingAijFunc::Value(const double u, double& f)
{
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

  const int oneBasedIndex1 = m_iIndex1 + 1;
  const int oneBasedIndex2 = m_iIndex2 + 1;

  double Ni = 0.0, Nj = 0.0, d2Ni = 0.0, d2Nj = 0.0;

  // For indices in a band of width (p + 1), we can query what
  // OpenCascade returns. Otherwise, the derivative vanishes.
  if ( (oneBasedIndex1 >= firstNonZeroIdx) && (oneBasedIndex1 < firstNonZeroIdx + m_iDegree + 1) )
    Ni = N_mx(1, oneBasedIndex1 - firstNonZeroIdx + 1);
  //
  if ( (oneBasedIndex2 >= firstNonZeroIdx) && (oneBasedIndex2 < firstNonZeroIdx + m_iDegree + 1) )
    Nj = N_mx(1, oneBasedIndex2 - firstNonZeroIdx + 1);

  // Derivatives.
  if ( (oneBasedIndex1 >= firstNonZeroIdx) && (oneBasedIndex1 < firstNonZeroIdx + m_iDegree + 1) )
    d2Ni = N_mx(3, oneBasedIndex1 - firstNonZeroIdx + 1);
  //
  if ( (oneBasedIndex2 >= firstNonZeroIdx) && (oneBasedIndex2 < firstNonZeroIdx + m_iDegree + 1) )
    d2Nj = N_mx(3, oneBasedIndex2 - firstNonZeroIdx + 1);

  f = Ni*Nj + this->GetLambda()*d2Ni*d2Nj;

  return true;
}
