//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_RESurfacesDataProvider_h
#define asiVisu_RESurfacesDataProvider_h

// A-Situs visualization includes
#include <asiVisu_DataProvider.h>

// OCCT includes
#include <TColGeom_SequenceOfSurface.hxx>

DEFINE_STANDARD_HANDLE(asiVisu_RESurfacesDataProvider, asiVisu_DataProvider)

//! Data provider for re-engineering surfaces.
class asiVisu_RESurfacesDataProvider : public asiVisu_DataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_RESurfacesDataProvider, asiVisu_DataProvider)

public:

  asiVisu_RESurfacesDataProvider();

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
