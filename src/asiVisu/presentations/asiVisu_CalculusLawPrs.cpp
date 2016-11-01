//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_CalculusLawPrs.h>

// A-Situs (visualization) includes
#include <asiVisu_LawDataProvider.h>
#include <asiVisu_LawPipeline.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//! Creates a Presentation object.
//! \param N [in] Data Node to create a Presentation for.
asiVisu_CalculusLawPrs::asiVisu_CalculusLawPrs(const Handle(ActAPI_INode)& N)
: asiVisu_Prs(N)
{
  // Pipeline for law
  this->addPipeline        ( Pipeline_Law, new asiVisu_LawPipeline() );
  this->assignDataProvider ( Pipeline_Law, new asiVisu_LawDataProvider( Handle(asiData_DesignLawNode)::DownCast(N) ) );
}

//! Factory method for Presentation.
//! \param N [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_CalculusLawPrs::Instance(const Handle(ActAPI_INode)& N)
{
  return new asiVisu_CalculusLawPrs(N);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool asiVisu_CalculusLawPrs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_CalculusLawPrs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void asiVisu_CalculusLawPrs::afterInitPipelines()
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void asiVisu_CalculusLawPrs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void asiVisu_CalculusLawPrs::afterUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for highlighting.
//! \param theRenderer  [in] renderer.
//! \param thePickRes   [in] picking results.
//! \param theSelNature [in] selection nature (picking or detecting).
void asiVisu_CalculusLawPrs::highlight(vtkRenderer*                 asiVisu_NotUsed(theRenderer),
                                      const asiUI_PickResult&      asiVisu_NotUsed(thePickRes),
                                      const asiUI_SelectionNature& asiVisu_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for highlighting reset.
//! \param theRenderer [in] renderer.
void asiVisu_CalculusLawPrs::unHighlight(vtkRenderer*                 asiVisu_NotUsed(theRenderer),
                                        const asiUI_SelectionNature& asiVisu_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void asiVisu_CalculusLawPrs::renderPipelines(vtkRenderer* asiVisu_NotUsed(theRenderer)) const
{
  // Do nothing...
}

//! Callback for de-rendering.
//! \param theRenderer [in] renderer.
void asiVisu_CalculusLawPrs::deRenderPipelines(vtkRenderer* asiVisu_NotUsed(theRenderer)) const
{
  // Do nothing...
}
