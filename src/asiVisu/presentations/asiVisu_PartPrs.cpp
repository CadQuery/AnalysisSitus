//-----------------------------------------------------------------------------
// Created on: 28 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_PartPrs.h>

// asiVisu includes
#include <asiVisu_PartDataProvider.h>
#include <asiVisu_PartEdgesPipeline.h>
#include <asiVisu_PartPipeline.h>
#include <asiVisu_ShapeDisplayMode.h>

// VTK includes
#include <vtkCellData.h>
#include <vtkCellTreeLocator.h>
#include <vtkIdTypeArray.h>
#include <vtkMapper.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

//! Creates a Presentation object for the passed Part Node.
//! \param[in] N Part Node to create a Presentation for.
asiVisu_PartPrs::asiVisu_PartPrs(const Handle(ActAPI_INode)& N) : asiVisu_Prs(N)
{
  Handle(asiData_PartNode) partNode = Handle(asiData_PartNode)::DownCast(N);

  /* ====================================
   *  Pipeline for shaded representation.
   * ==================================== */

  // Create Data Provider.
  Handle(asiVisu_PartDataProvider) dp = new asiVisu_PartDataProvider(partNode);

  // Main pipeline.
  Handle(asiVisu_PartPipeline) pl = new asiVisu_PartPipeline();
  //
  this->addPipeline        ( Pipeline_Main, pl );
  this->assignDataProvider ( Pipeline_Main, dp );

  // Adjust properties.
  pl->Actor()->GetProperty()->SetPointSize(5.0f);
  pl->Actor()->GetProperty()->SetLineWidth(1.0f);
  pl->Actor()->GetProperty()->BackfaceCullingOn();
  //
  pl->Mapper()->SetResolveCoincidentTopologyToPolygonOffset();

  /* =================================
   *  Pipeline for back side of shape.
   * ================================= */

  // Create pipeline for the backside.
  Handle(asiVisu_PartPipeline)
    backside_pl = new asiVisu_PartPipeline( pl->GetSource() );
  //
  this->addPipeline        ( Pipeline_Backside, backside_pl );
  this->assignDataProvider ( Pipeline_Backside, dp );

  // Adjust properties.
  backside_pl->SetScalarsOn(false);
  backside_pl->Actor()->GetProperty()->FrontfaceCullingOn();
  backside_pl->Actor()->SetPickable(0);
  backside_pl->Actor()->GetProperty()->SetColor(0.25, 0.25, 0.25);
}

//-----------------------------------------------------------------------------

//! Factory method for Presentation.
//! \param[in] N Part Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_PartPrs::Instance(const Handle(ActAPI_INode)& N)
{
  return new asiVisu_PartPrs(N);
}

