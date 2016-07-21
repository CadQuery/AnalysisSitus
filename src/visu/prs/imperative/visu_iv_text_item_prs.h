//-----------------------------------------------------------------------------
// Created on: 21 July 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_iv_text_item_prs_h
#define visu_iv_text_item_prs_h

// A-Situs (visualization) includes
#include <visu_iv_prs.h>

// A-Situs (visualization) includes
#include <visu_iv_text_item_node.h>

// VTK includes
#include <vtkTextWidget.h>

DEFINE_STANDARD_HANDLE(visu_iv_text_item_prs, visu_iv_prs)

//! Presentation class for a single text label in IV.
class visu_iv_text_item_prs : public visu_iv_prs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_iv_text_item_prs, visu_iv_prs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(visu_iv_text_item_node, Instance)

public:

  //! Pipelines.
  enum PipelineId
  {
    Pipeline_Main = 1
  };

public:

  static Handle(visu_prs)
    Instance(const Handle(ActAPI_INode)& theNode);

// Callbacks:
private:

  virtual void afterInitPipelines ();
  virtual void renderPipelines    (vtkRenderer* theRenderer) const;
  virtual void deRenderPipelines  (vtkRenderer* theRenderer) const;

private:

  //! Allocation is allowed only via Instance method.
  visu_iv_text_item_prs(const Handle(ActAPI_INode)& theNode);

private:

  vtkSmartPointer<vtkTextWidget> m_textWidget; //!< Annotation.

};

#endif
