//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_REContoursPrs.h>

// A-Situs (visualization) includes
#include <asiVisu_REContoursDataProvider.h>
#include <asiVisu_REContoursPipeline.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//! Creates a Presentation object for the passed Node.
//! \param theNode [in] Node to create a Presentation for.
asiVisu_REContoursPrs::asiVisu_REContoursPrs(const Handle(ActAPI_INode)& theNode)
: asiVisu_Prs(theNode)
{
  // Create Data Provider
  Handle(asiVisu_REContoursDataProvider) DP = new asiVisu_REContoursDataProvider;

  // Pipeline for contours
  this->addPipeline        ( Pipeline_Main, new asiVisu_REContoursPipeline );
  this->assignDataProvider ( Pipeline_Main, DP );
}

//! Factory method for Presentation.
//! \param theNode [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_REContoursPrs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new asiVisu_REContoursPrs(theNode);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool asiVisu_REContoursPrs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_REContoursPrs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void asiVisu_REContoursPrs::afterInitPipelines()
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void asiVisu_REContoursPrs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void asiVisu_REContoursPrs::afterUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for highlighting.
void asiVisu_REContoursPrs::highlight(vtkRenderer*                 asiVisu_NotUsed(theRenderer),
                                     const asiVisu_PickResult&      asiVisu_NotUsed(thePickRes),
                                     const asiVisu_SelectionNature& asiVisu_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for highlighting reset.
void asiVisu_REContoursPrs::unHighlight(vtkRenderer*                 asiVisu_NotUsed(theRenderer),
                                       const asiVisu_SelectionNature& asiVisu_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for rendering.
void asiVisu_REContoursPrs::renderPipelines(vtkRenderer* asiVisu_NotUsed(theRenderer)) const
{
  // Do nothing...
}

//! Callback for de-rendering.
void asiVisu_REContoursPrs::deRenderPipelines(vtkRenderer* asiVisu_NotUsed(theRenderer)) const
{
  // Do nothing...
}
