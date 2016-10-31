//-----------------------------------------------------------------------------
// Created on: 15 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiAlgo_MeshGen_h
#define asiAlgo_MeshGen_h

// A-Situs includes
#include <asiAlgo.h>

// Mesh includes
#include <asiAlgo_MeshInfo.h>

// OCCT includes
#include <TopoDS_Shape.hxx>

//! Services related to mesh generation.
namespace asiAlgo_MeshGen
{
  asiAlgo_EXPORT
    bool DoNative(const TopoDS_Shape& shape,
                  const double        linearDeflection,
                  const double        angularDeflection_deg,
                  asiAlgo_MeshInfo&          info);
};

#endif
