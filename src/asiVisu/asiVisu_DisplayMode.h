//-----------------------------------------------------------------------------
// Created on: 26 November 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_DisplayMode_h
#define asiVisu_DisplayMode_h

// asiVisu includes
#include <asiVisu.h>

//! Enumerates display modes.
enum asiVisu_DisplayMode
{
  DisplayMode_Undefined = 0x0000, //!< Undefined display mode.
  DisplayMode_Shaded    = 0x0001, //!< Shaded.
  DisplayMode_Wireframe = 0x0002, //!< Wireframe.
  DisplayMode_Vertices  = 0x0004  //!< Vertices
};

#endif
