//-----------------------------------------------------------------------------
// Created on: 06 April 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_PartEdgesPipeline_h
#define asiVisu_PartEdgesPipeline_h

// asiVisu includes
#include <asiVisu_PartPipelineBase.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_PartEdgesPipeline, asiVisu_PartPipelineBase)

//! Visualization pipeline for wireframe of OCCT shape. This pipeline is
//! secondary: it should be feeded with a source of a main shape pipeline.
class asiVisu_PartEdgesPipeline : public asiVisu_PartPipelineBase
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_PartEdgesPipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_PartEdgesPipeline(const vtkSmartPointer<asiVisu_ShapeRobustSource>& source);

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& dataProvider);

};

#endif
