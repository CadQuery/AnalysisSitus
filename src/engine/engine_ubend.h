//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef engine_ubend_h
#define engine_ubend_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (geometry) includes
#include <geom_ubend_node.h>

//! Data Model API for U-bend.
class engine_ubend
{
public:

  static Handle(geom_ubend_node)
    Create_UBend();

private:

  engine_ubend() {}
  engine_ubend(const engine_ubend&) {}
  void operator=(const engine_ubend&) {}

};

#endif
