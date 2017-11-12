//-----------------------------------------------------------------------------
// Created on: 06 April 2016
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
  Handle(asiData_REPointsNode)
    points_n = Handle(asiData_REPointsNode)::DownCast(theNode);

  // Create Data Provider
  Handle(asiVisu_REPointsDataProvider) DP = new asiVisu_REPointsDataProvider(points_n);

  // Pipeline for points
  this->addPipeline        ( Pipeline_Main, new asiVisu_PointsPipeline );
  this->assignDataProvider ( Pipeline_Main, DP );

  // Set specific visual style for reverse engineering point clouds
  this->GetPipeline(Pipeline_Main)->Actor()->GetProperty()->SetPointSize(1.0);
  this->GetPipeline(Pipeline_Main)->Actor()->GetProperty()->SetColor(0.75, 0.75, 0.75);
  this->GetPipeline(Pipeline_Main)->Actor()->GetProperty()->SetOpacity(1.0);
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
void asiVisu_REPointsPrs::highlight(vtkRenderer*                  theRenderer,
                                    const asiVisu_PickResult&     thePickRes,
                                    const asiVisu_SelectionNature theSelNature) const
{
  asiVisu_NotUsed(theRenderer);
  asiVisu_NotUsed(thePickRes);
  asiVisu_NotUsed(theSelNature);
}

//! Callback for highlighting reset.
void asiVisu_REPointsPrs::unHighlight(vtkRenderer*                  theRenderer,
                                      const asiVisu_SelectionNature theSelNature) const
{
  asiVisu_NotUsed(theRenderer);
  asiVisu_NotUsed(theSelNature);
}

//! Callback for rendering.
void asiVisu_REPointsPrs::renderPipelines(vtkRenderer* theRenderer) const
{
  asiVisu_NotUsed(theRenderer);
}

//! Callback for de-rendering.
void asiVisu_REPointsPrs::deRenderPipelines(vtkRenderer* theRenderer) const
{
  asiVisu_NotUsed(theRenderer);
}
