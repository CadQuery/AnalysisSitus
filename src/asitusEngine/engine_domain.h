//-----------------------------------------------------------------------------
// Created on: 30 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef engine_domain_h
#define engine_domain_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (geometry) includes
#include <geom_part_node.h>

// OCCT includes
#include <TopTools_IndexedMapOfShape.hxx>

//! Data Model API for face domain.
namespace engine_domain
{
  void
    GetHighlightedEdges(TopTools_IndexedMapOfShape& edges);

  void
    GetHighlightedEdges(TopTools_IndexedMapOfShape& edges,
                        TopoDS_Face&                face);

};

#endif
