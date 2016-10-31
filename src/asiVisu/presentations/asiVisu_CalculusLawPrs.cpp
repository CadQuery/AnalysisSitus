//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_calculus_law_prs.h>

// A-Situs (visualization) includes
#include <visu_law_data_provider.h>
#include <visu_law_pipeline.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//! Creates a Presentation object.
//! \param N [in] Data Node to create a Presentation for.
visu_calculus_law_prs::visu_calculus_law_prs(const Handle(ActAPI_INode)& N)
: visu_prs(N)
{
  // Pipeline for law
  this->addPipeline        ( Pipeline_Law, new visu_law_pipeline() );
  this->assignDataProvider ( Pipeline_Law, new visu_law_data_provider( Handle(asiAlgo_DesignLaw_node)::DownCast(N) ) );
}

//! Factory method for Presentation.
//! \param N [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(visu_prs) visu_calculus_law_prs::Instance(const Handle(ActAPI_INode)& N)
{
  return new visu_calculus_law_prs(N);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool visu_calculus_law_prs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void visu_calculus_law_prs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void visu_calculus_law_prs::afterInitPipelines()
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void visu_calculus_law_prs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void visu_calculus_law_prs::afterUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for highlighting.
//! \param theRenderer  [in] renderer.
//! \param thePickRes   [in] picking results.
//! \param theSelNature [in] selection nature (picking or detecting).
void visu_calculus_law_prs::highlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                      const visu_pick_result&      ASitus_NotUsed(thePickRes),
                                      const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for highlighting reset.
//! \param theRenderer [in] renderer.
void visu_calculus_law_prs::unHighlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                        const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void visu_calculus_law_prs::renderPipelines(vtkRenderer* ASitus_NotUsed(theRenderer)) const
{
  // Do nothing...
}

//! Callback for de-rendering.
//! \param theRenderer [in] renderer.
void visu_calculus_law_prs::deRenderPipelines(vtkRenderer* ASitus_NotUsed(theRenderer)) const
{
  // Do nothing...
}
