//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_LawDataProvider_h
#define asiVisu_LawDataProvider_h

// A-Situs (visualization) includes
#include <asiVisu_DataProvider.h>

// A-Situs (calculus) includes
#include <asiAlgo_DesignLaw.h>
#include <asiData_DesignLawNode.h>

DEFINE_STANDARD_HANDLE(asiVisu_LawDataProvider, asiVisu_DataProvider)

//! Data provider for explicitly defined design law.
class asiVisu_LawDataProvider : public asiVisu_DataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_LawDataProvider, asiVisu_DataProvider)

public:

  asiVisu_EXPORT
    asiVisu_LawDataProvider(const Handle(asiData_DesignLawNode)& N);

public:

  asiVisu_EXPORT virtual Handle(asiAlgo_DesignLaw)
    GetLaw() const;

  asiVisu_EXPORT virtual ActAPI_DataObjectId
    GetNodeID() const;

protected:

  virtual Handle(ActAPI_HParameterList) translationSources() const;

protected:

  Handle(asiData_DesignLawNode) m_node; //!< Source Node.

};

#endif
