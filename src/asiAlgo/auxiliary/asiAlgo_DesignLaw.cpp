//-----------------------------------------------------------------------------
// Created on: 09 February 2016
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

// Geom includes
#include <asiAlgo_DesignLaw.h>

// OCCT includes
#include <BSplCLib.hxx>

//! Constructor accepting one-dimensional poles of the law to be given in
//! Bernstein / B-spline basis.
//! \param poles [in] poles of the law.
//! \param last  [in] last parameter value (the first one is always 0).
asiAlgo_DesignLaw::asiAlgo_DesignLaw(const Handle(TColStd_HArray1OfReal)& poles,
                                     const double                         last)
{
  m_poles = poles;

  if ( !m_poles.IsNull() )
  {
    // Derive degree
    const int degree = m_poles->Length() - 1;

    // Prepare knots
    m_knots = new TColStd_HArray1OfReal(0, 1);
    m_knots->ChangeValue(0) = 0.0;
    m_knots->ChangeValue(1) = last;

    // Prepare multiplicities
    m_mults = new TColStd_HArray1OfInteger(0, 1);
    m_mults->ChangeValue(0) = degree + 1;
    m_mults->ChangeValue(1) = degree + 1;
  }
}

//! Evaluates explicit law function f(x) for the given value x.
//! \param x [in] argument value.
//! \return function value.
double asiAlgo_DesignLaw::operator()(const double x)
{
  if ( m_poles.IsNull() )
    return 0.0;

  // Derive degree
  const int degree = m_poles->Length() - 1;

  // Find span
  int span_idx;
  double new_x;
  BSplCLib::LocateParameter(degree, m_knots->Array1(), &m_mults->Array1(), x, 0, span_idx, new_x);
  if ( new_x < m_knots->Value(span_idx) )
    span_idx--;

  // Evaluate
  double P;
  BSplCLib::D0(new_x, span_idx, degree, 0, m_poles->Array1(),
               BSplCLib::NoWeights(),
               m_knots->Array1(), &m_mults->Array1(), P);

  return P;
}

//! \return min argument.
double asiAlgo_DesignLaw::Min_X() const
{
  if ( m_knots.IsNull() )
    return 0.0;

  return m_knots->Value( m_knots->Lower() );
}

//! \return max argument.
double asiAlgo_DesignLaw::Max_X() const
{
  if ( m_knots.IsNull() )
    return 1.0;

  return m_knots->Value( m_knots->Upper() );
}
