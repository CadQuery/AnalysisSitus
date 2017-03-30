//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_GeomPrs_h
#define asiVisu_GeomPrs_h

// A-Situs (visualization) includes
#include <asiVisu_Prs.h>
#include <asiVisu_Utils.h>

// A-Situs (geometry) includes
#include <asiData_PartNode.h>

// Qt includes
#include <QColor>

// VTK includes
#include <vtkActor.h>

DEFINE_STANDARD_HANDLE(asiVisu_GeomPrs, asiVisu_Prs)

//! Presentation class for B-Rep geometry.
class asiVisu_GeomPrs : public asiVisu_Prs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_GeomPrs, asiVisu_Prs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(asiData_PartNode, Instance)

public:

  //! Pipelines.
  enum PipelineId
  {
    Pipeline_Main = 1,
    Pipeline_Contour,
    Pipeline_Robust
  };

public:

  asiVisu_EXPORT static Handle(asiVisu_Prs)
    Instance(const Handle(ActAPI_INode)& N);

  asiVisu_EXPORT virtual bool
    IsVisible() const;

// Visualization commands:
public:

  asiVisu_EXPORT void DoShading() const;
  asiVisu_EXPORT void DoWireframe() const;
  asiVisu_EXPORT void DoColor(const QColor& color) const;
  asiVisu_EXPORT void DoUnColor() const;
  asiVisu_EXPORT void DoVertices(const bool on) const;

public:

  vtkActor* MainActor() const
  {
    return this->GetPipeline(Pipeline_Main)->Actor();
  }

private:

  //! Allocation is allowed only via Instance method.
  asiVisu_GeomPrs(const Handle(ActAPI_INode)& N);

// Callbacks:
private:

  virtual void beforeInitPipelines();
  virtual void afterInitPipelines();
  virtual void beforeUpdatePipelines() const;
  virtual void afterUpdatePipelines() const;
  virtual void highlight(vtkRenderer* renderer,
                         const asiVisu_PickResult& pickRes,
                         const asiVisu_SelectionNature& selNature) const;
  virtual void unHighlight(vtkRenderer* renderer,
                           const asiVisu_SelectionNature& selNature) const;
  virtual void renderPipelines(vtkRenderer* renderer) const;
  virtual void deRenderPipelines(vtkRenderer* renderer) const;

};

#endif
