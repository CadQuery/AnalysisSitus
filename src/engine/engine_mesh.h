//-----------------------------------------------------------------------------
// Created on: 03 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef engine_mesh_h
#define engine_mesh_h

// A-Situs includes
#include <analysis_situs.h>

// Mesh includes
#include <mesh_params.h>

//! Data Model API for meshing.
class engine_mesh
{
public:

  static void
    SaveParameters(const mesh_params& params);

  static void
    GetParameters(mesh_params& params);

private:

  engine_mesh() {}
  engine_mesh(const engine_mesh&) {}
  void operator=(const engine_mesh&) {}

};

#endif
