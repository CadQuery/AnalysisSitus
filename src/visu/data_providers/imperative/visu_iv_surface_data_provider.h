//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_iv_surface_data_provider_h
#define visu_iv_surface_data_provider_h

// A-Situs visualization includes
#include <visu_data_provider.h>

// OCCT includes
#include <Geom_Surface.hxx>

DEFINE_STANDARD_HANDLE(visu_iv_surface_data_provider, visu_data_provider)

//! Data provider for a surface in IV.
class visu_iv_surface_data_provider : public visu_data_provider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_iv_surface_data_provider, visu_data_provider)

public:

  visu_iv_surface_data_provider(const Handle(ActAPI_INode)& N);

public:

  virtual ActAPI_DataObjectId GetNodeID() const;

public:

  Handle(Geom_Surface)
    GetSurface(double& uLimit, double& vLimit) const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

private:

  Handle(ActAPI_INode) m_node; //!< Source Node.

};

#endif
