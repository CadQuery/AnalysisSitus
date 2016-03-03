//-----------------------------------------------------------------------------
// Created on: 16 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef mesh_info_h
#define mesh_info_h

// OCCT includes
#include <Standard_OStream.hxx>

//! Mesh information.
struct mesh_info
{
  int    nNodes;
  int    nFacets;
  double maxDeflection;

  mesh_info() : nNodes(0), nFacets(0), maxDeflection(0.0) {}

  void Dump(Standard_OStream& out);
};

#endif
