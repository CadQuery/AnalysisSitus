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
#include <asiVisu_IVCurvePrs.h>

// asiVisu includes
#include <asiVisu_BCurveHandlesPipeline.h>
#include <asiVisu_BCurveKnotsPipeline.h>
#include <asiVisu_BCurvePolesPipeline.h>
#include <asiVisu_BCurveRepersPipeline.h>
#include <asiVisu_CurvePipeline.h>
#include <asiVisu_IVCurveDataProvider.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

asiVisu_IVCurvePrs::asiVisu_IVCurvePrs(const Handle(ActAPI_INode)& N)
: asiVisu_DefaultPrs(N)
{
  // Create Data Provider.
  Handle(asiVisu_IVCurveDataProvider) DP     = new asiVisu_IVCurveDataProvider(N);
  Handle(asiVisu_IVCurveDataProvider) DPsens = new asiVisu_IVCurveDataSensProvider(N);

  // Pipeline for curve.
  this->addPipeline        ( PrimaryPipeline_Main, new asiVisu_CurvePipeline );
  this->assignDataProvider ( PrimaryPipeline_Main, DP );

  // Pipeline for poles of b-curves.
  this->addPipeline        ( PrimaryPipeline_Poles, new asiVisu_BCurvePolesPipeline );
  this->assignDataProvider ( PrimaryPipeline_Poles, DP );

  // Pipeline for knots of b-curves.
  this->addPipeline        ( PrimaryPipeline_Knots, new asiVisu_BCurveKnotsPipeline );
  this->assignDataProvider ( PrimaryPipeline_Knots, DP );

  // Pipeline for handles of b-curves.
  this->addPipeline        ( PrimaryPipeline_Handles, new asiVisu_BCurveHandlesPipeline(false) );
  this->assignDataProvider ( PrimaryPipeline_Handles, DP );

  // Pipeline for reper points.
  this->addPipeline        ( PrimaryPipeline_Repers, new asiVisu_BCurveRepersPipeline(false) );
  this->assignDataProvider ( PrimaryPipeline_Repers, DP );

  // Pipeline for detection of handles.
  {
    Handle(asiVisu_BCurveHandlesPipeline)
      plDetection = new asiVisu_BCurveHandlesPipeline(true); // `true` for partial visualization.
    //
    plDetection->Actor()->GetProperty()->SetColor(0.0, 1.0, 1.0);
    plDetection->Actor()->GetProperty()->SetPointSize(10.0);
    //
    this->installDetectionPipeline(plDetection, DPsens, SelectionPipeline_Handles);
  }

  // Pipeline for detection of reper points.
  {
    Handle(asiVisu_BCurveRepersPipeline)
      plDetection = new asiVisu_BCurveRepersPipeline(true); // `true` for partial visualization.
    //
    plDetection->Actor()->GetProperty()->SetColor(0.0, 1.0, 1.0);
    plDetection->Actor()->GetProperty()->SetPointSize(10.0);
    //
    this->installDetectionPipeline(plDetection, DPsens, SelectionPipeline_Repers);
  }

  // Pipeline for persistent picking of handles.
  {
    Handle(asiVisu_BCurveHandlesPipeline)
      plPicking = new asiVisu_BCurveHandlesPipeline(true); // `true` for partial visualization.
    //
    plPicking->Actor()->GetProperty()->SetColor(1.0, 1.0, 0.0);
    plPicking->Actor()->GetProperty()->SetPointSize(10.0);
    //
    this->installSelectionPipeline(plPicking, DPsens, SelectionPipeline_Handles);
  }

  // Pipeline for persistent picking of repers.
  {
    Handle(asiVisu_BCurveRepersPipeline)
      plPicking = new asiVisu_BCurveRepersPipeline(true); // `true` for partial visualization.
    //
    plPicking->Actor()->GetProperty()->SetColor(1.0, 1.0, 0.0);
    plPicking->Actor()->GetProperty()->SetPointSize(10.0);
    //
    this->installSelectionPipeline(plPicking, DPsens, SelectionPipeline_Repers);
  }

  // Adjust props.
  this->GetPipeline(PrimaryPipeline_Main)    ->Actor()->GetProperty()->SetLineWidth(1.0f);
  this->GetPipeline(PrimaryPipeline_Poles)   ->Actor()->GetProperty()->SetColor(0.6, 0.6, 0.6);
  this->GetPipeline(PrimaryPipeline_Knots)   ->Actor()->GetProperty()->SetColor(0.0, 1.0, 0.0);
  this->GetPipeline(PrimaryPipeline_Handles) ->Actor()->GetProperty()->SetColor(1.0, 0.0, 0.0);
  this->GetPipeline(PrimaryPipeline_Repers)  ->Actor()->GetProperty()->SetColor(1.0, 0.0, 1.0);
  //
  this->GetPipeline(PrimaryPipeline_Main)    ->Actor()->SetPickable(0);
  this->GetPipeline(PrimaryPipeline_Poles)   ->Actor()->SetPickable(0);
  this->GetPipeline(PrimaryPipeline_Knots)   ->Actor()->SetPickable(0);
  this->GetPipeline(PrimaryPipeline_Handles) ->Actor()->SetPickable(0);
  this->GetPipeline(PrimaryPipeline_Repers)  ->Actor()->SetPickable(1);
  //
  this->GetPipeline(PrimaryPipeline_Poles)   ->Actor()->SetVisibility(0);
  this->GetPipeline(PrimaryPipeline_Knots)   ->Actor()->SetVisibility(0);
  this->GetPipeline(PrimaryPipeline_Handles) ->Actor()->SetVisibility(0);
  this->GetPipeline(PrimaryPipeline_Repers)  ->Actor()->SetVisibility(1);
}

