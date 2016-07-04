//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_epsilon_base_h
#define geom_epsilon_base_h

// Analysis Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <Standard_DefineHandle.hxx>
#include <Standard_Transient.hxx>

DEFINE_STANDARD_HANDLE(geom_epsilon_base, Standard_Transient)

//! Base class for all neighborhood representations. Neighborhood is a local
//! topological property which is used to facilitate "analysis situs" in
//! ambiguity conditions. This concept here is inspired by the works of
//! A.G. Requicha related to boundary evaluation and merging algorithms.
//! For sure, Contour Capture is an algorithm from that pool.
class geom_epsilon_base : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_epsilon_base, Standard_Transient)

protected:

  geom_epsilon_base() {}
  geom_epsilon_base(const geom_epsilon_base&) {}
  void operator=(const geom_epsilon_base&) {}

};

#endif
