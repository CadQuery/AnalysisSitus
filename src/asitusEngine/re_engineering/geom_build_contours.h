//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_build_contours_h
#define geom_build_contours_h

// A-Situs includes
#include <asitusGeom.h>

// Geometry includes
#include <geom_intersection_point_cc.h>

// OCCT includes
#include <TopTools_IndexedMapOfShape.hxx>

//-----------------------------------------------------------------------------

//! Utility to build contours from surface / surface intersection results.
class geom_build_contours : public ActAPI_IAlgorithm
{
public:

  ASitus_EXPORT
    geom_build_contours(ActAPI_ProgressEntry progress,
                        ActAPI_PlotterEntry  plotter);

public:

  ASitus_EXPORT bool
    operator()(const geom_intersection_points_cc& points,
               TopTools_IndexedMapOfShape&        resultWires,
               TopTools_IndexedMapOfShape&        resultFaces);

};

#endif
