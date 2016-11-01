//-----------------------------------------------------------------------------
// Created on: 09 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_SectionsDataProvider_h
#define asiVisu_SectionsDataProvider_h

// A-Situs visualization includes
#include <asiVisu_DataProvider.h>

// OCCT includes
#include <TopoDS_Compound.hxx>

DEFINE_STANDARD_HANDLE(asiVisu_SectionsDataProvider, asiVisu_DataProvider)

//! Data provider for a composition of skinning sections.
class asiVisu_SectionsDataProvider : public asiVisu_DataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_SectionsDataProvider, asiVisu_DataProvider)

public:

  asiVisu_SectionsDataProvider(const ActAPI_DataObjectId& nodeId);

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
