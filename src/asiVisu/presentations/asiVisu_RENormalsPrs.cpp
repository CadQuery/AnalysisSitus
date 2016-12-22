//-----------------------------------------------------------------------------
// Created on: 08 December 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_RENormalsPrs.h>

// asiVisu includes
#include <asiVisu_PointsPipeline.h>
#include <asiVisu_RENormalsDataProvider.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//! Creates a Presentation object for the passed Node.
//! \param node [in] Node to create a Presentation for.
asiVisu_RENormalsPrs::asiVisu_RENormalsPrs(const Handle(ActAPI_INode)& node)
: asiVisu_Prs(node)
{
  Handle(asiData_RENormalsNode)
    points_n = Handle(asiData_RENormalsNode)::DownCast(node);

  // Create Data Provider
  Handle(asiVisu_RENormalsDataProvider) DP = new asiVisu_RENormalsDataProvider(points_n);

  // Pipeline for points
  this->addPipeline        ( Pipeline_Main, new asiVisu_PointsPipeline );
  this->assignDataProvider ( Pipeline_Main, DP );
}

//! Factory method for Presentation.
//! \param node [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_RENormalsPrs::Instance(const Handle(ActAPI_INode)& node)
{
  return new asiVisu_RENormalsPrs(node);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool asiVisu_RENormalsPrs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_RENormalsPrs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void asiVisu_RENormalsPrs::afterInitPipelines()
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void asiVisu_RENormalsPrs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void asiVisu_RENormalsPrs::afterUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for highlighting.
void asiVisu_RENormalsPrs::highlight(vtkRenderer*                   asiVisu_NotUsed(renderer),
                                     const asiVisu_PickResult&      asiVisu_NotUsed(pickRes),
                                     const asiVisu_SelectionNature& asiVisu_NotUsed(selNature)) const
{
  // Do nothing...
}

//! Callback for highlighting reset.
void asiVisu_RENormalsPrs::unHighlight(vtkRenderer*                   asiVisu_NotUsed(renderer),
                                       const asiVisu_SelectionNature& asiVisu_NotUsed(selNature)) const
{
  // Do nothing...
}

//! Callback for rendering.
void asiVisu_RENormalsPrs::renderPipelines(vtkRenderer* asiVisu_NotUsed(renderer)) const
{
  // Do nothing...
}

//! Callback for de-rendering.
void asiVisu_RENormalsPrs::deRenderPipelines(vtkRenderer* asiVisu_NotUsed(renderer)) const
{
  // Do nothing...
}
