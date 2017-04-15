//-----------------------------------------------------------------------------
// Created on: 30 November 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_PartPipeline_h
#define asiVisu_PartPipeline_h

// asiVisu includes
#include <asiVisu_PartPipelineBase.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_PartPipeline, asiVisu_PartPipelineBase)

//! Visualization pipeline for OCCT shapes. This pipeline serves the purpose
//! of visualization of "not-so-valid" shapes. Such shapes can occur either
//! because of any sort of corruption, or they can represent some intermediate
//! state of modeling, e.g. a result of Euler Operation which breaks geometry,
//! but keeps topology consistent.
class asiVisu_PartPipeline : public asiVisu_PartPipelineBase
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_PartPipeline, asiVisu_PartPipelineBase)

public:

  asiVisu_EXPORT
    asiVisu_PartPipeline();

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& dataProvider);

};

#endif
