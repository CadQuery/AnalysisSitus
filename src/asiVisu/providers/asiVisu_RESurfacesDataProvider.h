//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_RESurfacesDataProvider_h
#define asiVisu_RESurfacesDataProvider_h

// A-Situs visualization includes
#include <asiVisu_DataProvider.h>

// asiData includes
#include <asiData_RESurfacesNode.h>

// OCCT includes
#include <Geom_Surface.hxx>

DEFINE_STANDARD_HANDLE(asiVisu_RESurfacesDataProvider, asiVisu_DataProvider)

//! Data provider for re-engineering surfaces.
class asiVisu_RESurfacesDataProvider : public asiVisu_DataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_RESurfacesDataProvider, asiVisu_DataProvider)

public:

  asiVisu_EXPORT
    asiVisu_RESurfacesDataProvider(const Handle(asiData_RESurfacesNode)& surfaces_n);

public:

  asiVisu_EXPORT virtual ActAPI_DataObjectId
    GetNodeID() const;

public:

  asiVisu_EXPORT int
    GetNumOfSurfaces() const;

  asiVisu_EXPORT Handle(Geom_Surface)
    GetSurface(const int oneBased_idx,
               double&   uLimit,
               double&   vLimit) const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

protected:

  Handle(asiData_RESurfacesNode) m_surfaces; //!< Surfaces Node.

};

#endif
