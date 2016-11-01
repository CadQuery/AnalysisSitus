//-----------------------------------------------------------------------------
// Created on: 22 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_GeomBoundaryEdgesPrs_h
#define asiVisu_GeomBoundaryEdgesPrs_h

// A-Situs (visualization) includes
#include <asiVisu_Prs.h>
#include <asiVisu_Utils.h>

// A-Situs (geometry) includes
#include <asiData_BoundaryEdgesNode.h>

// Qt includes
#include <QColor>

// VTK includes
#include <vtkActor.h>

DEFINE_STANDARD_HANDLE(asiVisu_GeomBoundaryEdgesPrs, asiVisu_Prs)

//! Presentation class for b-rep edges.
class asiVisu_GeomBoundaryEdgesPrs : public asiVisu_Prs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_GeomBoundaryEdgesPrs, asiVisu_Prs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(asiData_BoundaryEdgesNode, Instance)

public:

  //! Pipelines.
  enum PipelineId
  {
    Pipeline_Convex = 1,
    Pipeline_Concave,
    Pipeline_Undefined
  };

public:

  static Handle(asiVisu_Prs)
    Instance(const Handle(ActAPI_INode)& theNode);

  virtual bool
    IsVisible() const;

private:

  //! Allocation is allowed only via Instance method.
  asiVisu_GeomBoundaryEdgesPrs(const Handle(ActAPI_INode)& theNode);

// Callbacks:
private:

  virtual void beforeInitPipelines();
  virtual void afterInitPipelines();
  virtual void beforeUpdatePipelines() const;
  virtual void afterUpdatePipelines() const;
  virtual void highlight(vtkRenderer* theRenderer,
                         const asiUI_PickResult& thePickRes,
                         const asiUI_SelectionNature& theSelNature) const;
  virtual void unHighlight(vtkRenderer* theRenderer,
                           const asiUI_SelectionNature& theSelNature) const;
  virtual void renderPipelines(vtkRenderer* theRenderer) const;
  virtual void deRenderPipelines(vtkRenderer* theRenderer) const;

};

#endif
