//-----------------------------------------------------------------------------
// Created on: 21 March 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_ShapeCellType_h
#define asiVisu_ShapeCellType_h

// asiVisu includes
#include <asiVisu.h>

//! Possible cell types
enum asiVisu_ShapePrimitive
{
  ShapePrimitive_Undefined       = 0, //!< Undefined cell type.
  ShapePrimitive_FreeVertex      = 1, //!< Cell type for free vertex.
  ShapePrimitive_SharedVertex    = 2, //!< Cell type for shared vertex.
  ShapePrimitive_DanglingEdge    = 3, //!< Cell type for dangling edge.
  ShapePrimitive_FreeEdge        = 4, //!< Cell type for free edge.
  ShapePrimitive_ManifoldEdge    = 5, //!< Cell type for manifold edge.
  ShapePrimitive_NonManifoldEdge = 6, //!< Cell type for non-manifold edge.
  ShapePrimitive_Facet           = 7  //!< Cell type for facet.
};

#endif
