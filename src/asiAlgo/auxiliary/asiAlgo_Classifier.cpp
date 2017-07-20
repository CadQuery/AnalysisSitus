//-----------------------------------------------------------------------------
// Created on: 21 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
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
