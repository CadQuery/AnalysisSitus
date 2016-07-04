//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_project_edges_faces_h
#define geom_project_edges_faces_h

// Geometry includes
#include <geom_utils.h>

// OCCT includes
#include <NCollection_DataMap.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_HSequenceOfShape.hxx>

//! Edges-to-faces projection tool.
class geom_project_edges_faces
{
public:

  ASitus_EXPORT
    geom_project_edges_faces();

public:

  ASitus_EXPORT bool
    operator()(const TSlicerMap&    Slicers,
               const double         inaccuracy,
               TopoDS_Shape&        ResultContour,
               TopoDS_Shape&        ResultSlicers,
               ActAPI_ProgressEntry Journal);

};

#endif
