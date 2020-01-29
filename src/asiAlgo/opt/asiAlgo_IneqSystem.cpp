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

// Own include
#include <asiAlgo_IneqSystem.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// OCCT includes
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <Geom_Plane.hxx>
#include <Geom2d_Line.hxx>
#include <GeomAPI.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pln.hxx>
#include <TopTools_ListOfShape.hxx>

//-----------------------------------------------------------------------------

#define uLimit 1000.0
#define vLimit 1000.0
#define ITER_LIMIT 1500

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #pragma message("===== warning: DRAW_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiAlgo_IneqSystem::asiAlgo_IneqSystem() : Standard_Transient()
{
  m_iN = m_iM = 0;
}

//-----------------------------------------------------------------------------

asiAlgo_IneqSystem::asiAlgo_IneqSystem(const int                                n,
                                       const int                                m,
                                       const std::vector<t_ineqRange>&          ANu,
                                       const std::vector<t_ineqNCoord<double>>& coeffs)
: Standard_Transient()
{
  m_iN     = n;
  m_iM     = m;
  m_ANu    = ANu;
  m_coeffs = coeffs;
}

//-----------------------------------------------------------------------------

bool asiAlgo_IneqSystem::Solve(const t_ineqNCoord<double>& x0,
                               t_ineqNCoord<double>&       sol,
                               ActAPI_ProgressEntry        progress,
                               ActAPI_PlotterEntry         plotter) const
{
#if defined DRAW_DEBUG
  // Draw initial guess.
  if ( m_iN == 2 )
    plotter.DRAW_POINT(gp_Pnt2d(x0.V[0], x0.V[1]), Color_Yellow, "x0");
  else if ( m_iN == 3 )
    plotter.DRAW_POINT(gp_Pnt(x0.V[0], x0.V[1], x0.V[2]), Color_Yellow, "x0");
#endif

  // Trace convergence.
  m_trace.push_back(x0);

  // Check if the system of inequalities is satisfied with the initial guess.
  // If not, the 1-based index of the first violated inequality is available.
  int nuViolated = 0;
  //
  if ( this->IsConsistent(x0, nuViolated) )
  {
    sol = x0;
    return true;
  }

  int i = 1;
  t_ineqNCoord<double> x_prev = x0, x_next;
  bool isSolved = false;

  for ( i = 1; i < ITER_LIMIT; ++i )
  {
    this->nextX(x_prev, nuViolated, x_next);

#if defined DRAW_DEBUG
    if ( m_iN == 2 )
    {
      plotter.DRAW_POINT(gp_Pnt2d(x_next.V[0], x_next.V[1]), Color_Yellow, "x_next");
      plotter.DRAW_LINK(gp_Pnt2d(x_prev.V[0], x_prev.V[1]), gp_Pnt2d(x_next.V[0], x_next.V[1]), Color_Yellow, "l_next");
    }
    else if ( m_iN == 3 )
    {
      plotter.DRAW_POINT(gp_Pnt(x_next.V[0], x_next.V[1], x_next.V[2]), Color_Yellow, "x_next");
      plotter.DRAW_LINK(gp_Pnt(x_prev.V[0], x_prev.V[1], x_prev.V[2]), gp_Pnt(x_next.V[0], x_next.V[1], x_next.V[2]), Color_Yellow, "l_next");
    }
#endif

    // Trace convergence.
    m_trace.push_back(x_next);

    if ( this->IsConsistent(x_next, nuViolated) )
    {
      progress.SendLogMessage(LogInfo(Normal) << "Converged on iteration %1." << i);
      //
      sol = x_next;
      isSolved = true;
      break;
    }

#if defined COUT_DEBUG
    std::cout << "Iteration " << i << ". Nu violated: " << nuViolated << std::endl;
#endif

    x_prev = x_next;
  }

  TCollection_AsciiString NextXStr;
  //
  for ( int k = 0; k < x_next.Dim; ++k )
  {
    NextXStr += x_next.V[k];

    if ( k < x_next.Dim - 1 )
      NextXStr += " ";
  }
  //
  std::cout << "Best approached after " << i << " iterations: " << NextXStr.ToCString() << std::endl;

  return isSolved;
}

