//-----------------------------------------------------------------------------
// Created on: 16 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <mesh_info.h>

//! Dumps mesh information to the given output stream.
//! \param out [in/out] target output stream.
void mesh_info::Dump(Standard_OStream& out)
{
  out << "Num. triangles: "        << nFacets       << "\n";
  out << "Num. nodes: "            << nNodes        << "\n";
  out << "Max surface deviation: " << maxDeflection << "\n";
}
