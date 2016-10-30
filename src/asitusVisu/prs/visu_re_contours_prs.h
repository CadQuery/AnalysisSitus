//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_re_contours_prs_h
#define visu_re_contours_prs_h

// A-Situs (visualization) includes
#include <visu_prs.h>
#include <visu_utils.h>

// A-Situs (geometry) includes
#include <geom_re_contours_node.h>

DEFINE_STANDARD_HANDLE(visu_re_contours_prs, visu_prs)

//! Presentation class for re-engineering contours.
class visu_re_contours_prs : public visu_prs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_re_contours_prs, visu_prs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(geom_re_contours_node, Instance)

public:

  //! Pipelines.
  enum PipelineId
  {
    Pipeline_Main = 1
  };

public:

  static Handle(visu_prs)
    Instance(const Handle(ActAPI_INode)& theNode);

  virtual bool
    IsVisible() const;

private:

  //! Allocation is allowed only via Instance method.
  visu_re_contours_prs(const Handle(ActAPI_INode)& theNode);

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
