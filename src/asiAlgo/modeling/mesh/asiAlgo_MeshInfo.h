//-----------------------------------------------------------------------------
// Created on: 16 February 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_MeshInfo_h
#define asiAlgo_MeshInfo_h

// asiAlgo includes
#include <asiAlgo.h>

// OCCT includes
#include <Standard_OStream.hxx>

//! Mesh information.
struct asiAlgo_MeshInfo
{
  int    nNodes;
  int    nFacets;
  double maxDeflection;

  asiAlgo_MeshInfo() : nNodes(0), nFacets(0), maxDeflection(0.0) {}

  asiAlgo_EXPORT
    void Dump(Standard_OStream& out);
};

#endif
