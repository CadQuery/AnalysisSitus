//-----------------------------------------------------------------------------
// Created on: 13 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_geom_volume_prs_h
#define visu_geom_volume_prs_h

// Visualization includes
#include <visu_iv_prs.h>

// Geometry includes
#include <geom_volume_node.h>

DEFINE_STANDARD_HANDLE(visu_geom_volume_prs, visu_iv_prs)

//! Presentation class for volumetric parts (features).
class visu_geom_volume_prs : public visu_iv_prs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_geom_volume_prs, visu_iv_prs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(geom_volume_node, Instance)

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
  visu_geom_volume_prs(const Handle(ActAPI_INode)& theNode);

};

#endif
