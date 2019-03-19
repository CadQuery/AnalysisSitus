//-----------------------------------------------------------------------------
// Created on: 16 February 2019
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef ottoData_h
#define ottoData_h

//-----------------------------------------------------------------------------

#define ottoData_NotUsed(x) x

//-----------------------------------------------------------------------------

#ifdef ottoData_EXPORTS
  #define ottoData_EXPORT __declspec(dllexport)
#else
  #define ottoData_EXPORT __declspec(dllimport)
#endif

// asiData includes
#include <asiData.h>

//-----------------------------------------------------------------------------
// Custom Active Data Parameters
//-----------------------------------------------------------------------------

#define Parameter_PointCloud Parameter_LASTFREE_ASITUS

#endif
