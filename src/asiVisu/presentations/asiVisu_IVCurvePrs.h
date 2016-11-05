//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_IVCurvePrs_h
#define asiVisu_IVCurvePrs_h

// Visualization includes
#include <asiData_IVCurveNode.h>
#include <asiVisu_IVPrs.h>

DEFINE_STANDARD_HANDLE(asiVisu_IVCurvePrs, asiVisu_IVPrs)

//! Presentation class for curves in IV.
class asiVisu_IVCurvePrs : public asiVisu_IVPrs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_IVCurvePrs, asiVisu_IVPrs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(asiData_IVCurveNode, Instance)

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
  asiVisu_IVCurvePrs(const Handle(ActAPI_INode)& theNode);

};

#endif
