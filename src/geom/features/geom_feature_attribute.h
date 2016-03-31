//-----------------------------------------------------------------------------
// Created on: 21 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_feature_attribute_h
#define geom_feature_attribute_h

// A-Situs includes
#include <analysis_situs.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_feature_attribute, Standard_Transient)

//! Base class for all feature attributes.
class geom_feature_attribute : public Standard_Transient
{
public:

  DEFINE_STANDARD_RTTI_INLINE(geom_feature_attribute, Standard_Transient)

public:

  virtual ~geom_feature_attribute() {}

};

#endif
