//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_pick_shell_h
#define geom_pick_shell_h

// Analysis Situs includes
#include <analysis_situs.h>

// Geometry includes
#include <geom_edge_edge_dist.h>

// OCCT includes
#include <Bnd_Box.hxx>
#include <NCollection_DataMap.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//! Shell picking tool.
class geom_pick_shell
{
public:

  ASitus_EXPORT static gp_Pnt
    MidPoint(const TopoDS_Shape& Contour,
             const double        prec);

public:

  ASitus_EXPORT
    geom_pick_shell(const double overlapping_prec,
                 const double    inaccuracy);

public:

  ASitus_EXPORT void
    operator()(const TopoDS_Shape&  Body,
               const TopoDS_Shape&  Contour,
               TopoDS_Shell&        Result,
               ActAPI_ProgressEntry Journal);

  ASitus_EXPORT void
    operator()(const TopoDS_Shape&  Body,
               const TopoDS_Shape&  Contour,
               const gp_Pnt&        MidPt,
               TopoDS_Shell&        Result,
               ActAPI_ProgressEntry Journal);

protected:

  ASitus_EXPORT void
    traverse(const TopoDS_Face&                                     SeedFace,
             const TopTools_IndexedDataMapOfShapeListOfShape&       M,
             int&                                                   faceIdx,
             TopTools_IndexedMapOfShape&                            BlockedEdges,
             TopTools_IndexedMapOfShape&                            BlockedFaces,
             TopTools_IndexedMapOfShape&                            Result,
             NCollection_DataMap<TopoDS_Edge, geom_edge_edge_dist>& ExtremaTools);

  ASitus_EXPORT double
    maxTol(const TopoDS_Shape& theShape) const;

  ASitus_EXPORT TopoDS_Face
    chooseByBoundingMiddle(const NCollection_Sequence<TopoDS_Face>& Faces,
                           const NCollection_Sequence<Bnd_Box>&     FaceBoxes,
                           const gp_Pnt&                            MidPt) const;

protected:

  double m_fInaccuracy; //!< Inaccuracy of the input geometry.

  struct
  {
    double prec;
    double laid_max;
    double height_max;
  } m_overlapping;

};

#endif
