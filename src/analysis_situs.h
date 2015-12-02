//-----------------------------------------------------------------------------
// Created on: 25 September 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef analysis_situs_h
#define analysis_situs_h

#ifdef ASITUS_EXPORTS
#define ASitus_EXPORT __declspec(dllexport)
#else
#define ASitus_EXPORT __declspec(dllimport)
#endif

#define ASitus_NotUsed(x)

// Active Data (API) includes
#include <ActAPI_Common.h>

// VTK includes
#include <vtkAutoInit.h>

#endif
