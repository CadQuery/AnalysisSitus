//-----------------------------------------------------------------------------
// Created on: 03 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiEngine_Mesh_h
#define asiEngine_Mesh_h

// A-Situs includes
#include <asiEngine.h>

// Mesh includes
#include <asiAlgo_MeshParams.h>

//! Data Model API for meshing.
namespace asiEngine_Mesh
{
  asiEngine_EXPORT void
    SaveParameters(const asiAlgo_MeshParams& params);

  asiEngine_EXPORT void
    GetParameters(asiAlgo_MeshParams& params);
};

#endif
