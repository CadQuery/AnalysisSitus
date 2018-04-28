//-----------------------------------------------------------------------------
// Created on: 25 April 2018
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
#include <asiAlgo_IneqOptParams.h>

//-----------------------------------------------------------------------------

asiAlgo_IneqOptParams::asiAlgo_IneqOptParams()
: m_iN(0), m_iM(0)
{}

//-----------------------------------------------------------------------------

int asiAlgo_IneqOptParams::GetN() const
{
  return m_iN;
}

//-----------------------------------------------------------------------------

void asiAlgo_IneqOptParams::SetN(const int n)
{
  m_iN = n;
}

//-----------------------------------------------------------------------------

int asiAlgo_IneqOptParams::GetM() const
{
  return m_iM;
}

//-----------------------------------------------------------------------------

void asiAlgo_IneqOptParams::SetM(const int m)
{
  m_iM = m;
}

//-----------------------------------------------------------------------------

double asiAlgo_IneqOptParams::GetAMinus(const int nu) const
{
  return m_ANu[nu-1].values.first;
}

//-----------------------------------------------------------------------------

void asiAlgo_IneqOptParams::SetAMinus(const int nu, const double a)
{
  if ( nu > m_ANu.size() )
    m_ANu.push_back( asiAlgo_IneqSystem::t_range() );

  m_ANu[nu-1].values.first = a;
}

//-----------------------------------------------------------------------------

double asiAlgo_IneqOptParams::GetAPlus(const int nu) const
{
  return m_ANu[nu-1].values.second;
}

//-----------------------------------------------------------------------------

void asiAlgo_IneqOptParams::SetAPlus(const int nu, const double a)
{
  if ( nu > m_ANu.size() )
    m_ANu.push_back( asiAlgo_IneqSystem::t_range() );

  m_ANu[nu-1].values.second = a;
}

//-----------------------------------------------------------------------------

void asiAlgo_IneqOptParams::GetCoeffs(const int                             nu,
                                      asiAlgo_IneqSystem::t_ncoord<double>& coeffs) const
{
  coeffs = m_coeffs[nu-1];
}

//-----------------------------------------------------------------------------

void asiAlgo_IneqOptParams::SetCoeffs(const int                                   nu,
                                      const asiAlgo_IneqSystem::t_ncoord<double>& coeffs)
{
  if ( nu > m_coeffs.size() )
    m_coeffs.push_back( asiAlgo_IneqSystem::t_ncoord<double>() );

  m_coeffs[nu-1] = coeffs;
}

//-----------------------------------------------------------------------------

void asiAlgo_IneqOptParams::GetInterval0(const int nu,
                                         double&   left,
                                         double&   right) const
{
  left  = m_S0Nu[nu-1].values.first;
  right = m_S0Nu[nu-1].values.second;
}

//-----------------------------------------------------------------------------

void asiAlgo_IneqOptParams::SetInterval0(const int    nu,
                                         const double left,
                                         const double right)
{
  if ( nu > m_S0Nu.size() )
    m_S0Nu.push_back( asiAlgo_IneqSystem::t_range() );

  m_S0Nu[nu-1].values.first  = left;
  m_S0Nu[nu-1].values.second = right;
}

//-----------------------------------------------------------------------------

Handle(asiAlgo_IneqSystem)
  asiAlgo_IneqOptParams::GetSystem(const asiAlgo_IneqSystem::t_ncoord<int>& penalties) const
{
  // Get globally constrained inequalities.
  Handle(asiAlgo_IneqSystem)
    system = new asiAlgo_IneqSystem(m_iN, m_iM, m_ANu, m_coeffs);

  // Adjust boundaries according to the requested penalties.
  for ( int k = 0; k < m_iM; ++k )
  {
    if ( penalties.V[k] == 0 )
    {
      double left, right;
      this->GetInterval0(k+1, left, right);
      //
      system->SetAMinus(k+1, left);
      system->SetAPlus(k+1, right);
    }
  }

  return system;
}
