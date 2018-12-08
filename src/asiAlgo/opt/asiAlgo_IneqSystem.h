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

#ifndef asiAlgo_IneqSystem_h
#define asiAlgo_IneqSystem_h

// asiAlgo includes
#include <asiAlgo.h>

// Active Data includes
#include <ActAPI_IPlotter.h>
#include <ActAPI_IProgressNotifier.h>

//-----------------------------------------------------------------------------

//! Range of a two-sided inequality.
struct t_ineqRange
{
  std::pair<double, double> values;

  t_ineqRange() {}

  t_ineqRange(const double left, const double right)
  {
    values.first  = left;
    values.second = right;
  }
};

//-----------------------------------------------------------------------------

//! N-dimensional coordinates (vector or point).
template <typename T>
struct t_ineqNCoord
{
  std::vector<T> V;   //!< Coordinates.
  int            Dim; //!< Dimension.

  t_ineqNCoord() : Dim(0) {} //!< Default ctor.

  //! Ctor to create n-dimensional origin.
  //! \param[in] dim dimension.
  //! \param[in] val value to set for all <dim> coordinates.
  t_ineqNCoord(const int dim, const T val = 0) : Dim(dim)
  {
    for ( size_t j = 0; j < Dim; ++j )
      V.push_back(val);
  }

  //! Ctor with initialization of coordinates.
  //! \param[in] coords coordinates to set.
  t_ineqNCoord(const std::vector<T>& coords) : V(coords), Dim( int(coords.size()) ) {}

  //! Initialize all coordinates with <val>.
  void Init(const T val)
  {
    for ( size_t j = 0; j < Dim; ++j )
      V[j] = val;
  }

  //! Calculates modulus of this n-dimensional tuple.
  //! \return modulus.
  double Modulus() const
  {
    double modulus2 = 0;
    //
    for ( size_t j = 0; j < Dim; ++j )
      modulus2 += V[j]*V[j];
    //
    return Sqrt(modulus2);
  }

  //! Computes dot product between this tuple and the passed tuple.
  //! \param[in] coords coordinates of the operand tuple.
  //! \return dot product.
  double Dot(const t_ineqNCoord<T>& coords) const
  {
    double dot = 0.0;
    //
    for ( size_t j = 0; j < Dim; ++j )
      dot += V[j]*coords.V[j];

    return dot;
  }

  void operator*=(const T val)
  {
    for ( size_t j = 0; j < Dim; ++j )
      V[j] *= val;
  }

  //! Adds the passed value to each coordinate of the tuple.
  t_ineqNCoord<T> operator+(const T val) const
  {
    t_ineqNCoord<T> res = *this;
    //
    for ( size_t j = 0; j < Dim; ++j )
      res.V[j] += val;

    return res;
  }

  t_ineqNCoord<T> operator+(const t_ineqNCoord<T>& other) const
  {
    t_ineqNCoord<T> res = *this;
    //
    for ( size_t j = 0; j < Dim; ++j )
      res.V[j] += other.V[j];

    return res;
  }

  t_ineqNCoord<T> operator-(const t_ineqNCoord<T>& other) const
  {
    t_ineqNCoord<T> res = *this;
    //
    for ( size_t j = 0; j < Dim; ++j )
      res.V[j] -= other.V[j];

    return res;
  }

  t_ineqNCoord<T> operator*(const T val) const
  {
    t_ineqNCoord<T> res = *this;
    //
    for ( size_t j = 0; j < Dim; ++j )
      res.V[j] *= val;

    return res;
  }

  //! Subtracts the passed value from each coordinate of the tuple.
  t_ineqNCoord<T> operator-(const T val) const
  {
    t_ineqNCoord<T> res = *this;
    //
    for ( size_t j = 0; j < Dim; ++j )
      res.V[j] -= val;

    return res;
  }
};

//-----------------------------------------------------------------------------

//! System of inequalities.
class asiAlgo_IneqSystem : public Standard_Transient
{
public:

  // OCCT RTTI.
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_IneqSystem, Standard_Transient)

public:

  //! Default ctor.
  asiAlgo_EXPORT
    asiAlgo_IneqSystem();

  //! Ctor accepting complete definition of the system.
  //! \param[in] n      number of variables.
  //! \param[in] m      number of inequalities.
  //! \param[in] ANu    <m> ranges for all inequalities.
  //! \param[in] coeffs <m> vectors of coefficients for all inequalities.
  asiAlgo_EXPORT
    asiAlgo_IneqSystem(const int                                n,
                       const int                                m,
                       const std::vector<t_ineqRange>&          ANu,
                       const std::vector<t_ineqNCoord<double>>& coeffs);

