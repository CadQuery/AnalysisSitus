//-----------------------------------------------------------------------------
// Created on: 17 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_IGES_h
#define geom_IGES_h

// A-Situs includes
#include <asitusGeom.h>

// OCCT includes
#include <Precision.hxx>
#include <TCollection_AsciiString.hxx>

//! IGES interoperability tool.
class geom_IGES
{
public:

  static bool Read(const TCollection_AsciiString& filename,
                   const bool                     doHealing,
                   TopoDS_Shape&                  result);

private:

  geom_IGES() {}
  ~geom_IGES() {}

};

#endif
