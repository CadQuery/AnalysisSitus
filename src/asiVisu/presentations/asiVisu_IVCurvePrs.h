//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_iv_curve_prs_h
#define visu_iv_curve_prs_h

// Visualization includes
#include <asiData_IVCurveNode.h>
#include <visu_iv_prs.h>

DEFINE_STANDARD_HANDLE(visu_iv_curve_prs, visu_iv_prs)

//! Presentation class for curves in IV.
class visu_iv_curve_prs : public visu_iv_prs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_iv_curve_prs, visu_iv_prs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(asiData_IVCurveNode, Instance)

public:

  //! Pipelines.
  enum PipelineId
  {
    Pipeline_Main = 1
  };

public:

  static Handle(visu_prs)
    Instance(const Handle(ActAPI_INode)& theNode);

private:

  //! Allocation is allowed only via Instance method.
  visu_iv_curve_prs(const Handle(ActAPI_INode)& theNode);

};

#endif
