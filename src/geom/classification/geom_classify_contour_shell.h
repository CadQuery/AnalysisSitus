//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_classify_contour_shell_h
#define geom_classify_contour_shell_h

// Geometry includes
#include <geom_classifier.h>
#include <geom_intersection_point_edges.h>
#include <geom_utils.h>

// OCCT includes
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_HSequenceOfShape.hxx>

//! Contour-Shell classifier.
class geom_classify_contour_shell
{
public:

  ASitus_EXPORT
    geom_classify_contour_shell(const TopoDS_Shell& S,
                                const double        inaccuracy);

public:

  ASitus_EXPORT bool
    operator()(const TopoDS_Wire&              Probe,
               TSlicerMap&                     Result,
               geom_intersection_points_edges& SlicerPts,
               ActAPI_ProgressEntry            Journal) const;

protected:

  ASitus_EXPORT TopoDS_Edge
    trimmedCopy(const TopoDS_Edge& E,
                const double       f,
                const double       l) const;

protected:

  TopoDS_Shell m_S;           //!< Working shell.
  double       m_fInaccuracy; //!< Inaccuracy of geometry.

};

#endif
