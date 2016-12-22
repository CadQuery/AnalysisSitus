//-----------------------------------------------------------------------------
// Created on: 04 December 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_PlaneOnPoints.h>

// Eigen includes
#include <Eigen/Dense>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

bool compare(const std::pair<double, int>& p1, const std::pair<double, int>& p2)
{
  return p1.first > p2.first;
}

//-----------------------------------------------------------------------------

//! Constructs the tool.
//! \param progress [in] progress indicator.
//! \param plotter  [in] imperative plotter.
asiAlgo_PlaneOnPoints::asiAlgo_PlaneOnPoints(ActAPI_ProgressEntry progress,
                                             ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter)
{}

//-----------------------------------------------------------------------------

//! Constructs the average plane on the given point set.
//! \param points [in]  point set to build a fitting plane for.
//! \param result [out] result plane.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_PlaneOnPoints::Build(const std::vector<gp_Pnt>& points,
                                  gp_Pln&                    result)
{
  const int nPts = (int) points.size();

  /* ======================
   *  Calculate mean point
   * ====================== */

  gp_XYZ mu;
  for ( size_t i = 0; i < points.size(); ++i )
  {
    mu += points[i].XYZ();
  }
  mu /= nPts;

  /* =========================
   *  Build covariance matrix
   * ========================= */

  Eigen::Matrix3d C;
  for ( int j = 1; j <= 3; ++j )
  {
    for ( int k = 1; k <= 3; ++k )
    {
      C(j-1, k-1) = 0.0; // TODO: is that necessary?
    }
  }

  for ( size_t i = 0; i < points.size(); ++i )
  {
    const gp_XYZ& p      = points[i].XYZ();
    gp_XYZ        p_dash = p - mu;

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
      C(j-1, k-1) /= nPts;
    }
  }

  Eigen::EigenSolver<Eigen::Matrix3d> EigenSolver(C);

#if defined COUT_DEBUG
  std::cout << "\tThe eigen values of C are:" << std::endl << EigenSolver.eigenvalues() << std::endl;
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
  //
  gp_Vec vec_X( ax_X.Direction() );
  gp_Vec vec_Y( ax_Y.Direction() );
  gp_Vec vec_Z( ax_Z.Direction() );
  //
  if ( (vec_X ^ vec_Y).Magnitude() < gp::Resolution() ||
       (vec_X ^ vec_Z).Magnitude() < gp::Resolution() ||
       (vec_Y ^ vec_Z).Magnitude() < gp::Resolution() )
  {
    std::cout << "Warning: degenerated normal" << std::endl;
    return false; // Degenerated normal
  }

  // Check if the system is right-handed
  const double ang = ax_X.Direction().AngleWithRef( ax_Y.Direction(), ax_Z.Direction() );
  if ( ang < 0 )
  {
    gp_Ax1 tmp = ax_X;
    ax_X = ax_Y;
    ax_Y = tmp;
  }

  // Store results
  gp_Ax3 ax3( gp_Pnt(mu), ax_Z.Direction(), ax_X.Direction() );
  result.SetPosition(ax3);
  //
  return true;
}
