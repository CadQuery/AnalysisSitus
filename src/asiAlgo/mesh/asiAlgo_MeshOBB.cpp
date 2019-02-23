//-----------------------------------------------------------------------------
// Created on: 16 February 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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
#include <asiAlgo_MeshOBB.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// OCCT includes
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepPrimAPI_MakeBox.hxx>

// Eigen includes
#pragma warning(push, 0)
#include <Eigen/Dense>
#pragma warning(pop)

// STL includes
#include <vector>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #pragma message("===== warning: DRAW_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

static bool compare(const std::pair<double, int>& p1,
                    const std::pair<double, int>& p2)
{
  return p1.first > p2.first;
}

//-----------------------------------------------------------------------------

asiAlgo_MeshOBB::asiAlgo_MeshOBB(const Handle(Poly_Triangulation)& mesh,
                                 ActAPI_ProgressEntry              progress,
                                 ActAPI_PlotterEntry               plotter)
//
: ActAPI_IAlgorithm (progress, plotter),
  m_input           (mesh)
{}

//-----------------------------------------------------------------------------

bool asiAlgo_MeshOBB::Perform()
{
  // Check if triangulation exists.
  if ( m_input.IsNull() || !m_input->NbNodes() )
  {
    this->GetProgress().SendLogMessage( LogErr(Normal) << "Cannot build OBB for empty triangulation." );
    return false;
  }

  // Calculate principal axes.
  gp_Ax1 ax_X, ax_Y, ax_Z;
  gp_XYZ mu;
  //
  this->calculateByCovariance(ax_X, ax_Y, ax_Z, mu);

  { // Imperative drawing
    // NOTE: that looks a bit funny, but in order to visualize the just calculated
    //       principal axes of OBB we use AABB as it gives us hint for sizing.

    double xMin, yMin, zMin, xMax, yMax, zMax;
    asiAlgo_Utils::Bounds(m_input, xMin, yMin, zMin, xMax, yMax, zMax);
    //
    gp_Pnt bndMin(xMin, yMin, zMin), bndMax(xMax, yMax, zMax);
    const double magnitude = bndMax.Distance(bndMin)*0.1;

    this->GetPlotter().DRAW_VECTOR_AT(mu, ax_X.Direction().XYZ()*magnitude, Color_Red);
    this->GetPlotter().DRAW_VECTOR_AT(mu, ax_Y.Direction().XYZ()*magnitude, Color_Green);
    this->GetPlotter().DRAW_VECTOR_AT(mu, ax_Z.Direction().XYZ()*magnitude, Color_Blue);
  }

  /* =============================================
   *  Calculate extremities on the principal axes
   * ============================================= */

  double ax_X_param_max = -RealLast(),
         ax_Y_param_max = -RealLast(),
         ax_Z_param_max = -RealLast();
  double ax_X_param_min =  RealLast(),
         ax_Y_param_min =  RealLast(),
         ax_Z_param_min =  RealLast();
  //
  const TColgp_Array1OfPnt& nodes = m_input->Nodes();
  //
  for ( int i = nodes.Lower(); i <= nodes.Upper(); ++i )
  {
    const gp_Pnt& P = nodes(i);
    //
    const double node_X = P.X();
    const double node_Y = P.Y();
    const double node_Z = P.Z();
    //
    gp_XYZ p(node_X, node_Y, node_Z);
    gp_Vec p_local = p - mu;

    const double ax_X_param = p_local.Dot( ax_X.Direction() );
    const double ax_Y_param = p_local.Dot( ax_Y.Direction() );
    const double ax_Z_param = p_local.Dot( ax_Z.Direction() );

    if ( ax_X_param > ax_X_param_max )
      ax_X_param_max = ax_X_param;
    //
    if ( ax_Y_param > ax_Y_param_max )
      ax_Y_param_max = ax_Y_param;
    //
    if ( ax_Z_param > ax_Z_param_max )
      ax_Z_param_max = ax_Z_param;
    //
    if ( ax_X_param < ax_X_param_min )
      ax_X_param_min = ax_X_param;
    //
    if ( ax_Y_param < ax_Y_param_min )
      ax_Y_param_min = ax_Y_param;
    //
    if ( ax_Z_param < ax_Z_param_min )
      ax_Z_param_min = ax_Z_param;
  }

#if defined DRAW_DEBUG
  { // Imperative drawing.
    const gp_XYZ ax_X_max = mu + ax_X.Direction().XYZ()*ax_X_param_max;
    const gp_XYZ ax_Y_max = mu + ax_Y.Direction().XYZ()*ax_Y_param_max;
    const gp_XYZ ax_Z_max = mu + ax_Z.Direction().XYZ()*ax_Z_param_max;
    //
    const gp_XYZ ax_X_min = mu + ax_X.Direction().XYZ()*ax_X_param_min;
    const gp_XYZ ax_Y_min = mu + ax_Y.Direction().XYZ()*ax_Y_param_min;
    const gp_XYZ ax_Z_min = mu + ax_Z.Direction().XYZ()*ax_Z_param_min;

    this->Plotter().DRAW_POINT(ax_X_max, Color_Red);
    this->Plotter().DRAW_POINT(ax_Y_max, Color_Green);
    this->Plotter().DRAW_POINT(ax_Z_max, Color_Blue);
    //
    this->Plotter().DRAW_POINT(ax_X_min, Color_Magenta);
    this->Plotter().DRAW_POINT(ax_Y_min, Color_Yellow);
    this->Plotter().DRAW_POINT(ax_Z_min, Color_Violet);
  }
#endif

  /* =====================
   *  STAGE 4: Set result
   * ===================== */

  // Set placement and corner positions to the result.
  gp_Ax3 ax3_placement( mu, ax_Z.Direction(), ax_X.Direction() );
  gp_Pnt corner_min(ax_X_param_min, ax_Y_param_min, ax_Z_param_min);
  gp_Pnt corner_max(ax_X_param_max, ax_Y_param_max, ax_Z_param_max);
  //
  m_placement      = ax3_placement;
  m_localCornerMin = corner_min;
  m_localCornerMax = corner_max;

  return true;
}

