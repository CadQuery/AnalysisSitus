//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_law_data_provider_h
#define visu_law_data_provider_h

// A-Situs (visualization) includes
#include <visu_data_provider.h>

// A-Situs (calculus) includes
#include <asiAlgo_DesignLaw.h>
#include <asiAlgo_DesignLaw_node.h>

DEFINE_STANDARD_HANDLE(visu_law_data_provider, visu_data_provider)

//! Data provider for explicitly defined design law.
class visu_law_data_provider : public visu_data_provider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_law_data_provider, visu_data_provider)

public:

  visu_law_data_provider(const Handle(asiAlgo_DesignLaw_node)& N);

public:

  virtual Handle(asiAlgo_DesignLaw) GetLaw()    const;
  virtual ActAPI_DataObjectId         GetNodeID() const;

protected:

  virtual Handle(ActAPI_HParameterList) translationSources() const;

protected:

  Handle(asiAlgo_DesignLaw_node) m_node; //!< Source Node.

};

#endif
