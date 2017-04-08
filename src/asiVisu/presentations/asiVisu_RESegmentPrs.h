//-----------------------------------------------------------------------------
// Created on: 01 March 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_RESegmentPrs_h
#define asiVisu_RESegmentPrs_h

// asiVisu includes
#include <asiVisu_Prs.h>
#include <asiVisu_Utils.h>

// asiData includes
#include <asiData_RESegmentNode.h>

DEFINE_STANDARD_HANDLE(asiVisu_RESegmentPrs, asiVisu_Prs)

//! Presentation class for a segment of re-engineering point cloud.
class asiVisu_RESegmentPrs : public asiVisu_Prs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_RESegmentPrs, asiVisu_Prs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(asiData_RESegmentNode, Instance)

public:

  //! Pipelines.
  enum PipelineId
  {
    Pipeline_Main = 1
  };

public:

  asiVisu_EXPORT static Handle(asiVisu_Prs)
    Instance(const Handle(ActAPI_INode)& N);

  asiVisu_EXPORT virtual bool
    IsVisible() const;

private:

  //! Allocation is allowed only via Instance method.
  asiVisu_RESegmentPrs(const Handle(ActAPI_INode)& N);

// Callbacks:
private:

  virtual void beforeInitPipelines();
  virtual void afterInitPipelines();
  virtual void beforeUpdatePipelines() const;
  virtual void afterUpdatePipelines() const;
  virtual void highlight(vtkRenderer* theRenderer,
                         const asiVisu_PickResult& thePickRes,
                         const asiVisu_SelectionNature theSelNature) const;
  virtual void unHighlight(vtkRenderer* theRenderer,
                           const asiVisu_SelectionNature theSelNature) const;
  virtual void renderPipelines(vtkRenderer* theRenderer) const;
  virtual void deRenderPipelines(vtkRenderer* theRenderer) const;

};

#endif
