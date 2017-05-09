//-----------------------------------------------------------------------------
// Created on: 25 September 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
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

#define Parameter_AAG             Parameter_LASTFREE
#define Parameter_Function        Parameter_LASTFREE + 1
//
#define Parameter_LASTFREE_ASITUS Parameter_Function

#endif
