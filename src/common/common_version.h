//-----------------------------------------------------------------------------
// Created on: 26 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef common_version_h
#define common_version_h

//-----------------------------------------------------------------------------
// A-Situs version
//-----------------------------------------------------------------------------

#define ASitus_Version_MAJOR 0 // Major: incremented by major releases
#define ASitus_Version_MINOR 0 // Minor: incremented by minor releases
#define ASitus_Version_PATCH 0 // Patch: incremented with bug-fix releases

//-----------------------------------------------------------------------------

// Cumulative version: 8 bits for each version number
#define ASitus_Version_HEX \
( (ASitus_Version_MAJOR << 16) | (ASitus_Version_MINOR << 8) | ASitus_Version_PATCH )

#define ASitus_Version_STRING "0.0.0"
#define ASitus_APP_NAME "Analysis Situs"

#endif
