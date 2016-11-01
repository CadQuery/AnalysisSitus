//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_REPointsPrs.h>

// A-Situs (visualization) includes
#include <asiVisu_REPointsDataProvider.h>
#include <asiVisu_PointsPipeline.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//! Creates a Presentation object for the passed Node.
//! \param theNode [in] Node to create a Presentation for.
asiVisu_REPointsPrs::asiVisu_REPointsPrs(const Handle(ActAPI_INode)& theNode)
: asiVisu_Prs(theNode)
{
  // Create Data Provider
  Handle(asiVisu_REPointsDataProvider) DP = new asiVisu_REPointsDataProvider;

  // Pipeline for points
  this->addPipeline        ( Pipeline_Main, new asiVisu_PointsPipeline );
  this->assignDataProvider ( Pipeline_Main, DP );
}

//! Factory method for Presentation.
//! \param theNode [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_REPointsPrs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new asiVisu_REPointsPrs(theNode);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool asiVisu_REPointsPrs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_REPointsPrs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void asiVisu_REPointsPrs::afterInitPipelines()
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void asiVisu_REPointsPrs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void asiVisu_REPointsPrs::afterUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for highlighting.
void asiVisu_REPointsPrs::highlight(vtkRenderer*                 asiVisu_NotUsed(theRenderer),
                                   const asiVisu_PickResult&      asiVisu_NotUsed(thePickRes),
                                   const asiVisu_SelectionNature& asiVisu_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for highlighting reset.
void asiVisu_REPointsPrs::unHighlight(vtkRenderer*                 asiVisu_NotUsed(theRenderer),
                                     const asiVisu_SelectionNature& asiVisu_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for rendering.
void asiVisu_REPointsPrs::renderPipelines(vtkRenderer* asiVisu_NotUsed(theRenderer)) const
{
  // Do nothing...
}

//! Callback for de-rendering.
void asiVisu_REPointsPrs::deRenderPipelines(vtkRenderer* asiVisu_NotUsed(theRenderer)) const
{
  // Do nothing...
}
