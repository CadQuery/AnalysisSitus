//-----------------------------------------------------------------------------
// Created on: 21 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef engine_part_h
#define engine_part_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (geometry) includes
#include <geom_part_node.h>

// OCCT includes
#include <TopTools_IndexedMapOfShape.hxx>

//! Data Model API for geometric part.
namespace engine_part
{
  Handle(geom_part_node)
    Create_Part();

  void
    Clean_Part();

  void
    GetSubShapeIndices(const TopTools_IndexedMapOfShape& subShapes,
                       TColStd_PackedMapOfInteger&       indices);

  void
    HighlightSubShapes(const TopTools_IndexedMapOfShape& subShapes);

  void
    HighlightSubShapes(const TopTools_IndexedMapOfShape& subShapes,
                       const int                         color);

  void
    GetHighlightedSubShapes(TopTools_IndexedMapOfShape& subShapes);

};

#endif
