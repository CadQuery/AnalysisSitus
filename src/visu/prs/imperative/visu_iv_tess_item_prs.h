//-----------------------------------------------------------------------------
// Created on: 23 May 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_iv_tess_item_prs_h
#define visu_iv_tess_item_prs_h

// Visualization includes
#include <visu_iv_prs.h>
#include <visu_iv_tess_item_node.h>

DEFINE_STANDARD_HANDLE(visu_iv_tess_item_prs, visu_iv_prs)

//! Presentation class for tessellation items in IV.
class visu_iv_tess_item_prs : public visu_iv_prs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_iv_tess_item_prs, visu_iv_prs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(visu_iv_tess_item_node, Instance)

public:

  //! Pipelines.
  enum PipelineId
  {
    Pipeline_Main = 1,
    Pipeline_MeshContour
  };

public:

  static Handle(visu_prs)
    Instance(const Handle(ActAPI_INode)& theNode);

private:

  //! Allocation is allowed only via Instance method.
  visu_iv_tess_item_prs(const Handle(ActAPI_INode)& theNode);

};

#endif
