//-----------------------------------------------------------------------------
// Created on: 18 April 2017
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

#ifndef asiAlgo_PSO_h
#define asiAlgo_PSO_h

// asiAlgo includes
#include <asiAlgo_Function.h>

// STD includes
#include <vector>

//-----------------------------------------------------------------------------
// Random number generator
//-----------------------------------------------------------------------------

//! RNG based on code presented by Ian C. Bullard (more
//! accurate than standard C rand() and at least 10 times
//! faster).
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

//-----------------------------------------------------------------------------
// Optimization tool
//-----------------------------------------------------------------------------

//! Particle Swarm Optimizer.
template <typename T_DIMENSION>
class asiAlgo_PSO
{
public:

  typedef T_DIMENSION t_argument;

public:

  //! Search area as a simple hyperbox.
  struct t_search_hyperbox
  {
    t_argument corner_min; //!< Lower corner.
    t_argument corner_max; //!< Upper corner.
  };

  //! Measuring consisting in function evaluation at the given point.
  struct t_measuring
  {
    t_argument p; //!< Coordinates of measuring.
    double     Q; //!< Fitness (target function value).

    t_measuring() : p( t_argument() ), Q(0.0) {}
  };

  //! Single particle (also called "agent" in PSO community).
  struct t_particle
  {
    t_measuring current; //!< Current measuring.
    t_measuring best;    //!< Best previous measuring.
    t_argument  v;       //!< Current velocity.
  };

  //! Search parameters.
  struct t_search_params
  {
    int                           num_particles;  //!< Number of particles in a swarm.
    int                           num_iterations; //!< Max number of iterations.
    double                        precision;      //!< Precision stopping criterion.
    t_search_hyperbox             area;           //!< Search area.
    double                        m;              //!< Inertia property.
    double                        mu_cognition;   //!< Cognitive coefficient.
    double                        nu_social;      //!< Social coefficient.
    asiAlgo_Function<t_argument>* pFunc;          //!< Target function.

    t_search_params() : num_particles  (0),
                        num_iterations (0),
                        precision      (0.0),
                        m              (0.0),
                        mu_cognition   (0.0),
                        nu_social      (0.0),
                        pFunc          (NULL)
    {}
  };

public:

  //! Creates optimization tool initializing it with the given search
  //! parameters.
  //! \param params [in] PSO parameters.
  asiAlgo_PSO(const t_search_params& params) : m_params(params)
  {}

  //! Destructor.
  ~asiAlgo_PSO() {}

public:

  //! Runs optimization.
  //! \param num_iters [out] consumed number of iterations.
  //! \return true in case of success, false -- otherwise.
  bool Perform(int& num_iters)
  {
    /* ======================
     *  Initialization stage
     * ====================== */

    this->populate();

    // Initialize best fitness using the initial distribution of particles
    m_best.Q = DBL_MAX;
    for ( size_t i = 0; i < m_particles.size(); ++i )
    {
      const t_argument& x = m_particles[i].current.p;

      // Evaluate distance
      const double d = m_params.pFunc->Value(x);
      if ( d < m_best.Q )
      {
        m_best.Q = d;
        m_best.p = x;
      }
    }

    std::cout << "Best initial fitness: " << m_best.Q << std::endl;

    /* ================
     *  Flocking stage
     * ================ */

    bool doStop;
    num_iters = 0;
    do
    {
      ++num_iters;
      if ( num_iters < m_params.num_iterations )
      {
        this->fit_and_update_best();
        this->update_positions_and_velocities();

        // Check stop criteria
        doStop = true;
        for ( size_t i = 0; i < m_particles.size(); ++i )
        {
          if ( m_particles[i].v.Modulus() > m_params.precision )
          {
            doStop = false;
            break;
          }
        }
      }
      else
        doStop = true;
    }
    while ( !doStop );

    return true; // Success
  }

public:


  //! \return individual particles.
  const std::vector<t_particle>& GetParticles() const
  {
    return m_particles;
  }

  //! \return best global fitness.
  const t_measuring& GetBestGlobal() const
  {
    return m_best;
  }

protected:

