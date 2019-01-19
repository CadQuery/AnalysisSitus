//-----------------------------------------------------------------------------
// Created on: 11 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2016-present, Sergey Slyadnev
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
//    * Neither the name of Sergey Slyadnev nor the
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
#include <asiVisu_IVPointSetPrs.h>

// asiVisu includes
#include <asiVisu_IVPointSetDataProvider.h>
#include <asiVisu_PointsPipeline.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>

//-----------------------------------------------------------------------------

asiVisu_IVPointSetPrs::asiVisu_IVPointSetPrs(const Handle(ActAPI_INode)& N)
: asiVisu_DefaultPrs(N)
{
  // Create Data Provider.
  Handle(asiVisu_IVPointSetDataProvider) DP = new asiVisu_IVPointSetDataProvider(N);

  // Pipeline for points.
  {
    this->addPipeline        ( PrimaryPipeline_Main, new asiVisu_PointsPipeline(false) );
    this->assignDataProvider ( PrimaryPipeline_Main, DP );
  }

  // Pipeline for detecting points.
  {
    Handle(asiVisu_PointsPipeline)
      plDetection = new asiVisu_PointsPipeline(true); // `true` for partial visualization.
    //
    plDetection->Actor()->GetProperty()->SetColor(0.0, 1.0, 1.0);
    plDetection->Actor()->GetProperty()->SetPointSize(10.0);
    //
    this->installDetectionPipeline(plDetection, DP, SelectionPipeline_Main);
  }

  // Pipeline for selecting points.
  {
    Handle(asiVisu_PointsPipeline)
      plSelection = new asiVisu_PointsPipeline(true); // `true` for partial visualization.
    //
    plSelection->Actor()->GetProperty()->SetColor(1.0, 1.0, 0.0);
    plSelection->Actor()->GetProperty()->SetPointSize(10.0);
    //
    this->installSelectionPipeline(plSelection, DP, SelectionPipeline_Main);
  }

  // Adjust visual properties.
  this->GetPipeline(PrimaryPipeline_Main)->Actor()->GetProperty()->SetPointSize(4.0);
  this->GetPipeline(PrimaryPipeline_Main)->Actor()->GetProperty()->RenderPointsAsSpheresOn();
  this->GetPipeline(PrimaryPipeline_Main)->Actor()->SetPickable(1);

  // Initialize text widget used for annotation.
  m_textWidget = vtkSmartPointer<vtkTextWidget>::New();
  asiVisu_Utils::InitTextWidget(m_textWidget);
}

//-----------------------------------------------------------------------------

Handle(asiVisu_Prs)
  asiVisu_IVPointSetPrs::Instance(const Handle(ActAPI_INode)& N)
{
  return new asiVisu_IVPointSetPrs(N);
}

//-----------------------------------------------------------------------------

void asiVisu_IVPointSetPrs::highlight(vtkRenderer*                        renderer,
                                      const Handle(asiVisu_PickerResult)& pickRes,
                                      const asiVisu_SelectionNature       selNature) const
{
  asiVisu_NotUsed(renderer);

  // Check picked actor.
  vtkActor* pPickedActor = pickRes->GetPickedActor();
  //
  if ( pPickedActor != this->GetPipeline(PrimaryPipeline_Main)->Actor() )
    return;

  // Only cells are processed.
  Handle(asiVisu_CellPickerResult)
    cellRes = Handle(asiVisu_CellPickerResult)::DownCast(pickRes);
  //
  if ( cellRes.IsNull() )
    return;

  if ( selNature == SelectionNature_Detection )
  {
    Handle(asiVisu_HPipelineList) pls = this->GetDetectionPipelineList();

    // Activate detection pipelines.
    for ( asiVisu_HPipelineList::Iterator it(*pls); it.More(); it.Next() )
    {
      const Handle(asiVisu_Pipeline)& pl = it.Value();

      // Check pipeline type.
      if ( pl->IsKind( STANDARD_TYPE(asiVisu_PointsPipeline) ) )
      {
        Handle(asiVisu_PointsPipeline)
          castedPl = Handle(asiVisu_PointsPipeline)::DownCast(pl);
        //
        castedPl->SetForcedActiveRepers( new TColStd_HPackedMapOfInteger( cellRes->GetPickedCellIds() ) );
      }

      // Update.
      pl->Actor()->SetVisibility(1);
      pl->SetInput( this->dataProviderDetection() );
      pl->Update();
    }
  }
  else if ( selNature == SelectionNature_Persistent )
  {
    Handle(asiVisu_HPipelineList) pls = this->GetSelectionPipelineList();

    // Activate picking pipelines.
    int plIdx = 1;
    for ( asiVisu_HPipelineList::Iterator it(*pls); it.More(); it.Next() )
    {
      const Handle(asiVisu_Pipeline)& pl = it.Value();

      // Get indices of the picked cells.
      Handle(TColStd_HPackedMapOfInteger)
        pickedIds = new TColStd_HPackedMapOfInteger( cellRes->GetPickedCellIds() );

      // Update Data Model.
      m_model->OpenCommand();
      {
        Handle(asiData_IVPointSetNode)
          pointsNode = Handle(asiData_IVPointSetNode)::DownCast(m_node);

        pointsNode->SetFilter(pickedIds);
      }
      m_model->CommitCommand();

      // Update actor.
      pl->Actor()->SetVisibility(1);
      pl->SetInput( this->dataProviderSelection(plIdx++) );
      pl->Update();
    }
  }
}

