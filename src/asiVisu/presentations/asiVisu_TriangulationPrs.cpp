//-----------------------------------------------------------------------------
// Created on: 11 July 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_TriangulationPrs.h>

// asiVisu includes
#include <asiVisu_MeshDisplayMode.h>
#include <asiVisu_TriangulationDataProvider.h>
#include <asiVisu_TriangulationLinksPipeline.h>
#include <asiVisu_TriangulationPipeline.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkActor.h>
#include <vtkCellTreeLocator.h>
#include <vtkMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

//! Creates a Presentation object for the passed Triangulation Node.
//! \param N [in] Triangulation Node to create a Presentation for.
asiVisu_TriangulationPrs::asiVisu_TriangulationPrs(const Handle(ActAPI_INode)& N) : asiVisu_Prs(N)
{
  Handle(asiData_TriangulationNode)
    triangulationNode = Handle(asiData_TriangulationNode)::DownCast(N);

  // Create Data Provider
  Handle(asiVisu_TriangulationDataProvider)
    dp = new asiVisu_TriangulationDataProvider(triangulationNode);

  // Main pipeline
  Handle(asiVisu_TriangulationPipeline) pl = new asiVisu_TriangulationPipeline();
  //
  this->addPipeline        ( Pipeline_Triangulation, pl );
  this->assignDataProvider ( Pipeline_Triangulation, dp );

  // Set point size and line width
  pl->Actor()->GetProperty()->SetPointSize(5.0f);
  pl->Actor()->GetProperty()->SetLineWidth(1.5f);

  /* =========================
   *  Pipeline for mesh links
   * ========================= */

  // Create pipeline for links
  Handle(asiVisu_TriangulationLinksPipeline)
    contour_pl = new asiVisu_TriangulationLinksPipeline( pl->GetSource() );

  // Adjust props
  contour_pl->Actor()->GetProperty()->SetOpacity(0.5);
  contour_pl->Actor()->GetProperty()->SetLineWidth(1.0f);
  contour_pl->Actor()->SetPickable(0);
  //
  this->addPipeline        ( Pipeline_TriangulationLinks, contour_pl );
  this->assignDataProvider ( Pipeline_TriangulationLinks, dp );

  // Resolve coincident topology between shaded facets and border links
  vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();
}

//-----------------------------------------------------------------------------

//! Factory method for Node's Presentation.
//! \param theNode [in] Mesh Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_TriangulationPrs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new asiVisu_TriangulationPrs(theNode);
}

//-----------------------------------------------------------------------------

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool asiVisu_TriangulationPrs::IsVisible() const
{
  return true; // TODO: make visibility controllable
}

//-----------------------------------------------------------------------------

//! Sets diagnostic tools for the presentation.
//! \param progress [in] progress notifier.
//! \param plotter  [in] imperative plotter.
void asiVisu_TriangulationPrs::SetDiagnosticTools(ActAPI_ProgressEntry progress,
                                                  ActAPI_PlotterEntry  plotter)
{
  asiVisu_Prs::SetDiagnosticTools(progress, plotter);

  Handle(asiVisu_TriangulationPipeline)
    pl = Handle(asiVisu_TriangulationPipeline)::DownCast( this->GetPipeline(Pipeline_Triangulation) );

  if ( pl.IsNull() )
    return;

  pl->SetDiagnosticTools(m_progress, m_plotter);
}

//-----------------------------------------------------------------------------

//! Sets custom color for the geometry.
//! \param color [in] color to set.
void asiVisu_TriangulationPrs::doColor(const QColor& color) const
{
  if ( !color.isValid() )
    return;

  Handle(asiVisu_TriangulationPipeline)
    pl = Handle(asiVisu_TriangulationPipeline)::DownCast( this->GetPipeline(Pipeline_Triangulation) );

  if ( pl.IsNull() )
    return;

  pl->Mapper()->ScalarVisibilityOff();
  pl->Actor()->GetProperty()->SetColor( color.redF(),
                                        color.greenF(),
                                        color.blueF() );
}

//-----------------------------------------------------------------------------

//! Unsets custom color for the geometry.
void asiVisu_TriangulationPrs::doUnColor() const
{
  Handle(asiVisu_TriangulationPipeline)
    pl = Handle(asiVisu_TriangulationPipeline)::DownCast( this->GetPipeline(Pipeline_Triangulation) );

  if ( pl.IsNull() )
    return;

  pl->Mapper()->ScalarVisibilityOn();
}

