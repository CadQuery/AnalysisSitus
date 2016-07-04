//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_project_edge_face_h
#define geom_project_edge_face_h

// Analysis Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <gp_Lin.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

//! Edge-to-Face projection tool.
class geom_project_edge_face
{
public:

  ASitus_EXPORT
    geom_project_edge_face(const TopoDS_Face& Face);

public:

  ASitus_EXPORT bool
    operator()(const TopoDS_Edge&   Edge,
               const bool           doNormalProj,
               TopoDS_Shape&        Result,
               gp_Lin&              ProjRay,
               ActAPI_ProgressEntry Journal);

private:

  TopoDS_Face m_face; //!< Working face.

};

#endif
