//-----------------------------------------------------------------------------
// Created on: 15 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef tess_gen_h
#define tess_gen_h

// A-Situs includes
#include <analysis_situs.h>

// Mesh includes
#include <tess_info.h>

//! Services related to mesh generation.
namespace tess_gen
{
  bool DoNative(const TopoDS_Shape& shape,
                const double        linearDeflection,
                const double        angularDeflection_deg,
                tess_info&          info);
};

#endif
