//-----------------------------------------------------------------------------
// Created on: 28 September 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su
//-----------------------------------------------------------------------------

#ifndef visu_data_provider_h
#define visu_data_provider_h

// SBM Visualization Framework includes
#include <SbmVisu_Common.h>

// SBM API includes
#include <ActAPI_IParameter.h>
#include <SbmAPI_IPipelineDataProvider.h>

// SBM Algorithmic layer includes
#include <ActAux_TimeStamp.h>

DEFINE_STANDARD_HANDLE(SbmVisu_BasePipelineDataProvider, SbmAPI_IPipelineDataProvider)

//! \ingroup VISU
//!
//! The instances of this class are normally used to supply data to the VTK
//! visualization pipelines. The main idea behind this class is to provide
//! a universal way of feeding the pipelines with domain-specific data
//! structures for their subsequent translation to VTK data sets. Such
//! DOMAIN -> VTK translation process is known to be quite heavy, so it should
//! perform only in cases when the DOMAIN data is actually changed. The latter
//! fact is normally checked by comparing modification time (MTime) of the
//! data source with the modification time of the pipeline.
class SbmVisu_BasePipelineDataProvider : public SbmAPI_IPipelineDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI(SbmVisu_BasePipelineDataProvider,SbmAPI_IPipelineDataProvider)

public:

  SbmVisu_EXPORT virtual Standard_Boolean
    MustExecute(const Handle(ActAux_TimeStamp)& Against) const;

protected:

  //! Enumerates all Nodal Parameters playing as sources for DOMAIN -> VTK
  //! translation process. If any Parameter listed by this method is changed
  //! (more precisely, if its MTime record is updated), the translation must
  //! be repeated.
  //! \return list of source Parameters.
  SbmVisu_EXPORT virtual Handle(ActAPI_ParameterList)
    translationSources() const = 0;

};

#endif
