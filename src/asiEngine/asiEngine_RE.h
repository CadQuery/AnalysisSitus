//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiEngine_RE_h
#define asiEngine_RE_h

// A-Situs includes
#include <asiEngine.h>

// A-Situs (geometry) includes
#include <asiData_RENode.h>

// OCCT includes
#include <Geom_Surface.hxx>
#include <TopoDS_Wire.hxx>

//! Data Model API for reverse engineering.
namespace asiEngine_RE
{
  asiEngine_EXPORT Handle(asiData_RENode)
    Create_RE();

//---------------------------------------------------------------------------//

  asiEngine_EXPORT Handle(asiData_RESurfaceNode)
    Create_Surface(const Handle(Geom_Surface)& surface,
                   const double                uLimit,
                   const double                vLimit);

  asiEngine_EXPORT void
    Clean_Surfaces();

//---------------------------------------------------------------------------//

  asiEngine_EXPORT Handle(asiData_REContourNode)
    Create_Contour(const TopoDS_Wire& contour);

  asiEngine_EXPORT void
    Clean_Contours();

};

#endif