//-----------------------------------------------------------------------------

bool asiAlgo_IneqSystem::IsConsistent(const t_ineqNCoord<double>& x,
                                      int&                        nuViolated) const
{
  nuViolated = 0;

  for ( int nu = 1; nu <= m_iM; ++nu )
  {
    t_ineqNCoord<double> coeffs;
    this->GetCoeffs(nu, coeffs);

    // Compute expression.
    const double expr = x.Dot(coeffs);

    // Get limits.
    const double left  = this->GetAMinus(nu);
    const double right = this->GetAPlus(nu);

    bool isOk = false;
    if ( expr >= left && expr <= right )
      isOk = true;

    if ( !isOk )
    {
      nuViolated = nu;
      return false;
    }
  }

  return true;
}

//-----------------------------------------------------------------------------

void asiAlgo_IneqSystem::Dump(ActAPI_PlotterEntry            plotter,
                              const ActAPI_Color&            color,
                              const TCollection_AsciiString& name,
                              const bool                     isWireframe)
{
  // Just some cognitive drawings...
  TopTools_ListOfShape halfSpaces;
  //
  for ( int nu = 1; nu <= m_iM; ++nu )
  {
    // Get inequality coefficients.
    t_ineqNCoord<double> coeffs;
    this->GetCoeffs(nu, coeffs);

    // Get inequality bounds.
    const double ANuMinus = this->GetAMinus(nu);
    const double ANuPlus  = this->GetAPlus(nu);

    if ( m_iN == 2 )
    {
      // In 3D case, each inequality defines a halfspace under a line.
      gp_Lin2d HMinus (coeffs.V[0], coeffs.V[1], -ANuMinus);
      gp_Lin2d HPlus  (coeffs.V[0], coeffs.V[1], -ANuPlus);
      //
      Handle(Geom2d_Line) lin2dMinus = new Geom2d_Line(HMinus);
      Handle(Geom2d_Line) lin2dPlus  = new Geom2d_Line(HPlus);
      //
      Handle(Geom_Curve) linMinus = GeomAPI::To3d( lin2dMinus, gp_Pln( gp::XOY() ) );
      Handle(Geom_Curve) linPlus  = GeomAPI::To3d( lin2dPlus,  gp_Pln( gp::XOY() ) );
    }
    else if ( m_iN == 3 )
    {
      // In 3D case, each inequality defines a halfspace under a plane.
      gp_Pln HMinus (coeffs.V[0], coeffs.V[1], coeffs.V[2], -ANuMinus);
      gp_Pln HPlus  (coeffs.V[0], coeffs.V[1], coeffs.V[2], -ANuPlus);

      // Prepare half-spaces.
      {
        gp_Vec ref = gp_Vec(coeffs.V[0], coeffs.V[1], coeffs.V[2]) * uLimit;
        //
        TopoDS_Shape hsShape = BRepPrimAPI_MakePrism( BRepBuilderAPI_MakeFace(HMinus, -uLimit, uLimit, -vLimit, vLimit).Face(), ref);

        halfSpaces.Append(hsShape);
      }
      //
      {
        gp_Vec ref = -gp_Vec(coeffs.V[0], coeffs.V[1], coeffs.V[2]) * uLimit;
        //
        TopoDS_Shape hsShape = BRepPrimAPI_MakePrism( BRepBuilderAPI_MakeFace(HPlus, -uLimit, uLimit, -vLimit, vLimit).Face(), ref);

        halfSpaces.Append(hsShape);
      }
    }
  }
  //
  if ( m_iN == 2 || m_iN == 3 )
  {
    TopoDS_Shape polytope = asiAlgo_Utils::BooleanIntersect(halfSpaces);

    plotter.REDRAW_SHAPE(name, polytope, color, 1.0, isWireframe);
  }
}

//-----------------------------------------------------------------------------

int asiAlgo_IneqSystem::GetN() const
{
  return m_iN;
}

//-----------------------------------------------------------------------------

