//-----------------------------------------------------------------------------
// Created on: 25 September 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef analysis_situs_h
#define analysis_situs_h

#define ASitus_NotUsed(x)

#ifdef ASitus_EXPORTS
  #define ASitus_EXPORT __declspec(dllexport)
#else
  #define ASitus_EXPORT __declspec(dllimport)
#endif

//-----------------------------------------------------------------------------
// Current version of Analysis Situs software
//-----------------------------------------------------------------------------

#define ASITUS_VERSION_MAJOR 0 // Major: incremented by major releases
#define ASITUS_VERSION_MINOR 0 // Minor: incremented by minor releases
#define ASITUS_VERSION_PATCH 3 // Patch: incremented with bug-fix releases

//-----------------------------------------------------------------------------

// Cumulative version: 8 bits for each version number
#define ASITUS_VERSION_HEX \
( (ASITUS_VERSION_MAJOR << 16) | (ASITUS_VERSION_MINOR << 8) | ASITUS_VERSION_PATCH )

#define ASITUS_VERSION_STRING "0.0.3"
#define ASITUS_APP_NAME "Analysis Situs"

//-----------------------------------------------------------------------------

// Active Data (API) includes
#include <ActAPI_IAlgorithm.h>

// VTK includes
#include <vtkAutoInit.h>

#endif
