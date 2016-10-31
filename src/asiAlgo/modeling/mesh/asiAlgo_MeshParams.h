//-----------------------------------------------------------------------------
// Created on: 03 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiAlgo_MeshParams_h
#define asiAlgo_MeshParams_h

// OCCT includes
#include <Standard_OStream.hxx>

//! Meshing parameters.
struct asiAlgo_MeshParams
{
  //---------------------------------------------------------------------------
  struct t_deflection
  {
    double Linear;
    double Angular;

    t_deflection() : Linear(0.0), Angular(0.0) {}
  };
  //---------------------------------------------------------------------------

  t_deflection Deflection; //!< Deflection parameters.
};

#endif
