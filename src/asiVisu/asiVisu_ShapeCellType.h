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
enum asiVisu_ShapeCellType
{
  ShapeCellType_Undefined     = -1, //!< Undefined cell type.
  ShapeCellType_IsoLine       =  0, //!< Cell type for isoline.
  ShapeCellType_FreeVertex    =  1, //!< Cell type for free vertex.
  ShapeCellType_SharedVertex  =  2, //!< Cell type for shared vertex.
  ShapeCellType_FreeEdge      =  3, //!< Cell type for free edge.
  ShapeCellType_BorderEdge    =  4, //!< Cell type for single-face (border) edge.
  ShapeCellType_SharedEdge    =  5, //!< Cell type for shared edge.
  ShapeCellType_WireframeFace =  6, //!< Cell type for wireframe face.
  ShapeCellType_ShadedFace    =  7  //!< Cell type for shaded face.
};

#endif