  //! Populates set of agents.
  void populate()
  {
    // Add particles to corners of the search area where extrema is likely to be
    t_particle corner_agent[2];
    corner_agent[0].current.p = m_params.area.corner_min;
    corner_agent[1].current.p = m_params.area.corner_max;
    m_particles.push_back(corner_agent[0]);
    m_particles.push_back(corner_agent[1]);

    // TODO: add other corners of the hypercube as particles

    // Populate the initial set of particles
    for ( int i = 0; i < m_params.num_particles - 2; ++i )
    {
      t_particle new_particle;
      for ( int k = 0; k < t_argument::num_coordinates(); ++k )
      {
        const double t = this->rng(); // (0, 1)

        const double coord_min  = m_params.area.corner_min.Coord(k);
        const double coord_max  = m_params.area.corner_max.Coord(k);
        const double coord_rand = coord_min + t*(coord_max - coord_min);

        // Set k-th coordinate of the particle's initial position
        new_particle.current.p.SetCoord(k, coord_rand);
        new_particle.current.Q = DBL_MAX;
        new_particle.best = new_particle.current;
      }

      // Add particle to the population
      m_particles.push_back(new_particle);
    }
  }

  void fit_and_update_best()
  {
    // Loop over particles
    for ( size_t i = 0; i < m_particles.size(); ++i )
    {
      t_particle& agent = m_particles[i];

      // Fit particle position into the constrained area
      agent.current.p = this->proj_D(agent.current.p, m_params.area);

      // Evaluate distance
      const double d_new       = m_params.pFunc->Value(agent.current.p);
      const double d_self_best = agent.best.Q;
      if ( d_new < d_self_best )
      {
        // Update particle's best position and result
        agent.best.p = agent.current.p;
        agent.best.Q = d_self_best;

        if ( agent.best.Q < m_best.Q )
        {
          // Update global best position and result
          m_best.p = agent.best.p;
          m_best.Q = agent.best.Q;
        }
      }
    }
  }

  void update_positions_and_velocities()
  {
    // Loop over particles
    for ( size_t i = 0; i < m_particles.size(); ++i )
    {
      t_particle& agent = m_particles[i];

      // Evaluate random coefficients
      const double mu_random = this->rng();
      const double nu_random = this->rng();

      // Evaluate velocity and new position
      agent.v = this->v_update(m_params.m, agent.current.p, agent.v,
                               agent.best.p, m_best.p,
                               m_params.mu_cognition, m_params.nu_social,
                               mu_random, nu_random);
      agent.current.p = this->p_update(agent.current.p, agent.v);
    }
  }

  t_argument proj_D(const t_argument&        p,
                    const t_search_hyperbox& D)
  {
    t_argument res = p;
    for ( int k = 0; k < t_argument::num_coordinates(); ++k )
    {
      if ( res.Coord(k) < D.corner_min.Coord(k) )
        res.SetCoord( k, D.corner_min.Coord(k) );

      if ( res.Coord(k) > D.corner_max.Coord(k) )
        res.SetCoord( k, D.corner_max.Coord(k) );
    }

    return res;
  }

  t_argument p_update(const t_argument& p,
                      const t_argument& v)
  {
    return p + v;
  }

  t_argument v_update(const double      m,
                      const t_argument& p,
                      const t_argument& v,
                      const t_argument& p_best_individual,
                      const t_argument& p_best_social,
                      const double      mu_congition,
                      const double      nu_social,
                      const double      mu_random,
                      const double      nu_random)
  {
    return v*m
         + (p_best_individual - p)*mu_congition*mu_random
         + (p_best_social - p)*nu_social*nu_random;
  }

  //! Random number generator in range [0, 1]. Based on Ian C. Bullard's
  //! approach for fast and high quality (at least better than rand()) PRNG.
  //! \return random number.
  double rng()
  {
    return m_RNG.RandDouble();
  }

protected:

  asiAlgo_BullardRNG      m_RNG;       //!< RNG.
  t_search_params         m_params;    //!< Search parameters.
  t_measuring             m_best;      //!< Global best measuring.
  std::vector<t_particle> m_particles; //!< Individual particles.

};

#endif