//-----------------------------------------------------------------------------

void asiVisu_IVPointSetPrs::unHighlight(vtkRenderer*                  renderer,
                                        const asiVisu_SelectionNature selNature) const
{
  asiVisu_NotUsed(renderer);

  if ( selNature == SelectionNature_Detection )
  {
    Handle(asiVisu_HPipelineList) pls = this->GetDetectionPipelineList();

    // Deactivate detection pipelines.
    for ( asiVisu_HPipelineList::Iterator it(*pls); it.More(); it.Next() )
      it.Value()->Actor()->SetVisibility(0);
  }
  else if ( selNature == SelectionNature_Persistent )
  {
    Handle(asiVisu_HPipelineList) pls = this->GetSelectionPipelineList();

    // Deactivate picking pipelines.
    for ( asiVisu_HPipelineList::Iterator it(*pls); it.More(); it.Next() )
      it.Value()->Actor()->SetVisibility(0);
  }
}

//-----------------------------------------------------------------------------

void asiVisu_IVPointSetPrs::afterInitPipelines()
{
  Handle(asiVisu_IVPointSetDataProvider)
    DP = Handle(asiVisu_IVPointSetDataProvider)::DownCast( this->dataProvider(PrimaryPipeline_Main) );

  const int nPts = DP->GetPoints()->GetNumberOfElements();

  if ( nPts > 1 )
  {
    TCollection_AsciiString title("Num. of points: "); title += nPts;

    // Update text on the annotation.
    m_textWidget->GetTextActor()->SetInput( title.ToCString() );
  }
}

//-----------------------------------------------------------------------------

void asiVisu_IVPointSetPrs::renderPipelines(vtkRenderer* renderer) const
{
  /* Take care of selection pipelines */

  Handle(asiVisu_HPipelineList) detectPls = this->GetDetectionPipelineList();
  Handle(asiVisu_HPipelineList) pickPls   = this->GetSelectionPipelineList();

  // Picking pipeline must be added to renderer the LAST (!). Otherwise
  // we can experience some strange coloring bug because of their coincidence.
  for ( asiVisu_HPipelineList::Iterator it(*detectPls); it.More(); it.Next() )
    it.Value()->AddToRenderer(renderer);
  //
  for ( asiVisu_HPipelineList::Iterator it(*pickPls); it.More(); it.Next() )
    it.Value()->AddToRenderer(renderer);

  /* Take care of annotation widget */

  Handle(asiVisu_IVPointSetDataProvider)
    DP = Handle(asiVisu_IVPointSetDataProvider)::DownCast( this->dataProvider(PrimaryPipeline_Main) );

  if ( DP->GetPoints()->GetNumberOfElements() > 1 )
  {
    if ( !m_textWidget->GetCurrentRenderer() )
    {
      m_textWidget->SetInteractor      ( renderer->GetRenderWindow()->GetInteractor() );
      m_textWidget->SetDefaultRenderer ( renderer );
      m_textWidget->SetCurrentRenderer ( renderer );
      m_textWidget->On                 ( );
      m_textWidget->ReleaseFocus       ( );
    }
  }
}

//-----------------------------------------------------------------------------

void asiVisu_IVPointSetPrs::deRenderPipelines(vtkRenderer* renderer) const
{
  /* Take care of selection pipelines */

  Handle(asiVisu_HPipelineList) detectPls = this->GetDetectionPipelineList();
  Handle(asiVisu_HPipelineList) pickPls   = this->GetSelectionPipelineList();

  for ( asiVisu_HPipelineList::Iterator it(*detectPls); it.More(); it.Next() )
    it.Value()->RemoveFromRenderer(renderer);
  //
  for ( asiVisu_HPipelineList::Iterator it(*pickPls); it.More(); it.Next() )
    it.Value()->RemoveFromRenderer(renderer);

  /* Take care of annotation widget */

  m_textWidget->Off();
}
