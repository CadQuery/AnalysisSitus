//-----------------------------------------------------------------------------
// Created on: 24 April 2017
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

#ifndef asiAlgo_ArmijoRule_h
#define asiAlgo_ArmijoRule_h

// asiAlgo includes
#include <asiAlgo_FunctionWithGradient.h>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//! Armijo rule for adaptive selection of step length in inexact line search.
template <typename T_DIMENSION>
class asiAlgo_ArmijoRule
{
public:

  typedef T_DIMENSION t_coord;

public:

  //! Search parameters.
  struct t_search_params
  {
    double                                 max_alpha;      //!< Right bound.
    int                                    num_iterations; //!< Max number of iterations.
    t_coord                                x_k;            //!< Position in the search space.
    t_coord                                d_k;            //!< Direction of line search.
    asiAlgo_FunctionWithGradient<t_coord>* pFunc;          //!< Target function.

    t_search_params() : max_alpha      (0.0),
                        num_iterations (0),
                        pFunc          (NULL)
    {}
  };

public:

  asiAlgo_ArmijoRule() {}

public:

  bool Perform(const t_search_params& params,
               int&                   num_iters,
               double&                alpha)
  {
    num_iters = 0;
    alpha     = params.max_alpha;

    const double  beta      = 0.2; // Interval reduction coefficient
    const double  mu        = 1.0e-2;
    const double  min_alpha = 1.0e-14;
    const t_coord gradient  = params.pFunc->Gradient(params.x_k);
    const double  f         = params.pFunc->Value(params.x_k);
    const double  phi_deriv = gradient.Dot(params.d_k);

    // Initial alpha
    bool isSolved = false;
    bool doStop   = false;

    // Main iterations
    do
    {
      ++num_iters;
      if ( num_iters < params.num_iterations )
      {
        const double barrier = alpha*mu*phi_deriv;
        const double phi     = params.pFunc->Value(params.x_k + params.d_k*alpha) - f;

        if ( phi < barrier )
        {
          doStop   = true;
          isSolved = true;
        }
        else
        {
          alpha *= beta;

          if ( alpha < min_alpha )
          {
            doStop   = true;
            isSolved = false;
          }
        }
      }
      else
      {
        doStop   = true;
        isSolved = false;
      }
    }
    while ( !doStop );

#if defined COUT_DEBUG
    if ( !isSolved )
      std::cout << "WARNING: Armijo rule cannot choose a descent step!" << std::endl;
#endif

    return isSolved;
  }

};

#endif
