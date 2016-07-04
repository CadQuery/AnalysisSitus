//-----------------------------------------------------------------------------
// Created on: 30 May 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_build_obb.h>

// Mesh includes
#include <tess_conglomerate.h>

// Geometry includes
#include <geom_utils.h>

// Mesh (Active Data) includes
#include <Mesh_ElementsIterator.h>
#include <Mesh_Node.h>

// Eigen includes
#include <Eigen/Dense>

// STL includes
#include <vector>

// OCCT includes
#include <BRepPrimAPI_MakeBox.hxx>
#include <gp_Ax1.hxx>

//-----------------------------------------------------------------------------

bool compare(const std::pair<double, int>& p1, const std::pair<double, int>& p2)
{
  return p1.first > p2.first;
}

//-----------------------------------------------------------------------------

//! Constructor.
//! \param progress [in] Progress Entry.
//! \param plotter  [in] Imperative Plotter.
geom_build_obb::geom_build_obb(ActAPI_ProgressEntry progress,
                               ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter)
{}

//! Builds OBB volume from the given part.
//! \param part [in] workpiece to build OBB for.
//! \return true in case of success, false -- otherwise.
bool geom_build_obb::operator()(const TopoDS_Shape& part)
{
  // Build tessellation conglomerate
  tess_conglomerate conglomerate(part, tess_conglomerate::Mode_Mesh);
  //
  Handle(Poly_CoherentTriangulation) tess = conglomerate.GetResultPoly();
  Handle(Mesh)                 mesh = conglomerate.GetResultMesh();

  // Get number of nodes
  const int nNodes = mesh->NbNodes();

  /* ================================
   *  STAGE 1: calculate mean vertex
   * ================================ */

  gp_XYZ mu;
  for ( Mesh_ElementsIterator nit(mesh, Mesh_ET_Node); nit.More(); nit.Next() )
  {
    Handle(Mesh_Node) node = Handle(Mesh_Node)::DownCast( nit.GetValue() );
    const double node_X = node->X();
    const double node_Y = node->Y();
    const double node_Z = node->Z();

    mu += gp_XYZ(node_X, node_Y, node_Z);
  }
  mu /= nNodes;

  this->Plotter().CLEAN();
  this->Plotter().DRAW_TRIANGULATION(tess->GetTriangulation(), Color_Magenta, 0.5);
  this->Plotter().DRAW_POINT(mu, Color_Red);

  /* ==============================
   *  STAGE 2: calculate main axes
   * ============================== */

  Eigen::Matrix3d C;
  for ( int j = 1; j <= 3; ++j )
  {
    for ( int k = 1; k <= 3; ++k )
    {
      C(j-1, k-1) = 0.0; // TODO: is that necessary?
    }
  }

  for ( Mesh_ElementsIterator nit(mesh, Mesh_ET_Node); nit.More(); nit.Next() )
  {
    Handle(Mesh_Node) node = Handle(Mesh_Node)::DownCast( nit.GetValue() );
    //
    const double node_X = node->X();
    const double node_Y = node->Y();
    const double node_Z = node->Z();
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
  std::cout << "\tThe eigenvalues of A are:" << std::endl << EigenSolver.eigenvalues() << std::endl;
  std::cout << "\tThe matrix of eigenvectors, V, is:" << std::endl << EigenSolver.eigenvectors() << std::endl << std::endl;

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

  // Check if the system is right-handed
  const double ang = ax_X.Direction().AngleWithRef( ax_Y.Direction(), ax_Z.Direction() );
  if ( ang < 0 )
  {
    gp_Ax1 tmp = ax_X;
    ax_X = ax_Y;
    ax_Y = tmp;
  }

  {
    // NOTE: that looks a bit funny, but in order to visualize the just calculated
    //       principal axes of OBB we use AABB as it gives us hint for sizing

    double xMin, yMin, zMin, xMax, yMax, zMax;
    geom_utils::Bounds(part, xMin, yMin, zMin, xMax, yMax, zMax);
    //
    gp_Pnt bndMin(xMin, yMin, zMin), bndMax(xMax, yMax, zMax);
    const double magnitude = bndMax.Distance(bndMin)*0.1;

    this->Plotter().DRAW_VECTOR_AT(mu, ax_X.Direction().XYZ()*magnitude, Color_Red);
    this->Plotter().DRAW_VECTOR_AT(mu, ax_Y.Direction().XYZ()*magnitude, Color_Green);
    this->Plotter().DRAW_VECTOR_AT(mu, ax_Z.Direction().XYZ()*magnitude, Color_Blue);
  }

  /* ======================================================
   *  STAGE 3: calculate extremities on the principal axes
   * ====================================================== */

  double ax_X_param_max = -RealLast(),
         ax_Y_param_max = -RealLast(),
         ax_Z_param_max = -RealLast();
  double ax_X_param_min = RealLast(),
         ax_Y_param_min = RealLast(),
         ax_Z_param_min = RealLast();
  //
  for ( Mesh_ElementsIterator nit(mesh, Mesh_ET_Node); nit.More(); nit.Next() )
  {
    Handle(Mesh_Node) node = Handle(Mesh_Node)::DownCast( nit.GetValue() );
    //
    const double node_X = node->X();
    const double node_Y = node->Y();
    const double node_Z = node->Z();
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

  /* =======================================================================
   *  STAGE 4: Find transformation from global axes to the actual placement
   * ======================================================================= */

  // Find transformation
  gp_Ax3 ax3_placement( mu, ax_Z.Direction(), ax_X.Direction() );
  gp_Trsf T;
  T.SetTransformation(ax3_placement);
  T.Invert();

  // Build OBB
  BRepPrimAPI_MakeBox mkOBB( gp_Pnt(ax_X_param_min, ax_Y_param_min, ax_Z_param_min),
                             gp_Pnt(ax_X_param_max, ax_Y_param_max, ax_Z_param_max) );
  m_result = mkOBB.Solid().Moved(T);

  // Draw
  this->Plotter().DRAW_SHAPE(m_result, Color_White, 0.3, true);
  return true;
}
