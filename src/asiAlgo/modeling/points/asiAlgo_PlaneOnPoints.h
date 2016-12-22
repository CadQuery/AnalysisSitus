//-----------------------------------------------------------------------------
// Created on: 04 December 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_PlaneOnPoints_h
#define asiAlgo_PlaneOnPoints_h

// A-Situs includes
#include <asiAlgo_PointCloud.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <gp_Pln.hxx>

//-----------------------------------------------------------------------------

//! Utility to build a plane on the given point set.
class asiAlgo_PlaneOnPoints : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_PlaneOnPoints, ActAPI_IAlgorithm)

public:

  asiAlgo_EXPORT
    asiAlgo_PlaneOnPoints(ActAPI_ProgressEntry progress,
                          ActAPI_PlotterEntry  plotter);

public:

  asiAlgo_EXPORT bool
    Build(const std::vector<gp_Pnt>& points,
          gp_Pln&                    result);

};

#endif
