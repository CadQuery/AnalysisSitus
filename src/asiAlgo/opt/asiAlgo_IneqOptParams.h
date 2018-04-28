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

#ifndef asiAlgo_IneqOptParams_h
#define asiAlgo_IneqOptParams_h

// asiAlgo includes
#include <asiAlgo_IneqSystem.h>

// Standard includes
#include <vector>

//! Parameters for inequality optimizer.
class asiAlgo_IneqOptParams : public Standard_Transient
{
public:

  // OCCT RTTI.
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_IneqOptParams, Standard_Transient)

public:

  asiAlgo_EXPORT
    asiAlgo_IneqOptParams();

public:

  //! \return number of variables in the optimization problem.
  asiAlgo_EXPORT int
    GetN() const;

  //! Sets the number of variables in the optimization problem.
  //! \param[in] n value to set.
  asiAlgo_EXPORT void
    SetN(const int n);

  //! \return number of the controlled two-sided inequality constraints.
  asiAlgo_EXPORT int
    GetM() const;

  //! Sets the number of the controlled two-sided inequality constraints.
  //! \param[in] m value to set.
  asiAlgo_EXPORT void
    SetM(const int m);

  //! \brief Returns the global min bound for a particular two-sided
  //!        inequality constraint.
  //! \param[in] nu 1-based index of the inequality constraint in question.
  //! \return min bound for constraint number <nu>.
  asiAlgo_EXPORT double
    GetAMinus(const int nu) const;

  //! \brief Sets the global min bound for a particular two-sided
  //!        inequality constraint.
  //! \param[in] nu 1-based index of the inequality constraint in question.
  //! \param[in] a  value to set.
  asiAlgo_EXPORT void
    SetAMinus(const int nu, const double a);

  //! \brief Returns the global max bound for a particular two-sided
  //!        inequality constraint.
  //! \param[in] nu 1-based index of the inequality constraint in question.
  //! \return max bound for constraint number <nu>.
  asiAlgo_EXPORT double
    GetAPlus(const int nu) const;

  //! \brief Sets the global max bound for a particular two-sided
  //!        inequality constraint.
  //! \param[in] nu 1-based index of the inequality constraint in question.
  //! \param[in] a  value to set.
  asiAlgo_EXPORT void
    SetAPlus(const int nu, const double a);

  //! \brief Returns the coefficients of the inequality constraint number <nu>.
  //! \param[in]  nu     1-based index of the inequality constraint in question.
  //! \param[out] coeffs coefficients for the constraint number <nu>.
  asiAlgo_EXPORT void
    GetCoeffs(const int                             nu,
              asiAlgo_IneqSystem::t_ncoord<double>& coeffs) const;

  //! \brief Sets the coefficients of the inequality constraint number <nu>.
  //! \param[in] nu     1-based index of the inequality constraint in question.
  //! \param[in] coeffs coefficients to set.
  asiAlgo_EXPORT void
    SetCoeffs(const int                                   nu,
              const asiAlgo_IneqSystem::t_ncoord<double>& coeffs);

  //! \brief Returns the 0-penalty interval for the constraint number <nu>.
  //! \param[in]  nu    1-based index of the inequality constraint in question.
  //! \param[out] left  left bound of the interval.
  //! \param[out] right right bound of the interval.
  asiAlgo_EXPORT void
    GetInterval0(const int nu,
                 double&   left,
                 double&   right) const;

  //! \brief Sets the 0-penalty interval for the constraint number <nu>.
  //! \param[in] nu    1-based index of the inequality constraint in question.
  //! \param[in] left  left bound of the interval.
  //! \param[in] right right bound of the interval.
  asiAlgo_EXPORT void
    SetInterval0(const int    nu,
                 const double left,
                 const double right);

  //! \brief Constructs an explicit system of inequalities for the given
  //!        value of penalty.
  //!
  //! Penalty is defined as M-dimensional point with each coordinate determining
  //! the target sub-domain of the corresponding inequality criterion.
  asiAlgo_EXPORT Handle(asiAlgo_IneqSystem)
    GetSystem(const asiAlgo_IneqSystem::t_ncoord<int>& penalties) const;

protected:

  //! Number of variables.
  int m_iN;

  //! Number of the controlled two-sided inequality constraints.
  int m_iM;

  //! Global bounds for the constraints.
  std::vector<asiAlgo_IneqSystem::t_range> m_ANu;

  //! Coefficients for the constraints.
  std::vector<asiAlgo_IneqSystem::t_ncoord<double>> m_coeffs;

  //! 0-pentalty intervals for the constraints.
  std::vector<asiAlgo_IneqSystem::t_range> m_S0Nu;

};

#endif
