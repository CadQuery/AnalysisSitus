//-----------------------------------------------------------------------------
// Created on: 25 September 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su
//-----------------------------------------------------------------------------

#ifndef analysis_situs_h
#define analysis_situs_h

#ifdef ASITUS_EXPORTS
#define ASITUS_EXPORTS __declspec(dllexport)
#else
#define ASITUS_EXPORTS __declspec(dllimport)
#endif

// VTK includes
#include <vtkAutoInit.h>

// Activate object factory for Freetype fonts
VTK_MODULE_INIT(vtkRenderingFreeType);

#endif
