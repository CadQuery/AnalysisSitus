//-----------------------------------------------------------------------------
// Created on: 12 (*) April 2016
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
#include <asiVisu_IVTopoItemPrs.h>

// asiVisu includes
#include <asiVisu_ShapeDataProvider.h>
#include <asiVisu_ShapePipeline.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

//! Creates a Presentation object for the passed Node.
//! \param[in] N Node to create a Presentation for.
asiVisu_IVTopoItemPrs::asiVisu_IVTopoItemPrs(const Handle(ActAPI_INode)& N)
: asiVisu_DefaultPrs(N)
{
  // Create Data Provider
  Handle(asiVisu_ShapeDataProvider)
    DP = new asiVisu_ShapeDataProvider( N->GetId(),
                                        ActParamStream() << N->Parameter(asiData_IVTopoItemNode::PID_Geometry)
                                                         << N->Parameter(asiData_IVTopoItemNode::PID_TessLinDefl)
                                                         << N->Parameter(asiData_IVTopoItemNode::PID_TessAngDefl) );

  // Main pipeline
  Handle(asiVisu_ShapePipeline) pl = new asiVisu_ShapePipeline(false);
  //
  pl->GetDisplayModeFilter()->SetDisplayMode(ShapeDisplayMode_ShadedAndWireframe);

  // Pipeline for contours
  this->addPipeline        ( Pipeline_Main, pl );
  this->assignDataProvider ( Pipeline_Main, DP );

  // Configure
  pl->Actor()->GetProperty()->SetPointSize(5.0f);
  pl->Actor()->GetProperty()->SetLineWidth(2.0f);
  pl->Actor()->GetProperty()->RenderLinesAsTubesOn();
}

//-----------------------------------------------------------------------------

//! Factory method for Presentation.
//! \param[in] N Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_IVTopoItemPrs::Instance(const Handle(ActAPI_INode)& N)
{
  return new asiVisu_IVTopoItemPrs(N);
}

//-----------------------------------------------------------------------------

//! Sets custom color.
//! \param[in] color color to set.
void asiVisu_IVTopoItemPrs::Colorize(const QColor& color) const
{
  if ( !color.isValid() )
    return;

  Handle(asiVisu_ShapePipeline)
    pl = Handle(asiVisu_ShapePipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

  pl->Actor()->GetProperty()->SetColor( color.redF(),
                                        color.greenF(),
                                        color.blueF() );
}

//-----------------------------------------------------------------------------

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void asiVisu_IVTopoItemPrs::afterUpdatePipelines() const
{
  Handle(asiData_IVTopoItemNode)
    N = Handle(asiData_IVTopoItemNode)::DownCast( this->GetNode() );

  /* Actualize color */

  if ( N->HasColor() )
  {
    QColor color = asiVisu_Utils::IntToColor( N->GetColor() );
    this->Colorize(color);
  }
}
