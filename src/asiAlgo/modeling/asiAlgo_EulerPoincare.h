//-----------------------------------------------------------------------------
// Created on: 21 July 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiAlgo_EulerPoincare_h
#define asiAlgo_EulerPoincare_h

// Analysis Situs includes
#include <asiAlgo.h>

// OCCT includes
#include <TopoDS_Shape.hxx>

//! Utilities to deal with Euler characteristics of a CAD model.
namespace asiAlgo_EulerPoincare
{
  asiAlgo_EXPORT bool
    Check(const TopoDS_Shape& shape,
          const int           genus);
};

#endif
