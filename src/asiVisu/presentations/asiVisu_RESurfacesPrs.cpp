//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_RESurfacesPrs.h>

// A-Situs (visualization) includes
#include <asiVisu_RESurfacesDataProvider.h>
#include <asiVisu_RESurfacesPipeline.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//! Creates a Presentation object for the passed Node.
//! \param theNode [in] Node to create a Presentation for.
asiVisu_RESurfacesPrs::asiVisu_RESurfacesPrs(const Handle(ActAPI_INode)& theNode)
: asiVisu_Prs(theNode)
{
  // Create Data Provider
  Handle(asiVisu_RESurfacesDataProvider) DP = new asiVisu_RESurfacesDataProvider;

  // Pipeline for surfaces
  this->addPipeline        ( Pipeline_Main, new asiVisu_RESurfacesPipeline );
  this->assignDataProvider ( Pipeline_Main, DP );
}

//! Factory method for Presentation.
//! \param theNode [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_RESurfacesPrs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new asiVisu_RESurfacesPrs(theNode);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool asiVisu_RESurfacesPrs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_RESurfacesPrs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void asiVisu_RESurfacesPrs::afterInitPipelines()
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void asiVisu_RESurfacesPrs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void asiVisu_RESurfacesPrs::afterUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for highlighting.
void asiVisu_RESurfacesPrs::highlight(vtkRenderer*                 asiVisu_NotUsed(theRenderer),
                                     const asiVisu_PickResult&      asiVisu_NotUsed(thePickRes),
                                     const asiVisu_SelectionNature& asiVisu_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for highlighting reset.
void asiVisu_RESurfacesPrs::unHighlight(vtkRenderer*                 asiVisu_NotUsed(theRenderer),
                                       const asiVisu_SelectionNature& asiVisu_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for rendering.
void asiVisu_RESurfacesPrs::renderPipelines(vtkRenderer* asiVisu_NotUsed(theRenderer)) const
{
  // Do nothing...
}

//! Callback for de-rendering.
void asiVisu_RESurfacesPrs::deRenderPipelines(vtkRenderer* asiVisu_NotUsed(theRenderer)) const
{
  // Do nothing...
}
