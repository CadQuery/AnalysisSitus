//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_STEP_h
#define geom_STEP_h

// A-Situs includes
#include <analysis_situs.h>

// Geometry includes
#include <geom_DE_vars.h>

// OCCT includes
#include <Precision.hxx>
#include <TCollection_AsciiString.hxx>

//! STEP interoperability tool.
class geom_STEP
{
public:

  static bool Read(const TCollection_AsciiString& filename,
                   const bool                     doHealing,
                   TopoDS_Shape&                  result);

  static bool Write(const TopoDS_Shape&            shape,
                    const TCollection_AsciiString& filename);

private:

  geom_STEP() {}
  ~geom_STEP() {}

};

#endif
