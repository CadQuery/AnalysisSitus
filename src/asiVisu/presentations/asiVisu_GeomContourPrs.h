//-----------------------------------------------------------------------------
// Created on: 19 September 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_GeomContourPrs_h
#define asiVisu_GeomContourPrs_h

// A-Situs (visualization) includes
#include <asiVisu_Prs.h>
#include <asiVisu_Utils.h>

// A-Situs (geometry) includes
#include <asiData_PartNode.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_GeomContourPrs, asiVisu_Prs)

//! Presentation class for a contour.
class asiVisu_GeomContourPrs : public asiVisu_Prs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_GeomContourPrs, asiVisu_Prs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(asiData_ContourNode, Instance)

public:

  //! Pipelines.
  enum PipelineId
  {
    Pipeline_Main = 1,
    Pipeline_Points
  };

public:

  asiVisu_EXPORT static Handle(asiVisu_Prs)
    Instance(const Handle(ActAPI_INode)& theNode);

  asiVisu_EXPORT virtual bool
    IsVisible() const;

private:

  //! Allocation is allowed only via Instance method.
  asiVisu_GeomContourPrs(const Handle(ActAPI_INode)& theNode);

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
