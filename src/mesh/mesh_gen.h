//-----------------------------------------------------------------------------
// Created on: 15 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef mesh_gen_h
#define mesh_gen_h

// A-Situs includes
#include <analysis_situs.h>

// Mesh includes
#include <mesh_info.h>

//! Services related to mesh generation.
namespace mesh_gen
{
  bool DoNative(const TopoDS_Shape& shape,
                const double        linearDeflection,
                const double        angularDeflection_deg,
                mesh_info&          info);
};

#endif
