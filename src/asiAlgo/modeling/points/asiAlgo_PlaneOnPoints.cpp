//-----------------------------------------------------------------------------
// Created on: 04 December 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_PlaneOnPoints.h>

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
  // TODO: NYI
  return false;
}