//-----------------------------------------------------------------------------

gp_Trsf asiAlgo_MeshOBB::GetResultTrsf() const
{
  gp_Trsf T;
  T.SetTransformation(m_placement);
  T.Invert();
  return T;
}

//-----------------------------------------------------------------------------

TopoDS_Solid asiAlgo_MeshOBB::GetResultBox() const
{
  gp_Trsf T = this->GetResultTrsf();

  // Build a properly located box solid representing OBB
  TopoDS_Shape solid;
  try
  {
    BRepPrimAPI_MakeBox mkOBB(m_localCornerMin, m_localCornerMax);
    solid = BRepBuilderAPI_Transform(mkOBB.Solid(), T, true);
  }
  catch ( ... ) {}

  return TopoDS::Solid(solid);
}

//-----------------------------------------------------------------------------

void asiAlgo_MeshOBB::calculateByCovariance(gp_Ax1& xAxis,
                                            gp_Ax1& yAxis,
                                            gp_Ax1& zAxis,
                                            gp_XYZ& meanVertex) const
{
  // Get number of nodes.
  const int nNodes = m_input->NbNodes();

  /* =======================
   *  Calculate mean vertex
   * ======================= */

  gp_XYZ mu;
  const TColgp_Array1OfPnt& nodes = m_input->Nodes();
  //
  for ( int i = nodes.Lower(); i <= nodes.Upper(); ++i )
  {
    const gp_Pnt& P = nodes(i);

    const double node_X = P.X();
    const double node_Y = P.Y();
    const double node_Z = P.Z();

    mu += gp_XYZ(node_X, node_Y, node_Z);
  }
  mu /= nNodes;

  this->GetPlotter().ERASE_ALL();
  this->GetPlotter().DRAW_TRIANGULATION(m_input, Color_Magenta, 0.5);
  this->GetPlotter().DRAW_POINT(mu, Color_Red);

  /* =====================
   *  Calculate main axes
   * ===================== */

  Eigen::Matrix3d C;
  for ( int j = 1; j <= 3; ++j )
  {
    for ( int k = 1; k <= 3; ++k )
    {
      C(j-1, k-1) = 0.0; // TODO: is that necessary?
    }
  }

  for ( int i = nodes.Lower(); i <= nodes.Upper(); ++i )
  {
    const gp_Pnt& P = nodes(i);
    //
    const double node_X = P.X();
    const double node_Y = P.Y();
    const double node_Z = P.Z();
    //
    gp_XYZ p(node_X, node_Y, node_Z);
    gp_XYZ p_dash = p - mu;

    for ( int j = 1; j <= 3; ++j )
    {
      for ( int k = 1; k <= 3; ++k )
      {
        C(j-1, k-1) += ( p_dash.Coord(j)*p_dash.Coord(k) );
      }
    }
  }

  for ( int j = 1; j <= 3; ++j )
  {
    for ( int k = 1; k <= 3; ++k )
    {
      C(j-1, k-1) /= (nNodes);
    }
  }

  Eigen::EigenSolver<Eigen::Matrix3d> EigenSolver(C);

#if defined COUT_DEBUG
  std::cout << "\tThe eigen values of A are:" << std::endl << EigenSolver.eigenvalues() << std::endl;
  std::cout << "\tThe matrix of eigenvectors, V, is:" << std::endl << EigenSolver.eigenvectors() << std::endl << std::endl;
#endif

  Eigen::Vector3cd v1 = EigenSolver.eigenvectors().col(0);
  Eigen::Vector3cd v2 = EigenSolver.eigenvectors().col(1);
  Eigen::Vector3cd v3 = EigenSolver.eigenvectors().col(2);

  gp_Vec V[3] = { gp_Vec( v1.x().real(), v1.y().real(), v1.z().real() ),
                  gp_Vec( v2.x().real(), v2.y().real(), v2.z().real() ),
                  gp_Vec( v3.x().real(), v3.y().real(), v3.z().real() ) };
  //
  std::vector< std::pair<double, int> >
    lambda { std::pair<double, int>( EigenSolver.eigenvalues()(0).real(), 0 ),
             std::pair<double, int>( EigenSolver.eigenvalues()(1).real(), 1 ),
             std::pair<double, int>( EigenSolver.eigenvalues()(2).real(), 2 ) };
  //
  std::sort(lambda.begin(), lambda.end(), compare);
  //
  gp_Ax1 ax_X(mu, V[lambda[0].second]);
  gp_Ax1 ax_Y(mu, V[lambda[1].second]);
  gp_Ax1 ax_Z(mu, V[lambda[2].second]);

  // Check if the system is right-handed.
  const double ang = ax_X.Direction().AngleWithRef( ax_Y.Direction(), ax_Z.Direction() );
  if ( ang < 0 )
  {
    gp_Ax1 tmp = ax_X;
    ax_X = ax_Y;
    ax_Y = tmp;
  }

  // Store results.
  meanVertex = mu;
  xAxis      = ax_X;
  yAxis      = ax_Y;
  zAxis      = ax_Z;
}
