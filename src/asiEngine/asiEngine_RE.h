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
#include <asiData_RENode.h>

// OCCT includes
#include <Geom_Surface.hxx>
#include <TopoDS_Wire.hxx>

//! Data Model API for reverse engineering.
namespace engine_re
{
  Handle(asiData_RENode)
    Create_RE();

//---------------------------------------------------------------------------//

  Handle(asiData_RESurfaceNode)
    Create_Surface(const Handle(Geom_Surface)& surface,
                   const double                uLimit,
                   const double                vLimit);

  void
    Clean_Surfaces();

//---------------------------------------------------------------------------//

  Handle(asiData_REContourNode)
    Create_Contour(const TopoDS_Wire& contour);

  void
    Clean_Contours();

};

#endif
