//-----------------------------------------------------------------------------
// Created on: 03 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef engine_tess_h
#define engine_tess_h

// A-Situs includes
#include <analysis_situs.h>

// Mesh includes
#include <tess_params.h>

//! Data Model API for meshing.
namespace engine_mesh
{
  void
    SaveParameters(const tess_params& params);

  void
    GetParameters(tess_params& params);

};

#endif
