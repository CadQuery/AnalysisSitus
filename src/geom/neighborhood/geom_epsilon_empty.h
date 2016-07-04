//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_epsilon_empty_h
#define geom_epsilon_empty_h

// Geometry includes
#include <geom_epsilon_base.h>

DEFINE_STANDARD_HANDLE(geom_epsilon_empty, geom_epsilon_base)

//! Empty neighborhood.
class geom_epsilon_empty : public geom_epsilon_base
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_epsilon_empty, geom_epsilon_base)

public:

  geom_epsilon_empty() {} //!< Default constructor.

};

#endif
