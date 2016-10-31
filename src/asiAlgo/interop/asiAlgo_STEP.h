//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiAlgo_STEP_h
#define asiAlgo_STEP_h

// A-Situs includes
#include <asiAlgo.h>

// Geometry includes
#include <asiAlgo_InteropVars.h>

// OCCT includes
#include <Precision.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>

//! STEP interoperability tool.
namespace asiAlgo_STEP
{
  asiAlgo_EXPORT bool
    Read(const TCollection_AsciiString& filename,
         const bool                     doHealing,
         TopoDS_Shape&                  result);

  asiAlgo_EXPORT bool
    Write(const TopoDS_Shape&            shape,
          const TCollection_AsciiString& filename);
};

#endif
