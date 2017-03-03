//-----------------------------------------------------------------------------
// Created on: 03 March 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_PlateOnPoints_h
#define asiAlgo_PlateOnPoints_h

// A-Situs includes
#include <asiAlgo_BaseCloud.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <Geom_BSplineSurface.hxx>

//-----------------------------------------------------------------------------

//! Utility to build a plate surface on the given point set.
class asiAlgo_PlateOnPoints : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_PlateOnPoints, ActAPI_IAlgorithm)

public:

  asiAlgo_EXPORT
    asiAlgo_PlateOnPoints(ActAPI_ProgressEntry progress,
                          ActAPI_PlotterEntry  plotter);

public:

  asiAlgo_EXPORT bool
    Build(const std::vector<gp_Pnt>&   points,
          Handle(Geom_BSplineSurface)& result);

};

#endif
