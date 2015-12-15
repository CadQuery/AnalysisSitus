//-----------------------------------------------------------------------------
// Created on: 09 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

// Own include
#include <visu_section_prs.h>

// A-Situs (visualization) includes
#include <visu_shape_data_provider.h>

// A-Situs (GUI) includes
#include <gui_common.h>

// VIS includes
#pragma warning(push, 0)
#include <IVtkTools_ShapeObject.hxx>
#pragma warning(pop)

#define COUT_DEBUG

//! Creates a Presentation object for the passed Section Node.
//! \param N [in] Section Node to create a Presentation for.
visu_section_prs::visu_section_prs(const Handle(ActAPI_INode)& N)
: visu_prs(N)
{
  // Create Data Provider
  Handle(visu_shape_data_provider) DP =
    new visu_shape_data_provider( N->GetId(),
                                  ActParamStream() << N->Parameter(geom_section_node::PID_Geometry) );

  // Main pipeline
  this->addPipeline        ( Pipeline_Main, new visu_shape_pipeline(true, true, false, false) );
  this->assignDataProvider ( Pipeline_Main, DP );

  // Set wireframe display mode
  Handle(visu_shape_pipeline)
    pl = Handle(visu_shape_pipeline)::DownCast( this->GetPipeline(Pipeline_Main) );
  //
  pl->WireframeModeOn();
}

//! Factory method for Presentation.
//! \param N [in] Section Node to create a Presentation for.
//! \return new Presentation instance.
Handle(visu_prs) visu_section_prs::Instance(const Handle(ActAPI_INode)& N)
{
  return new visu_section_prs(N);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool visu_section_prs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void visu_section_prs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void visu_section_prs::afterInitPipelines()
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void visu_section_prs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void visu_section_prs::afterUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for highlighting.
//! \param theRenderer  [in] renderer.
//! \param thePickRes   [in] picking results.
//! \param theSelNature [in] selection nature (picking or detecting).
void visu_section_prs::highlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                 const visu_pick_result&      ASitus_NotUsed(thePickRes),
                                 const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for highlighting reset.
//! \param theRenderer [in] renderer.
void visu_section_prs::unHighlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                   const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void visu_section_prs::renderPipelines(vtkRenderer* ASitus_NotUsed(theRenderer)) const
{
  // Do nothing...
}

//! Callback for de-rendering.
//! \param theRenderer [in] renderer.
void visu_section_prs::deRenderPipelines(vtkRenderer* ASitus_NotUsed(theRenderer)) const
{
  // Do nothing...
}