void asiAlgo_IneqSystem::SetN(const int n)
{
  m_iN = n;
}

//-----------------------------------------------------------------------------

int asiAlgo_IneqSystem::GetM() const
{
  return m_iM;
}

//-----------------------------------------------------------------------------

void asiAlgo_IneqSystem::SetM(const int m)
{
  m_iM = m;
}

//-----------------------------------------------------------------------------

const std::vector<t_ineqRange>& asiAlgo_IneqSystem::GetANu() const
{
  return m_ANu;
}

//-----------------------------------------------------------------------------

void asiAlgo_IneqSystem::SetANu(const std::vector<t_ineqRange>& ANu)
{
  m_ANu = ANu;
}

//-----------------------------------------------------------------------------

const std::vector< t_ineqNCoord<double> >& asiAlgo_IneqSystem::GetCoeffs() const
{
  return m_coeffs;
}

//-----------------------------------------------------------------------------

void asiAlgo_IneqSystem::SetCoeffs(const std::vector< t_ineqNCoord<double> >& coeffs)
{
  m_coeffs = coeffs;
}

//-----------------------------------------------------------------------------

double asiAlgo_IneqSystem::GetAMinus(const int nu) const
{
  return m_ANu[nu-1].values.first;
}

//-----------------------------------------------------------------------------

void asiAlgo_IneqSystem::SetAMinus(const int nu, const double a)
{
  if ( nu > int( m_ANu.size() ) )
    m_ANu.push_back( t_ineqRange() );

  m_ANu[nu-1].values.first = a;
}

//-----------------------------------------------------------------------------

double asiAlgo_IneqSystem::GetAPlus(const int nu) const
{
  return m_ANu[nu-1].values.second;
}

//-----------------------------------------------------------------------------

void asiAlgo_IneqSystem::SetAPlus(const int nu, const double a)
{
  if ( nu > int( m_ANu.size() ) )
    m_ANu.push_back( t_ineqRange() );

  m_ANu[nu-1].values.second = a;
}

//-----------------------------------------------------------------------------

void asiAlgo_IneqSystem::GetCoeffs(const int             nu,
                                   t_ineqNCoord<double>& coeffs) const
{
  coeffs = m_coeffs[nu-1];
}

//-----------------------------------------------------------------------------

void asiAlgo_IneqSystem::SetCoeffs(const int                   nu,
                                   const t_ineqNCoord<double>& coeffs)
{
  if ( nu > int( m_coeffs.size() ) )
    m_coeffs.push_back( t_ineqNCoord<double>() );

  m_coeffs[nu-1] = coeffs;
}

//-----------------------------------------------------------------------------

void asiAlgo_IneqSystem::nextX(const t_ineqNCoord<double>& x_prev,
                               const int                   nuViolated,
                               t_ineqNCoord<double>&       x_next) const
{
  x_next = x_prev;

  // Get bounds of the violated inequality.
  const double A_minus = this->GetAMinus(nuViolated);
  const double A_plus  = this->GetAPlus(nuViolated);

  t_ineqNCoord<double> violatedCoeffs;
  this->GetCoeffs(nuViolated, violatedCoeffs);

  // Working variables.
  const double         expr                  = x_prev.Dot(violatedCoeffs);
  const double         violatedCoeffsModulus = violatedCoeffs.Modulus();
  const double         distance_Hj_minus     = Abs(x_prev.Dot(violatedCoeffs) - A_minus)/violatedCoeffsModulus;
  const double         distance_Hj_plus      = Abs(x_prev.Dot(violatedCoeffs) - A_plus)/violatedCoeffsModulus;
  t_ineqNCoord<double> H_j_norm              = violatedCoeffs * (1/violatedCoeffsModulus);

  // Now check why inequality is not satisfied. There can be exactly two
  // reasons: X falls either to the left unbounded interval or to the right
  // unbounded interval.
  if ( A_minus > expr )
  {
    x_next = x_prev + H_j_norm * distance_Hj_minus * 1.5;
  }
  else if ( expr > A_plus )
  {
    x_next = x_prev - H_j_norm * distance_Hj_plus * 1.5;
  }
}
