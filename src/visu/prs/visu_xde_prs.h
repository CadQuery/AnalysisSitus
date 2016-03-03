//-----------------------------------------------------------------------------
// Created on: 04 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_xde_prs_h
#define visu_xde_prs_h

// A-Situs (visualization) includes
#include <visu_utils.h>

// A-Situs (xde) includes
#include <xde_model.h>

DEFINE_STANDARD_HANDLE(visu_xde_prs, Standard_Transient)

//! Presentation class for XDE data.
class visu_xde_prs : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_xde_prs, Standard_Transient)

public:

  //! Pipelines.
  enum PipelineId
  {
    Pipeline_Geometry = 1
  };

public:

  static Handle(visu_xde_prs)
    Instance(const Handle(xde_model)& M);

private:

  //! Allocation is allowed only via Instance method.
  visu_xde_prs(const Handle(xde_model)& M);

};

#endif
