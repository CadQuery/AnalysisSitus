//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
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
