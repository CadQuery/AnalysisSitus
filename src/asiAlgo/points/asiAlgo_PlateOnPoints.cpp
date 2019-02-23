//-----------------------------------------------------------------------------
// Created on: 03 March 2017
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
bool asiAlgo_PlateOnPoints::Perform(const std::vector<gp_XYZ>&   points,
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
