//-----------------------------------------------------------------------------
// Created on: 02 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

// Own include
#include <visu_ubend_prs.h>

// A-Situs (visualization) includes
#include <visu_shape_data_provider.h>
#include <visu_shape_pipeline.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//! Creates a Presentation object for the passed U-bend Node.
//! \param N [in] U-bend Node to create a Presentation for.
visu_ubend_prs::visu_ubend_prs(const Handle(ActAPI_INode)& N)
: visu_prs(N)
{
  // Pipeline for path
  this->addPipeline        ( Pipeline_Path, new visu_shape_pipeline() );
  this->assignDataProvider ( Pipeline_Path, new visu_shape_data_provider( N->GetId(),
                                                                          ActParamStream() << N->Parameter(geom_ubend_node::PID_Path) ) );

  // Pipeline for sections
  this->addPipeline        ( Pipeline_Sections, new visu_shape_pipeline(false) );
  this->assignDataProvider ( Pipeline_Sections, new visu_shape_data_provider( N->GetId(),
                                                                              ActParamStream() << N->Parameter(geom_ubend_node::PID_Sections) ) );

  // Pipeline for skin
  this->addPipeline        ( Pipeline_Skin, new visu_shape_pipeline() );
  this->assignDataProvider ( Pipeline_Skin, new visu_shape_data_provider( N->GetId(),
                                                                          ActParamStream() << N->Parameter(geom_ubend_node::PID_Skin) ) );

  // Tuning
  Handle(visu_shape_pipeline) path_pl = Handle(visu_shape_pipeline)::DownCast( this->GetPipeline(Pipeline_Path) );
  Handle(visu_shape_pipeline) sect_pl = Handle(visu_shape_pipeline)::DownCast( this->GetPipeline(Pipeline_Sections) );
  Handle(visu_shape_pipeline) skin_pl = Handle(visu_shape_pipeline)::DownCast( this->GetPipeline(Pipeline_Skin) );
  //
  path_pl->Actor()->GetProperty()->SetLineWidth(1.0f);
  path_pl->WireframeModeOn(); // We can see edges and wires in wireframe mode only
  //
  sect_pl->Actor()->GetProperty()->SetLineWidth(3.0f);
  sect_pl->Actor()->GetProperty()->SetColor(1.0, 1.0, 0.0);
  sect_pl->Mapper()->ScalarVisibilityOff();
  sect_pl->WireframeModeOn(); // We can see edges and wires in wireframe mode only
  //
  skin_pl->Actor()->GetProperty()->SetOpacity(0.9);
}

//! Factory method for Presentation.
//! \param N [in] U-bend Node to create a Presentation for.
//! \return new Presentation instance.
Handle(visu_prs) visu_ubend_prs::Instance(const Handle(ActAPI_INode)& N)
{
  return new visu_ubend_prs(N);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool visu_ubend_prs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void visu_ubend_prs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void visu_ubend_prs::afterInitPipelines()
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void visu_ubend_prs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void visu_ubend_prs::afterUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for highlighting.
//! \param theRenderer  [in] renderer.
//! \param thePickRes   [in] picking results.
//! \param theSelNature [in] selection nature (picking or detecting).
void visu_ubend_prs::highlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                               const visu_pick_result&      ASitus_NotUsed(thePickRes),
                               const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for highlighting reset.
//! \param theRenderer [in] renderer.
void visu_ubend_prs::unHighlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                 const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void visu_ubend_prs::renderPipelines(vtkRenderer* ASitus_NotUsed(theRenderer)) const
{
  // Do nothing...
}

//! Callback for de-rendering.
//! \param theRenderer [in] renderer.
void visu_ubend_prs::deRenderPipelines(vtkRenderer* ASitus_NotUsed(theRenderer)) const
{
  // Do nothing...
}
