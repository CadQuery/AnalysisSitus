//-----------------------------------------------------------------------------
// Created on: 21 July 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_IVTextItemPrs_h
#define asiVisu_IVTextItemPrs_h

// A-Situs (visualization) includes
#include <asiVisu_IVPrs.h>

// A-Situs (visualization) includes
#include <asiData_IVTextItemNode.h>

// VTK includes
#include <vtkTextWidget.h>

DEFINE_STANDARD_HANDLE(asiVisu_IVTextItemPrs, asiVisu_IVPrs)

//! Presentation class for a single text label in IV.
class asiVisu_IVTextItemPrs : public asiVisu_IVPrs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_IVTextItemPrs, asiVisu_IVPrs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(asiData_IVTextItemNode, Instance)

public:

  //! Pipelines.
  enum PipelineId
  {
    Pipeline_Main = 1
  };

public:

  asiVisu_EXPORT static Handle(asiVisu_Prs)
    Instance(const Handle(ActAPI_INode)& theNode);

// Callbacks:
private:

  virtual void afterInitPipelines ();
  virtual void renderPipelines    (vtkRenderer* theRenderer) const;
  virtual void deRenderPipelines  (vtkRenderer* theRenderer) const;

private:

  //! Allocation is allowed only via Instance method.
  asiVisu_IVTextItemPrs(const Handle(ActAPI_INode)& theNode);

private:

  vtkSmartPointer<vtkTextWidget> m_textWidget; //!< Annotation.

};

#endif
