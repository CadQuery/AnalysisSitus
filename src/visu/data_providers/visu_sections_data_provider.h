//-----------------------------------------------------------------------------
// Created on: 09 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_sections_data_provider_h
#define visu_sections_data_provider_h

// A-Situs visualization includes
#include <visu_data_provider.h>

// OCCT includes
#include <TopoDS_Compound.hxx>

DEFINE_STANDARD_HANDLE(visu_sections_data_provider, visu_data_provider)

//! Data provider for a composition of skinning sections.
class visu_sections_data_provider : public visu_data_provider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_sections_data_provider, visu_data_provider)

public:

  visu_sections_data_provider(const ActAPI_DataObjectId& nodeId);

public:

  ActAPI_DataObjectId GetNodeID()              const;
  TopoDS_Compound     GetTransformedSections() const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

private:

  ActAPI_DataObjectId m_nodeID; //!< Source Node ID.

};

#endif
