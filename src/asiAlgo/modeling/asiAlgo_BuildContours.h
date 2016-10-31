//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiAlgo_BuildContours_h
#define asiAlgo_BuildContours_h

// A-Situs includes
#include <asiAlgo.h>

// Geometry includes
#include <asiAlgo_IntersectionPointCC.h>

// OCCT includes
#include <TopTools_IndexedMapOfShape.hxx>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

//-----------------------------------------------------------------------------

//! Utility to build contours from surface / surface intersection results.
class asiAlgo_BuildContours : public ActAPI_IAlgorithm
{
public:

  asiAlgo_EXPORT
    asiAlgo_BuildContours(ActAPI_ProgressEntry progress,
                          ActAPI_PlotterEntry  plotter);

public:

  asiAlgo_EXPORT bool
    operator()(const asiAlgo_IntersectionPointsCC& points,
               TopTools_IndexedMapOfShape&         resultWires,
               TopTools_IndexedMapOfShape&         resultFaces);

};

#endif
