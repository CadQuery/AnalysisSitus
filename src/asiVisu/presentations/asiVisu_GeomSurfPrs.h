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
#include <asiData_PartNode.h>

// VTK includes
#include <vtkButtonWidget.h>
#include <vtkCommand.h>
#include <vtkScalarBarWidget.h>
#include <vtkTextWidget.h>

class visu_surf_btn_callback;

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(visu_geom_surf_prs, visu_prs)

//! Presentation class for a host surface of a b-rep face.
class visu_geom_surf_prs : public visu_prs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_geom_surf_prs, visu_prs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(asiData_SurfNode, Instance)

public:

  //! Pipelines.
  enum PipelineId
  {
    Pipeline_Main = 1,
    Pipeline_Shaded
  };

public:

  static Handle(visu_prs)
    Instance(const Handle(ActAPI_INode)& theNode);

  virtual bool
    IsVisible() const;

//-----------------------------------------------------------------------------
public:

  void DoWireframe();
  void DoShaded();
  void DoShadedAndGaussian();
  void DoShadedAndMean();
  //
  bool IsShaded() const;
  bool IsShadedAndScalars() const;

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

  vtkSmartPointer<vtkTextWidget>          m_textWidget;      //!< Annotation.
  vtkSmartPointer<vtkScalarBarWidget>     m_scalarBarWidget; //!< Scalar bar.
  vtkSmartPointer<vtkButtonWidget>        m_modeButton;      //!< Button to switch visualization modes.
  vtkSmartPointer<visu_surf_btn_callback> m_modeCallback;    //!< Callback for visualization mode button.

};

//-----------------------------------------------------------------------------

//! Callback for switching visualization mode.
class visu_surf_btn_callback : public vtkCommand
{
public:

  static visu_surf_btn_callback* New();
  vtkTypeMacro(visu_surf_btn_callback, vtkCommand);

  visu_surf_btn_callback() {}

public:

  void SetPresentation(const Handle(visu_geom_surf_prs)& prs)
  {
    m_prs = prs;
  }

  virtual void Execute(vtkObject*    theCaller,
                       unsigned long theEventId,
                       void*         theCallData);

private:

  Handle(visu_geom_surf_prs) m_prs; //!< Listened Presentation.

};

#endif
