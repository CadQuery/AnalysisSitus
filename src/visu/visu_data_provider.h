//-----------------------------------------------------------------------------
// Created on: 28 September 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef visu_data_provider_h
#define visu_data_provider_h

// A-Situs includes
#include <analysis_situs.h>

// Active Data (API) includes
#include <ActAPI_IParameter.h>

// Active Data (auxiliary) includes
#include <ActAux_TimeStamp.h>

DEFINE_STANDARD_HANDLE(visu_data_provider, Standard_Transient)

//! The instances of this class are normally used to supply data to the VTK
//! visualization pipelines. The main idea behind this class is to provide
//! a universal way of feeding the pipelines with domain-specific data
//! structures for their subsequent translation to VTK data sets. Such
//! DOMAIN -> VTK translation process is usually quite heavy, so it should
//! perform only in cases when the DOMAIN data is actually changed. The latter
//! fact is normally checked by comparing modification time (MTime) of the
//! data source with the modification time of the pipeline.
class visu_data_provider : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI(visu_data_provider, Standard_Transient)

public:

  virtual bool
    MustExecute(const Handle(ActAux_TimeStamp)& Against) const;

public:

  //! Returns associated Node ID.
  //! \return Node ID.
  virtual ActAPI_DataObjectId
    GetNodeID() const = 0;

protected:

  //! Enumerates all Active Data Parameters playing as sources for DOMAIN -> VTK
  //! translation process. If any Parameter listed by this method is changed
  //! (more precisely, if its MTime record is updated), the translation must
  //! be repeated.
  //! \return list of source Parameters.
  virtual Handle(ActAPI_HParameterList)
    translationSources() const = 0;

};

#endif
