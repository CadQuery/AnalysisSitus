//-----------------------------------------------------------------------------
// Created on: 03 March 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_PlateOnPoints.h>

// asiAlgo includes
#include <asiAlgo_PlaneOnPoints.h>
#include <asiAlgo_Timer.h>

// OCCT includes
#include <Geom_Plane.hxx>
#include <GeomPlate_BuildPlateSurface.hxx>
#include <GeomPlate_MakeApprox.hxx>
#include <GeomPlate_PlateG0Criterion.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

//! Constructs the tool.
//! \param progress [in] progress indicator.
//! \param plotter  [in] imperative plotter.
asiAlgo_PlateOnPoints::asiAlgo_PlateOnPoints(ActAPI_ProgressEntry progress,
                                             ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter)
{}

//-----------------------------------------------------------------------------

//! Constructs TPS (Thin Plate Spline) approximation for the passed points.
//! \param points [in]  point set to build the approximation surface for.
//! \param result [out] approximation surface.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_PlateOnPoints::Build(const std::vector<gp_Pnt>&   points,
                                  Handle(Geom_BSplineSurface)& result)
{
  const int    Degree      = 3;
  const int    NbPtsOnCur  = 10;
  const int    NbIter      = 3;
  const double Tol2d       = 0.00001;
  const double Tol3d       = 0.0001;
  const double TolAng      = 0.01;
  const double TolCurv     = 0.1;
  const bool   Anisotropie = false;

  /* =================================
   *  STAGE 1: build an average plane
   * ================================= */

  TIMER_NEW
  TIMER_GO

  gp_Pln plane;
  asiAlgo_PlaneOnPoints planeAlgo(m_progress, m_plotter);
  //
  if ( !planeAlgo.Build(points, plane) )
  {
#if defined COUT_DEBUG
    std::cout << "Error: cannot build average plane" << std::endl;
#endif
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot build average plane" );
    return false;
  }
  //
  Handle(Geom_Plane) planeSurf = new Geom_Plane(plane);

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Build average plate")

  /* ======================
   *  STAGE 2: build plate
   * ====================== */

  TIMER_RESET
  TIMER_GO

  GeomPlate_BuildPlateSurface plateAlgo(planeSurf,
                                        Degree,
                                        NbPtsOnCur,
                                        NbIter,
                                        Tol2d,
                                        Tol3d,
                                        TolAng,
                                        TolCurv,
                                        Anisotropie);

  // Add pinpoint constraints
  for ( size_t k = 0; k < points.size(); ++k )
  {
    const gp_Pnt& P = points[k];

    Handle(GeomPlate_PointConstraint)
      PC = new GeomPlate_PointConstraint( P, GeomAbs_C0, Precision::Confusion() );
    //
    plateAlgo.Add(PC);
  }

  // Run plate algorithm
  plateAlgo.Perform();
  //
  if ( !plateAlgo.IsDone() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Plating failed");
    return false;
  }
  //
  Handle(GeomPlate_Surface) plateSurf = plateAlgo.Surface();

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Build plate")

  /* ====================================
   *  STAGE 3: approximate plate surface
   * ==================================== */

  TIMER_RESET
  TIMER_GO

  int    maxNumOfConstraints = 1000;
  double tol                 = Precision::Confusion();
  int    nbP                 = 4;
  int    maxNumOfSegments    = maxNumOfConstraints / 10;
  //
  TColgp_SequenceOfXY  seqXY;
  TColgp_SequenceOfXYZ seqXYZ;

  plateAlgo.Disc2dContour(nbP, seqXY);
  plateAlgo.Disc3dContour(nbP, GeomAbs_C0, seqXYZ);
  //
  GeomPlate_PlateG0Criterion aPlateCriterion(seqXY, seqXYZ, tol);
  GeomPlate_MakeApprox plateApprox(plateSurf, aPlateCriterion, tol, maxNumOfSegments, Degree);
  result = plateApprox.Surface();

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Approximate plate with B-surface")

  return true;
}
