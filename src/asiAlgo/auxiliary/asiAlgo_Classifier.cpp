//-----------------------------------------------------------------------------
// Created on: 21 April 2016
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
#include <asiAlgo_Classifier.h>

//! Generates a random ray from the given point using the passed random
//! number generator (RNG). It is important to pass the latter generator as
//! normally in "panic" classification methods we are going to select
//! different rays on each iteration (thus, we have to reuse the random seed).
//!
//! \param pos [in]     position in the model space to cast a ray from.
//! \param rng [in/out] random number generator to choose direction.
//! \return generated random ray.
gp_Ax1 asiAlgo_Classifier::CastRay(const gp_Pnt&          pos,
                                   math_BullardGenerator& rng)
{
  // Generate random angles in range [0, 2PI]
  const double rnd_phi_rad = 0.0 + 2*M_PI*rng.NextReal();
  const double rnd_psi_rad = 0.0 + 2*M_PI*rng.NextReal();

  // Switch to Cartesian coordinates for the random ray
  gp_Dir rnd_dir( cos(rnd_psi_rad)*cos(rnd_phi_rad),
                  cos(rnd_psi_rad)*sin(rnd_phi_rad),
                  sin(rnd_psi_rad) );

  return gp_Ax1(pos, rnd_dir);
}
