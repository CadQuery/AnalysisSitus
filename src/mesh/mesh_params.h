//-----------------------------------------------------------------------------
// Created on: 03 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef mesh_params_h
#define mesh_params_h

// OCCT includes
#include <Standard_OStream.hxx>

//! Meshing parameters.
struct mesh_params
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
