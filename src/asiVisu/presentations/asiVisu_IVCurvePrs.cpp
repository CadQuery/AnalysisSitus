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
  Handle(asiVisu_IVCurveDataProvider) DP = new asiVisu_IVCurveDataProvider(N);

  // Pipeline for curve.
  this->addPipeline        ( Pipeline_Main, new asiVisu_CurvePipeline );
  this->assignDataProvider ( Pipeline_Main, DP );

  // Pipeline for poles of b-curves.
  this->addPipeline        ( Pipeline_Poles, new asiVisu_BCurvePolesPipeline );
  this->assignDataProvider ( Pipeline_Poles, DP );

  // Pipeline for knots of b-curves.
  this->addPipeline        ( Pipeline_Knots, new asiVisu_BCurveKnotsPipeline );
  this->assignDataProvider ( Pipeline_Knots, DP );

  // Pipeline for handles of b-curves.
  this->addPipeline        ( Pipeline_Handles, new asiVisu_BCurveHandlesPipeline );
  this->assignDataProvider ( Pipeline_Handles, DP );

  // Adjust props.
  this->GetPipeline(Pipeline_Main)    ->Actor()->GetProperty()->SetLineWidth(1.0f);
  this->GetPipeline(Pipeline_Poles)   ->Actor()->GetProperty()->SetColor(0.6, 0.6, 0.6);
  this->GetPipeline(Pipeline_Knots)   ->Actor()->GetProperty()->SetColor(0.0, 1.0, 0.0);
  this->GetPipeline(Pipeline_Handles) ->Actor()->GetProperty()->SetColor(1.0, 0.0, 0.0);
  //
  this->GetPipeline(Pipeline_Main)    ->Actor()->SetPickable(0);
  this->GetPipeline(Pipeline_Poles)   ->Actor()->SetPickable(0);
  this->GetPipeline(Pipeline_Knots)   ->Actor()->SetPickable(0);
  this->GetPipeline(Pipeline_Handles) ->Actor()->SetPickable(0);
  //
  this->GetPipeline(Pipeline_Poles)   ->Actor()->SetVisibility(0);
  this->GetPipeline(Pipeline_Knots)   ->Actor()->SetVisibility(0);
  this->GetPipeline(Pipeline_Handles) ->Actor()->SetVisibility(0);
}

//-----------------------------------------------------------------------------

Handle(asiVisu_Prs) asiVisu_IVCurvePrs::Instance(const Handle(ActAPI_INode)& N)
{
  return new asiVisu_IVCurvePrs(N);
}
