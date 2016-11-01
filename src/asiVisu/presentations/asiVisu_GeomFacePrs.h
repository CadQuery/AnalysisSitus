//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_GeomFacePrs_h
#define asiVisu_GeomFacePrs_h

// A-Situs (visualization) includes
#include <asiVisu_Prs.h>
#include <asiVisu_Utils.h>

// A-Situs (geometry) includes
#include <asiData_PartNode.h>

// VTK includes
#include <vtkTextWidget.h>

DEFINE_STANDARD_HANDLE(asiVisu_GeomFacePrs, asiVisu_Prs)

//! Presentation class for b-rep face.
class asiVisu_GeomFacePrs : public asiVisu_Prs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_GeomFacePrs, asiVisu_Prs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(asiData_FaceNode, Instance)

public:

  //! Pipelines.
  enum PipelineId
  {
    Pipeline_Main = 1
  };

public:

  static Handle(asiVisu_Prs)
    Instance(const Handle(ActAPI_INode)& theNode);

  virtual bool
    IsVisible() const;

private:

  //! Allocation is allowed only via Instance method.
  asiVisu_GeomFacePrs(const Handle(ActAPI_INode)& theNode);

// Callbacks:
private:

  virtual void beforeInitPipelines();
  virtual void afterInitPipelines();
  virtual void beforeUpdatePipelines() const;
  virtual void afterUpdatePipelines() const;
  virtual void highlight(vtkRenderer* theRenderer,
                         const asiVisu_PickResult& thePickRes,
                         const asiVisu_SelectionNature& theSelNature) const;
  virtual void unHighlight(vtkRenderer* theRenderer,
                           const asiVisu_SelectionNature& theSelNature) const;
  virtual void renderPipelines(vtkRenderer* theRenderer) const;
  virtual void deRenderPipelines(vtkRenderer* theRenderer) const;

private:

  vtkSmartPointer<vtkTextWidget> m_textWidget; //!< Annotation.

};

#endif
  