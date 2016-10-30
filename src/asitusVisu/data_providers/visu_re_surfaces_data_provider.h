//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_re_surfaces_data_provider_h
#define visu_re_surfaces_data_provider_h

// A-Situs visualization includes
#include <visu_data_provider.h>

// OCCT includes
#include <TColGeom_SequenceOfSurface.hxx>

DEFINE_STANDARD_HANDLE(visu_re_surfaces_data_provider, visu_data_provider)

//! Data provider for re-engineering surfaces.
class visu_re_surfaces_data_provider : public visu_data_provider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_re_surfaces_data_provider, visu_data_provider)

public:

  visu_re_surfaces_data_provider();

public:

  virtual ActAPI_DataObjectId GetNodeID() const;

public:

  int
    GetNumOfSurfaces() const;

  Handle(Geom_Surface)
    GetSurface(const int oneBased_idx,
               double&   uLimit,
               double&   vLimit) const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

};

#endif
