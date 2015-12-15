//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef geom_STEP_h
#define geom_STEP_h

// A-Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <Precision.hxx>
#include <TCollection_AsciiString.hxx>

//! STEP interoperability tool.
class geom_STEP
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI(geom_STEP, Standard_Transient)

public:

  static bool Read(const TCollection_AsciiString& filename,
                   const bool                     doHealing,
                   TopoDS_Shape&                  result);

private:

  geom_STEP() {}
  ~geom_STEP() {}

};

#endif
