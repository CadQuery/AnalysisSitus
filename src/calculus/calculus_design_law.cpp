//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Geom includes
#include <calculus_design_law.h>

// OCCT includes
#include <BSplCLib.hxx>

//! Constructor accepting unidimensional poles of the law to be given in
//! Bernstein / B-spline basis.
//! \param poles [in] poles of the law.
//! \param last  [in] last parameter value (the first one is always 0).
calculus_design_law::calculus_design_law(const Handle(TColStd_HArray1OfReal)& poles,
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
double calculus_design_law::operator()(const double x)
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
double calculus_design_law::Min_X() const
{
  if ( m_knots.IsNull() )
    return 0.0;

  return m_knots->Value( m_knots->First() );
}

//! \return max argument.
double calculus_design_law::Max_X() const
{
  if ( m_knots.IsNull() )
    return 1.0;

  return m_knots->Value( m_knots->Last() );
}