//-----------------------------------------------------------------------------

void asiVisu_TriangulationPrs::InitializePicker(const vtkSmartPointer<vtkCellPicker>& picker) const
{
  picker->RemoveAllLocators();

  // Set octee locators to speed up cell picking
  if ( this->MainActor() )
  {
    vtkSmartPointer<vtkCellTreeLocator>
      facetLocator = vtkSmartPointer<vtkCellTreeLocator>::New();
    //
    facetLocator->SetDataSet( this->MainActor()->GetMapper()->GetInput() );
    facetLocator->AutomaticOn();
    facetLocator->BuildLocator();
    //
    picker->AddLocator(facetLocator);
  }

  // NOTICE: we do not apply cell locator for selection of edges as it seems
  //         to work weird (at least for vtkCellTreeLocator)
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_TriangulationPrs::beforeInitPipelines()
{
  // Do nothing...
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_TriangulationPrs::afterInitPipelines()
{
  // Do nothing...
}

//-----------------------------------------------------------------------------

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void asiVisu_TriangulationPrs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//-----------------------------------------------------------------------------

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void asiVisu_TriangulationPrs::afterUpdatePipelines() const
{
  // Do nothing...
}

//-----------------------------------------------------------------------------

//! Callback for highlighting.
//! \param renderer  [in] renderer.
//! \param pickRes   [in] picking results.
//! \param selNature [in] selection nature (picking or detecting).
void asiVisu_TriangulationPrs::highlight(vtkRenderer*                  renderer,
                                         const asiVisu_PickResult&     pickRes,
                                         const asiVisu_SelectionNature selNature) const
{
  asiVisu_NotUsed(renderer);

  // #################################################
  // FACET selection
  if ( pickRes.GetPickedActor() == this->MainActor() )
  {
#if defined COUT_DEBUG
    std::cout << "Picked MAIN actor" << std::endl;
#endif

    Handle(asiVisu_TriangulationPipeline)
      mainPl = Handle(asiVisu_TriangulationPipeline)::DownCast( this->GetPipeline(Pipeline_Triangulation) );

    mainPl->SetPickedElements( pickRes.GetPickedElementIds(), selNature );
  }
  // #################################################
  // LINK selection
  else if ( pickRes.GetPickedActor() == this->ContourActor() )
  {
#if defined COUT_DEBUG
    std::cout << "Picked CONTOUR actor" << std::endl;
#endif

    Handle(asiVisu_TriangulationLinksPipeline)
      contourPl = Handle(asiVisu_TriangulationLinksPipeline)::DownCast( this->GetPipeline(Pipeline_TriangulationLinks) );

    contourPl->SetPickedElements( pickRes.GetPickedElementIds(), selNature );
  }
}

//-----------------------------------------------------------------------------

//! Callback for highlighting reset.
//! \param renderer  [in] renderer.
//! \param selNature [in] selection nature (picking or detecting).
void asiVisu_TriangulationPrs::unHighlight(vtkRenderer*                  renderer,
                                           const asiVisu_SelectionNature selNature) const
{
  asiVisu_NotUsed(renderer);

  Handle(asiVisu_TriangulationPipeline)
    mainPl = Handle(asiVisu_TriangulationPipeline)::DownCast( this->GetPipeline(Pipeline_Triangulation) );
  //
  Handle(asiVisu_TriangulationLinksPipeline)
    contourPl = Handle(asiVisu_TriangulationLinksPipeline)::DownCast( this->GetPipeline(Pipeline_TriangulationLinks) );

  mainPl->ResetPickedElements(selNature);
  contourPl->ResetPickedElements(selNature);
}

//-----------------------------------------------------------------------------

//! Callback for rendering.
//! \param renderer [in] renderer.
void asiVisu_TriangulationPrs::renderPipelines(vtkRenderer* renderer) const
{
  asiVisu_NotUsed(renderer);

  // Do nothing...
}

//-----------------------------------------------------------------------------

//! Callback for de-rendering.
//! \param renderer [in] renderer.
void asiVisu_TriangulationPrs::deRenderPipelines(vtkRenderer* renderer) const
{
  asiVisu_NotUsed(renderer);

  // Do nothing...
}
