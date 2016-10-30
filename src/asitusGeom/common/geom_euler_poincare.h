//-----------------------------------------------------------------------------
// Created on: 21 July 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_euler_poincare_h
#define geom_euler_poincare_h

// Analysis Situs includes
#include <asitusGeom.h>

//! Utilities to deal with Euler characteristics of a CAD model.
namespace geom_euler_poincare
{
  bool Check(const TopoDS_Shape& shape,
             const int           genus);
};

#endif
