//-----------------------------------------------------------------------------
// Created on: 16 February 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_MeshInfo.h>

//! Dumps mesh information to the given output stream.
//! \param out [in/out] target output stream.
void asiAlgo_MeshInfo::Dump(Standard_OStream& out)
{
  out << "Num. triangles: "        << nFacets       << "\n";
  out << "Num. nodes: "            << nNodes        << "\n";
  out << "Max surface deviation: " << maxDeflection << "\n";
}
