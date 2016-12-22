//-----------------------------------------------------------------------------
// Created on: 16 April 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_IVPointSet2dPrs_h
#define asiVisu_IVPointSet2dPrs_h

// A-Situs (visualization) includes
#include <asiVisu_IVPrs.h>

// A-Situs (visualization) includes
#include <asiData_IVPointSet2dNode.h>

DEFINE_STANDARD_HANDLE(asiVisu_IVPointSet2dPrs, asiVisu_IVPrs)

//! Presentation class for a single 2D point cloud in IV.
class asiVisu_IVPointSet2dPrs : public asiVisu_IVPrs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_IVPointSet2dPrs, asiVisu_IVPrs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(asiData_IVPointSet2dNode, Instance)

public:

  //! Pipelines.
  enum PipelineId
  {
    Pipeline_Main = 1
  };

public:

  asiVisu_EXPORT static Handle(asiVisu_Prs)
    Instance(const Handle(ActAPI_INode)& theNode);

private:

  //! Allocation is allowed only via Instance method.
  asiVisu_IVPointSet2dPrs(const Handle(ActAPI_INode)& theNode);

};

#endif