//-----------------------------------------------------------------------------

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool asiVisu_PartPrs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Sets diagnostic tools for the presentation.
//! \param[in] progress progress notifier.
//! \param[in] plotter  imperative plotter.
void asiVisu_PartPrs::SetDiagnosticTools(ActAPI_ProgressEntry progress,
                                         ActAPI_PlotterEntry  plotter)
{
  asiVisu_Prs::SetDiagnosticTools(progress, plotter);

  Handle(asiVisu_PartPipeline)
    pl = Handle(asiVisu_PartPipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

  if ( pl.IsNull() )
    return;

  pl->SetDiagnosticTools(m_progress, m_plotter);
}

//-----------------------------------------------------------------------------

//! Enables visualization of vertices.
void asiVisu_PartPrs::VerticesOn() const
{
  Handle(asiVisu_PartPipeline)
    pl = Handle(asiVisu_PartPipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

  if ( pl.IsNull() )
    return;

  // Configure display mode
  //pl->GetDisplayModeFilter()->SetDisplayMode(ShapeDisplayMode_WireframeAndVertices);
}

//-----------------------------------------------------------------------------

//! Disables visualization of vertices.
void asiVisu_PartPrs::VerticesOff() const
{
  Handle(asiVisu_PartPipeline)
    pl = Handle(asiVisu_PartPipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

  if ( pl.IsNull() )
    return;

  // Configure display mode
  //pl->GetDisplayModeFilter()->SetDisplayMode(ShapeDisplayMode_Wireframe);
}

//-----------------------------------------------------------------------------

//! Sets custom color.
//! \param[in] color color to set.
void asiVisu_PartPrs::Colorize(const QColor& color) const
{
  if ( !color.isValid() )
    return;

  Handle(asiVisu_PartPipeline)
    pl = Handle(asiVisu_PartPipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

  if ( !pl.IsNull() )
    pl->Actor()->GetProperty()->SetColor( color.redF(),
                                          color.greenF(),
                                          color.blueF() );
}

//-----------------------------------------------------------------------------

//! Enables shaded mode for the main pipeline.
void asiVisu_PartPrs::ShadingOn() const
{
  Handle(asiVisu_PartPipeline)
    plMain = Handle(asiVisu_PartPipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

  if ( plMain.IsNull() )
    return;

  // Configure display mode.
  plMain->GetDisplayModeFilter()->SetDisplayMode(ShapeDisplayMode_Shaded);
  //
  plMain->Actor()->SetPickable(1);
}
//-----------------------------------------------------------------------------

//! Enables wireframe mode for the main pipeline.
void asiVisu_PartPrs::WireframeOn() const
{
  Handle(asiVisu_PartPipeline)
    plMain = Handle(asiVisu_PartPipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

  if ( plMain.IsNull() )
    return;

  // Configure display mode.
  plMain->GetDisplayModeFilter()->SetDisplayMode(ShapeDisplayMode_WireframeAndVertices);
  //
  plMain->Actor()->SetPickable(0);
}

//-----------------------------------------------------------------------------

void asiVisu_PartPrs::InitializePicker(const vtkSmartPointer<vtkCellPicker>& asiVisu_NotUsed(renderer)) const
{
  //picker->RemoveAllLocators();

  //// Set octee locators to speed up cell picking
  //if ( this->MainActor() )
  //{
  //  vtkSmartPointer<vtkCellTreeLocator>
  //    facetLocator = vtkSmartPointer<vtkCellTreeLocator>::New();
  //  //
  //  facetLocator->SetDataSet( this->MainActor()->GetMapper()->GetInput() );
  //  facetLocator->AutomaticOn();
  //  facetLocator->BuildLocator();
  //  //
  //  picker->AddLocator(facetLocator);
  //}

  // NOTICE: we do not apply cell locator for selection of edges as it seems
  //         to work weird (at least for vtkCellTreeLocator)
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_PartPrs::beforeInitPipelines()
{
  // Do nothing...
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_PartPrs::afterInitPipelines()
{
  // Do nothing...
}

//-----------------------------------------------------------------------------

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void asiVisu_PartPrs::beforeUpdatePipelines() const
{
  Handle(asiData_PartNode)
    N = Handle(asiData_PartNode)::DownCast( this->GetNode() );

  /* Actualize display mode */

  const int dm = N->GetDisplayMode();
  //
  if ( dm & ShapeDisplayMode_Shaded )
    this->ShadingOn();
  else if ( dm & ShapeDisplayMode_WireframeAndVertices )
    this->WireframeOn();
}

//-----------------------------------------------------------------------------

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void asiVisu_PartPrs::afterUpdatePipelines() const
{
  Handle(asiData_PartNode)
    N = Handle(asiData_PartNode)::DownCast( this->GetNode() );

  /* Actualize color */

  if ( N->HasColor() )
  {
    QColor color = asiVisu_Utils::IntToColor( N->GetColor() );
    this->Colorize(color);
  }
  else
    this->Colorize(Qt::white);

  /* Actualize visualization of vertices */

  if ( N->HasVertices() )
    this->VerticesOn();
  else
    this->VerticesOff();
}

//-----------------------------------------------------------------------------

//! Callback for highlighting.
//! \param[in] renderer  renderer.
//! \param[in] pickRes   picking results.
//! \param[in] selNature selection nature (picking or detecting).
void asiVisu_PartPrs::highlight(vtkRenderer*                        asiVisu_NotUsed(renderer),
                                const Handle(asiVisu_PickerResult)& pickRes,
                                const asiVisu_SelectionNature       selNature) const
{
  // Can react on cell picking only.
  Handle(asiVisu_CellPickerResult)
    cellPickRes = Handle(asiVisu_CellPickerResult)::DownCast(pickRes);
  //
  if ( cellPickRes.IsNull() )
    return;

  if ( cellPickRes->GetPickedActor() == this->MainActor() )
  {
#if defined COUT_DEBUG
    std::cout << "Picked MAIN actor" << std::endl;
#endif

    Handle(asiVisu_PartPipeline)
      mainPl = Handle(asiVisu_PartPipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

    mainPl->SetPickedElements(cellPickRes->GetPickedElementIds(), selNature);
  }
}

//-----------------------------------------------------------------------------

//! Callback for highlighting reset.
//! \param[in] renderer  renderer.
//! \param[in] selNature selection nature (picking or detecting).
void asiVisu_PartPrs::unHighlight(vtkRenderer*                  asiVisu_NotUsed(renderer),
                                  const asiVisu_SelectionNature selNature) const
{
  Handle(asiVisu_PartPipeline)
    mainPl = Handle(asiVisu_PartPipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

  mainPl->ResetPickedElements(selNature);
}

//-----------------------------------------------------------------------------

//! Callback for rendering.
//! \param[in] renderer renderer.
void asiVisu_PartPrs::renderPipelines(vtkRenderer* asiVisu_NotUsed(renderer)) const
{
  // Do nothing...
}

//-----------------------------------------------------------------------------

//! Callback for de-rendering.
//! \param[in] renderer renderer.
void asiVisu_PartPrs::deRenderPipelines(vtkRenderer* asiVisu_NotUsed(renderer)) const
{
  // Do nothing...
}
