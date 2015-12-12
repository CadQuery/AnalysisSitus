//-----------------------------------------------------------------------------
// Created on: 09 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <visu_sections_prs.h>

// A-Situs (visualization) includes
#include <visu_sections_data_provider.h>
#include <visu_sections_pipeline.h>
#include <visu_shape_data_provider.h>
#include <visu_shape_pipeline.h>

// VTK includes
#include <vtkProperty.h>

//! Creates a Presentation object for the passed Sections Node.
//! \param N [in] Sections Node to create a Presentation for.
visu_sections_prs::visu_sections_prs(const Handle(ActAPI_INode)& N)
: visu_prs(N)
{
  // Pipeline for sections
  this->addPipeline        ( Pipeline_Sections, new visu_sections_pipeline() );
  this->assignDataProvider ( Pipeline_Sections, new visu_sections_data_provider( N->GetId() ) );

  // Pipeline for skin
  this->addPipeline        ( Pipeline_Skin, new visu_shape_pipeline() );
  this->assignDataProvider ( Pipeline_Skin, new visu_shape_data_provider( N->GetId(),
                                                                          ActParamStream() << N->Parameter(geom_sections_node::PID_Geometry) ) );

  // Tuning
  this->GetPipeline(Pipeline_Sections)->Actor()->GetProperty()->SetColor(1.0, 0.7, 0.2);
  this->GetPipeline(Pipeline_Sections)->Actor()->GetProperty()->SetLineWidth(2.0f);
}

//! Factory method for Presentation.
//! \param N [in] Sections Node to create a Presentation for.
//! \return new Presentation instance.
Handle(visu_prs) visu_sections_prs::Instance(const Handle(ActAPI_INode)& N)
{
  return new visu_sections_prs(N);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool visu_sections_prs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void visu_sections_prs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void visu_sections_prs::afterInitPipelines()
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void visu_sections_prs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void visu_sections_prs::afterUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for highlighting.
//! \param theRenderer  [in] renderer.
//! \param thePickRes   [in] picking results.
//! \param theSelNature [in] selection nature (picking or detecting).
void visu_sections_prs::highlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                  const visu_pick_result&      ASitus_NotUsed(thePickRes),
                                  const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for highlighting reset.
//! \param theRenderer [in] renderer.
void visu_sections_prs::unHighlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                    const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void visu_sections_prs::renderPipelines(vtkRenderer* ASitus_NotUsed(theRenderer)) const
{
  // Do nothing...
}

//! Callback for de-rendering.
//! \param theRenderer [in] renderer.
void visu_sections_prs::deRenderPipelines(vtkRenderer* ASitus_NotUsed(theRenderer)) const
{
  // Do nothing...
}
