//-----------------------------------------------------------------------------
// Created on: 23 May 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_IVTessItemPrs_h
#define asiVisu_IVTessItemPrs_h

// Visualization includes
#include <asiVisu_IVPrs.h>
#include <asiData_IVTessItemNode.h>

DEFINE_STANDARD_HANDLE(asiVisu_IVTessItemPrs, asiVisu_IVPrs)

//! Presentation class for tessellation items in IV.
class asiVisu_IVTessItemPrs : public asiVisu_IVPrs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_IVTessItemPrs, asiVisu_IVPrs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(asiData_IVTessItemNode, Instance)

public:

  //! Pipelines.
  enum PipelineId
  {
    Pipeline_Main = 1,
    Pipeline_MeshContour
  };

public:

  asiVisu_EXPORT static Handle(asiVisu_Prs)
    Instance(const Handle(ActAPI_INode)& theNode);

private:

  //! Allocation is allowed only via Instance method.
  asiVisu_IVTessItemPrs(const Handle(ActAPI_INode)& theNode);

};

#endif
