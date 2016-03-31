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
class engine_part
{
public:

  static Handle(geom_part_node)
    Create_Part();

  static void
    GetSubShapeIndices(const TopTools_IndexedMapOfShape& subShapes,
                       TColStd_PackedMapOfInteger&       indices);

  static void
    HighlightSubShapes(const TopTools_IndexedMapOfShape& subShapes);

  static void
    HighlightSubShapes(const TopTools_IndexedMapOfShape& subShapes,
                       const int                         color);

  static void
    GetHighlightedSubShapes(TopTools_IndexedMapOfShape& subShapes);

private:

  engine_part() {}
  engine_part(const engine_part&) {}
  void operator=(const engine_part&) {}

};

#endif
