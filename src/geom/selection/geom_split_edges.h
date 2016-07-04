//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_split_edges_h
#define geom_split_edges_h

// Analysis Situs includes
#include <analysis_situs.h>

// Geometry includes
#include <geom_intersection_point_edges.h>
#include <geom_split_shape.h>

// OCCT includes
#include <NCollection_Sequence.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopTools_ShapeMapHasher.hxx>

//! This tool splits edges by the passed vertices.
class geom_split_edges : public geom_split_shape
{
public:

  ASitus_EXPORT
    geom_split_edges(const double inaccuracy);

public:

  ASitus_EXPORT void
    operator()(const geom_intersection_points_edges& InterPts,
               const TSlicerMap&                     SlicerMap,
               const TopoDS_Shape&                   OriginalBody,
               TopoDS_Shape&                         ResultBody,
               TopoDS_Compound&                      Result,
               TSlicerMap&                           ResultSlicerMap,
               ActAPI_ProgressEntry                  Journal);

};

#endif
