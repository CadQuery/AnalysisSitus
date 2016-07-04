//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef engine_re_h
#define engine_re_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (geometry) includes
#include <geom_re_node.h>

// OCCT includes
#include <Geom_Surface.hxx>
#include <TopoDS_Wire.hxx>

//! Data Model API for reverse engineering.
namespace engine_re
{
  Handle(geom_re_node)
    Create_RE();

//---------------------------------------------------------------------------//

  Handle(geom_re_surface_node)
    Create_Surface(const Handle(Geom_Surface)& surface,
                   const double                uLimit,
                   const double                vLimit);

  void
    Clean_Surfaces();

//---------------------------------------------------------------------------//

  Handle(geom_re_contour_node)
    Create_Contour(const TopoDS_Wire& contour);

  void
    Clean_Contours();

};

#endif
