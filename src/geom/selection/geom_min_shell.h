//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_min_shell_h
#define geom_min_shell_h

// Analysis Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//! Extracts min shell.
class geom_min_shell
{
public:

  ASitus_EXPORT
    geom_min_shell(const double inaccuracy);

public:

  ASitus_EXPORT bool
    operator()(const TopoDS_Shape&  Body,
               const TopoDS_Shape&  Contour,
               TopoDS_Shell&        Result,
               ActAPI_ProgressEntry Journal);

protected:

  ASitus_EXPORT void
    traverse(const TopoDS_Face&                               SeedFace,
             const TopTools_IndexedDataMapOfShapeListOfShape& M,
             int&                                             faceIdx,
             TopTools_IndexedMapOfShape&                      BlockedEdges,
             TopTools_IndexedMapOfShape&                      BlockedFaces,
             TopTools_IndexedMapOfShape&                      Result);

protected:

  double m_fInaccuracy; //!< Inaccuracy of the input geometry.

};

#endif
