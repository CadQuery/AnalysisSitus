//-----------------------------------------------------------------------------
// Created on: 25 September 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiData_h
#define asiData_h

#define asiData_NotUsed(x)

#ifdef asiData_EXPORTS
  #define asiData_EXPORT __declspec(dllexport)
#else
  #define asiData_EXPORT __declspec(dllimport)
#endif

// Active Data includes
#include <ActAPI_IParameter.h>

//-----------------------------------------------------------------------------
// Custom Active Data Parameters
//-----------------------------------------------------------------------------

#define Parameter_AAG Parameter_LASTFREE

#endif
