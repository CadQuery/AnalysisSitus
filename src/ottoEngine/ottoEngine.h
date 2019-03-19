//-----------------------------------------------------------------------------
// Created on: 16 February 2019
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef ottoEngine_h
#define ottoEngine_h

#define ottoEngine_NotUsed(x) x

#ifdef ottoEngine_EXPORTS
  #define ottoEngine_EXPORT __declspec(dllexport)
#else
  #define ottoEngine_EXPORT __declspec(dllimport)
#endif

#endif
