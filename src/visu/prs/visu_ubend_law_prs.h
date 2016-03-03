//-----------------------------------------------------------------------------
// Created on: 10 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_ubend_law_prs_h
#define visu_ubend_law_prs_h

// A-Situs (visualization) includes
#include <visu_law_pipeline.h>
#include <visu_prs.h>
#include <visu_utils.h>

// A-Situs (geometry) includes
#include <geom_ubend_law_node.h>

DEFINE_STANDARD_HANDLE(visu_ubend_law_prs, visu_prs)

//! Presentation class for u-bend evolution law.
class visu_ubend_law_prs : public visu_prs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_ubend_law_prs, visu_prs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(geom_ubend_law_node, Instance)

public:

  //! Pipelines.
  enum PipelineId
  {
    Pipeline_Law_XY = 1,
    Pipeline_Law_XY_Poles
  };

public:

  static Handle(visu_prs)
    Instance(const Handle(ActAPI_INode)& theNode);

  virtual bool
    IsVisible() const;

private:

  //! Allocation is allowed only via Instance method.
  visu_ubend_law_prs(const Handle(ActAPI_INode)& theNode);

// Callbacks:
private:

  virtual void beforeInitPipelines();
  virtual void afterInitPipelines();
  virtual void beforeUpdatePipelines() const;
  virtual void afterUpdatePipelines() const;
  virtual void highlight(vtkRenderer* theRenderer,
                         const visu_pick_result& thePickRes,
                         const visu_selection_nature& theSelNature) const;
  virtual void unHighlight(vtkRenderer* theRenderer,
                           const visu_selection_nature& theSelNature) const;
  virtual void renderPipelines(vtkRenderer* theRenderer) const;
  virtual void deRenderPipelines(vtkRenderer* theRenderer) const;

};

#endif