//-----------------------------------------------------------------------------

Handle(asiVisu_Prs) asiVisu_IVCurvePrs::Instance(const Handle(ActAPI_INode)& N)
{
  return new asiVisu_IVCurvePrs(N);
}

//-----------------------------------------------------------------------------

void asiVisu_IVCurvePrs::highlight(vtkRenderer*                        renderer,
                                   const Handle(asiVisu_PickerResult)& pickRes,
                                   const asiVisu_SelectionNature       selNature) const
{
  asiVisu_NotUsed(renderer);

  // Check picked actor.
  vtkActor* pPickedActor = pickRes->GetPickedActor();
  //
  if ( pPickedActor != this->GetPipeline(PrimaryPipeline_Handles)->Actor() &&
       pPickedActor != this->GetPipeline(PrimaryPipeline_Repers)->Actor() )
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

      // Handles.
      if ( pl->IsKind( STANDARD_TYPE(asiVisu_BCurveHandlesPipeline) ) )
      {
        Handle(asiVisu_BCurveHandlesPipeline)
          castedPl = Handle(asiVisu_BCurveHandlesPipeline)::DownCast(pl);
        //
        castedPl->SetForcedActiveHandle( cellRes->GetPickedElementIds().GetMinimalMapped() );
      }

      // Repers.
      else if ( pl->IsKind( STANDARD_TYPE(asiVisu_BCurveRepersPipeline) ) )
      {
        Handle(asiVisu_BCurveRepersPipeline)
          castedPl = Handle(asiVisu_BCurveRepersPipeline)::DownCast(pl);
        //
        castedPl->SetForcedActiveReper( cellRes->GetPickedElementIds().GetMinimalMapped() );
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

      // Update.
      pl->Actor()->SetVisibility(1);
      pl->SetInput( this->dataProviderSelection(plIdx++) );
      pl->Update();
    }
  }
}

//-----------------------------------------------------------------------------

void asiVisu_IVCurvePrs::unHighlight(vtkRenderer*                  renderer,
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

void asiVisu_IVCurvePrs::renderPipelines(vtkRenderer* renderer) const
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
}

//-----------------------------------------------------------------------------

void asiVisu_IVCurvePrs::deRenderPipelines(vtkRenderer* renderer) const
{
  /* Take care of selection pipelines */

  Handle(asiVisu_HPipelineList) detectPls = this->GetDetectionPipelineList();
  Handle(asiVisu_HPipelineList) pickPls   = this->GetSelectionPipelineList();

  for ( asiVisu_HPipelineList::Iterator it(*detectPls); it.More(); it.Next() )
    it.Value()->RemoveFromRenderer(renderer);
  //
  for ( asiVisu_HPipelineList::Iterator it(*pickPls); it.More(); it.Next() )
    it.Value()->RemoveFromRenderer(renderer);
}
