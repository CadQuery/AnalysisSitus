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

#ifndef asiAlgo_IneqOpt_h
#define asiAlgo_IneqOpt_h

// asiAlgo includes
#include <asiAlgo_IneqOptParams.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

//-----------------------------------------------------------------------------

//! Optimizer of homogeneous resource distribution in the presence of inequality
//! constraints. The algorithm is based on the ideas of M. Kh. Prilutskii
//! which are best described in his Russian paper
//!
//! "Multicriterial distribution of homogeneous resource in hierarchical
//!  systems" from 1996.
//!
//! The algorithm accepts a set of systems of inequalities which describe the
//! N-dimensional search area (N-dimensional polytope). The optimizer searches
//! for a point X within the polytope which corresponds to the minimal penalty.
//! Penalties are expressed by sub-domains of the N-dimensional polytope, i.e.,
//! some sub-domains are "better" than others. The algorithm attempts to
//! find a point X within the best region.
class asiAlgo_IneqOpt : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI.
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_IneqOpt, ActAPI_IAlgorithm)

public:

  //! Default ctor.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_IneqOpt(ActAPI_ProgressEntry progress = NULL,
                    ActAPI_PlotterEntry  plotter  = NULL);

  //! Ctor accepting optimization parameters.
  //! \param[in] params   parameters defining the optimization problem.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_IneqOpt(const Handle(asiAlgo_IneqOptParams)& params,
                    ActAPI_ProgressEntry                 progress = NULL,
                    ActAPI_PlotterEntry                  plotter  = NULL);

public:

  //! Performs optimization.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Perform();

public:

  //! \return system which worked.
  const t_ineqNCoord<int>& GetSolR() const
  {
    return m_R;
  }

  //! \return solution X.
  const t_ineqNCoord<double>& GetSolX() const
  {
    return m_X;
  }

private:

  //! Calculates all permutations of an M-dimensional vector representing
  //! grid point.
  void populateLexicographic(const int                         dim,
                             const int                         minVal,
                             const int                         maxVal,
                             std::vector< t_ineqNCoord<int> >& gridPts) const;

protected:

  Handle(asiAlgo_IneqOptParams) m_params; //!< Optimization problem.
  t_ineqNCoord<int>             m_R;      //!< Polytope which worked.
  t_ineqNCoord<double>          m_X;      //!< Final solution.

};

#endif
