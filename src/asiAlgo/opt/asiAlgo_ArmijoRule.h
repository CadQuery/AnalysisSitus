//-----------------------------------------------------------------------------
// Created on: 24 April 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_ArmijoRule_h
#define asiAlgo_ArmijoRule_h

// asiAlgo includes
#include <asiAlgo_FunctionWithGradient.h>

#define COUT_DEBUG
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
    const double  min_alpha = 1.0e-8;
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
