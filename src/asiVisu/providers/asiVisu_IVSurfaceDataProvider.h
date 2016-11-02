//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_IVSurfaceDataProvider_h
#define asiVisu_IVSurfaceDataProvider_h

// A-Situs visualization includes
#include <asiVisu_DataProvider.h>

// OCCT includes
#include <Geom_Surface.hxx>

DEFINE_STANDARD_HANDLE(asiVisu_IVSurfaceDataProvider, asiVisu_DataProvider)

//! Data provider for a surface in IV.
class asiVisu_IVSurfaceDataProvider : public asiVisu_DataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_IVSurfaceDataProvider, asiVisu_DataProvider)

public:

  asiVisu_EXPORT
    asiVisu_IVSurfaceDataProvider(const Handle(ActAPI_INode)& N);

public:

  asiVisu_EXPORT virtual ActAPI_DataObjectId
    GetNodeID() const;

public:

  asiVisu_EXPORT Handle(Geom_Surface)
    GetSurface(double& uLimit, double& vLimit) const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

private:

  Handle(ActAPI_INode) m_node; //!< Source Node.

};

#endif
