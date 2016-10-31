//-----------------------------------------------------------------------------
// Created on: 12 (*) April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_iv_topo_item_prs_h
#define visu_iv_topo_item_prs_h

// Visualization includes
#include <visu_iv_prs.h>
#include <asiData_IVTopoItemNode.h>

DEFINE_STANDARD_HANDLE(visu_iv_topo_item_prs, visu_iv_prs)

//! Presentation class for topological items in IV.
class visu_iv_topo_item_prs : public visu_iv_prs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_iv_topo_item_prs, visu_iv_prs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(asiData_IVTopoItemNode, Instance)

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
  visu_iv_topo_item_prs(const Handle(ActAPI_INode)& theNode);

};

#endif
