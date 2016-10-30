//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_re_points_prs.h>

// A-Situs (visualization) includes
#include <visu_re_points_data_provider.h>
#include <visu_points_pipeline.h>
#include <visu_utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//! Creates a Presentation object for the passed Node.
//! \param theNode [in] Node to create a Presentation for.
visu_re_points_prs::visu_re_points_prs(const Handle(ActAPI_INode)& theNode)
: visu_prs(theNode)
{
  // Create Data Provider
  Handle(visu_re_points_data_provider) DP = new visu_re_points_data_provider;

  // Pipeline for points
  this->addPipeline        ( Pipeline_Main, new visu_points_pipeline );
  this->assignDataProvider ( Pipeline_Main, DP );
}

//! Factory method for Presentation.
//! \param theNode [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(visu_prs) visu_re_points_prs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new visu_re_points_prs(theNode);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool visu_re_points_prs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void visu_re_points_prs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void visu_re_points_prs::afterInitPipelines()
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void visu_re_points_prs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void visu_re_points_prs::afterUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for highlighting.
void visu_re_points_prs::highlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                   const visu_pick_result&      ASitus_NotUsed(thePickRes),
                                   const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for highlighting reset.
void visu_re_points_prs::unHighlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                     const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for rendering.
void visu_re_points_prs::renderPipelines(vtkRenderer* ASitus_NotUsed(theRenderer)) const
{
  // Do nothing...
}

//! Callback for de-rendering.
void visu_re_points_prs::deRenderPipelines(vtkRenderer* ASitus_NotUsed(theRenderer)) const
{
  // Do nothing...
}
