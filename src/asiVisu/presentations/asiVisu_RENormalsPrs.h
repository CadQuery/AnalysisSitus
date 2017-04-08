//-----------------------------------------------------------------------------
// Created on: 08 December 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_RENormalsPrs_h
#define asiVisu_RENormalsPrs_h

// asiVisu includes
#include <asiVisu_Prs.h>
#include <asiVisu_Utils.h>

// asiData includes
#include <asiData_RENormalsNode.h>

DEFINE_STANDARD_HANDLE(asiVisu_RENormalsPrs, asiVisu_Prs)

//! Presentation class for re-engineering normals.
class asiVisu_RENormalsPrs : public asiVisu_Prs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_RENormalsPrs, asiVisu_Prs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(asiData_RENormalsNode, Instance)

public:

  //! Pipelines.
  enum PipelineId
  {
    Pipeline_Main = 1
  };

public:

  asiVisu_EXPORT static Handle(asiVisu_Prs)
    Instance(const Handle(ActAPI_INode)& node);

  asiVisu_EXPORT virtual bool
    IsVisible() const;

private:

  //! Allocation is allowed only via Instance method.
  asiVisu_RENormalsPrs(const Handle(ActAPI_INode)& node);

// Callbacks:
private:

  virtual void beforeInitPipelines();
  virtual void afterInitPipelines();
  virtual void beforeUpdatePipelines() const;
  virtual void afterUpdatePipelines() const;
  virtual void highlight(vtkRenderer* renderer,
                         const asiVisu_PickResult& pickRes,
                         const asiVisu_SelectionNature selNature) const;
  virtual void unHighlight(vtkRenderer* renderer,
                           const asiVisu_SelectionNature selNature) const;
  virtual void renderPipelines(vtkRenderer* renderer) const;
  virtual void deRenderPipelines(vtkRenderer* renderer) const;

};

#endif
