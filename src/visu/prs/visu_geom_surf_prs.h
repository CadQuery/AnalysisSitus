//-----------------------------------------------------------------------------
// Created on: 04 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_geom_surf_prs_h
#define visu_geom_surf_prs_h

// A-Situs (visualization) includes
#include <visu_prs.h>
#include <visu_utils.h>

// A-Situs (geometry) includes
#include <geom_part_node.h>

// VTK includes
#include <vtkTextWidget.h>

DEFINE_STANDARD_HANDLE(visu_geom_surf_prs, visu_prs)

//! Presentation class for a host surface of a b-rep face.
class visu_geom_surf_prs : public visu_prs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_geom_surf_prs, visu_prs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(geom_surf_node, Instance)

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
  visu_geom_surf_prs(const Handle(ActAPI_INode)& theNode);

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

private:

  vtkSmartPointer<vtkTextWidget> m_textWidget; //!< Annotation.

};

#endif
  