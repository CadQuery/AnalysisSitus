//-----------------------------------------------------------------------------
// Created on: 18 April 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017-present, Sergey Slyadnev
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

//! RNG (Random number generator) based on code presented by Ian C. Bullard (more
//! accurate than standard C rand() and at least 10 times faster).
class asiAlgo_BullardRNG
{
public:

  asiAlgo_BullardRNG(const unsigned seed = 1)
  {
    this->Initialize(seed);
  }

  void Initialize(const unsigned seed)
  {
    m_iHi = seed;
    m_iLo = seed ^ 0x49616E42;
  }

  unsigned RandInt()
  {
    static const int shift = sizeof(int) / 2;
    m_iHi = (m_iHi >> shift) + (m_iHi << shift);
    m_iHi += m_iLo;
    m_iLo += m_iHi;
    return m_iHi;
  }

  double RandDouble()
  {
    return RandInt() / (double) (0xFFFFFFFF);
  }

protected:

  unsigned m_iLo, m_iHi;

};