public:

  asiAlgo_EXPORT bool
    Solve(const t_ineqNCoord<double>& x0,
          t_ineqNCoord<double>&       sol,
          ActAPI_ProgressEntry        progress,
          ActAPI_PlotterEntry         plotter) const;

  asiAlgo_EXPORT bool
    IsConsistent(const t_ineqNCoord<double>& x,
                 int&                        nuViolated) const;

public:

  asiAlgo_EXPORT void
    Dump(ActAPI_PlotterEntry            plotter,
         const ActAPI_Color&            color,
         const TCollection_AsciiString& name,
         const bool                     isWireframe);

public:

  //! \return number of variables.
  asiAlgo_EXPORT int
    GetN() const;

  //! Sets the number of variables.
  //! \param[in] n value to set.
  asiAlgo_EXPORT void
    SetN(const int n);

  //! \return number of inequalities.
  asiAlgo_EXPORT int
    GetM() const;

  //! Sets the number of inequalities.
  //! \param[in] m value to set.
  asiAlgo_EXPORT void
    SetM(const int m);

  //! \return bounds of inequalities.
  asiAlgo_EXPORT const std::vector<t_ineqRange>&
    GetANu() const;

  //! Sets bounds of inequalities.
  //! \param[in] ANu bounds to set.
  asiAlgo_EXPORT void
    SetANu(const std::vector<t_ineqRange>& ANu);

  //! \return coefficients of inequalities.
  asiAlgo_EXPORT const std::vector<t_ineqNCoord<double>>&
    GetCoeffs() const;

  //! \brief Sets the coefficients of inequalities.
  //! \param[in] coeffs coefficients to set.
  asiAlgo_EXPORT void
    SetCoeffs(const std::vector<t_ineqNCoord<double>>& coeffs);

  //! \brief Returns the global min bound for a particular two-sided
  //!        inequality.
  //! \param[in] nu 1-based index of the inequality in question.
  //! \return min bound for inequality number <nu>.
  asiAlgo_EXPORT double
    GetAMinus(const int nu) const;

  //! \brief Sets the global min bound for a particular two-sided
  //!        inequality.
  //! \param[in] nu 1-based index of the inequality in question.
  //! \param[in] a  value to set.
  asiAlgo_EXPORT void
    SetAMinus(const int nu, const double a);

  //! \brief Returns the global max bound for a particular two-sided
  //!        inequality.
  //! \param[in] nu 1-based index of the inequality in question.
  //! \return max bound for inequality number <nu>.
  asiAlgo_EXPORT double
    GetAPlus(const int nu) const;

  //! \brief Sets the global max bound for a particular two-sided
  //!        inequality.
  //! \param[in] nu 1-based index of the inequality in question.
  //! \param[in] a  value to set.
  asiAlgo_EXPORT void
    SetAPlus(const int nu, const double a);

  //! \brief Returns the coefficients of the inequality number <nu>.
  //! \param[in]  nu     1-based index of the inequality in question.
  //! \param[out] coeffs coefficients for the inequality number <nu>.
  asiAlgo_EXPORT void
    GetCoeffs(const int             nu,
              t_ineqNCoord<double>& coeffs) const;

  //! \brief Sets the coefficients of the inequality number <nu>.
  //! \param[in] nu     1-based index of the inequality in question.
  //! \param[in] coeffs coefficients to set.
  asiAlgo_EXPORT void
    SetCoeffs(const int               nu,
              const t_ineqNCoord<double>& coeffs);

public:

  //! \return series of points illustrating how the algorithm converged.
  const std::vector< t_ineqNCoord<double> >& GetTraceOfConvergence() const
  {
    return m_trace;
  }

protected:

  asiAlgo_EXPORT void
    nextX(const t_ineqNCoord<double>& x_prev,
          const int                   nuViolated,
          t_ineqNCoord<double>&       x_next) const;

protected:

  //! Number of variables.
  int m_iN;

  //! Number of inequalities.
  int m_iM;

  //! Inequality bounds.
  std::vector<t_ineqRange> m_ANu;

  //! Coefficients for the variables.
  std::vector< t_ineqNCoord<double> > m_coeffs;

  //! Trace of convergence.
  mutable std::vector< t_ineqNCoord<double> > m_trace;

};

#endif
