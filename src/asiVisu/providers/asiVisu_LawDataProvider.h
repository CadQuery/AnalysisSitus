//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_LawDataProvider_h
#define asiVisu_LawDataProvider_h

// asiVisu includes
#include <asiVisu_FuncUnivariateDataProvider.h>

// asiData includes
#include <asiData_DesignLawNode.h>

//! Data provider for explicitly defined design law.
class asiVisu_LawDataProvider : public asiVisu_FuncUnivariateDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_LawDataProvider, asiVisu_FuncUnivariateDataProvider)

public:

  asiVisu_EXPORT
    asiVisu_LawDataProvider(const Handle(asiData_DesignLawNode)& N);

public:

  asiVisu_EXPORT virtual Handle(asiAlgo_FuncUnivariate)
    GetFunc() const;

  asiVisu_EXPORT virtual ActAPI_DataObjectId
    GetNodeID() const;

protected:

  virtual Handle(ActAPI_HParameterList) translationSources() const;

protected:

  Handle(asiData_DesignLawNode) m_node; //!< Source Node.

};

#endif
