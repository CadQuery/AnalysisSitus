//-----------------------------------------------------------------------------
// Created on: 01 March 2017
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
#include <asiVisu_RESegmentPrs.h>

// asiVisu includes
#include <asiVisu_RESegmentDataProvider.h>
#include <asiVisu_PointsPipeline.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//! Creates a Presentation object for the passed Node.
//! \param N [in] Node to create a Presentation for.
asiVisu_RESegmentPrs::asiVisu_RESegmentPrs(const Handle(ActAPI_INode)& N)
: asiVisu_Prs(N)
{
  Handle(asiData_RESegmentNode)
    segment_n = Handle(asiData_RESegmentNode)::DownCast(N);

  // Create Data Provider
  Handle(asiVisu_RESegmentDataProvider) DP = new asiVisu_RESegmentDataProvider(segment_n);

  // Pipeline for points
  this->addPipeline        ( Pipeline_Main, new asiVisu_PointsPipeline );
  this->assignDataProvider ( Pipeline_Main, DP );

  // Set specific visual style for reverse engineering point clouds
  this->GetPipeline(Pipeline_Main)->Actor()->GetProperty()->SetPointSize(1.0);
  this->GetPipeline(Pipeline_Main)->Actor()->GetProperty()->SetColor(0.75, 0.75, 0.75);
  this->GetPipeline(Pipeline_Main)->Actor()->GetProperty()->SetOpacity(1.0);
}

//! Factory method for Presentation.
//! \param N [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_RESegmentPrs::Instance(const Handle(ActAPI_INode)& N)
{
  return new asiVisu_RESegmentPrs(N);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool asiVisu_RESegmentPrs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_RESegmentPrs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void asiVisu_RESegmentPrs::afterInitPipelines()
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void asiVisu_RESegmentPrs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void asiVisu_RESegmentPrs::afterUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for highlighting.
void asiVisu_RESegmentPrs::highlight(vtkRenderer*                  theRenderer,
                                     const asiVisu_PickResult&     thePickRes,
                                     const asiVisu_SelectionNature theSelNature) const
{
  asiVisu_NotUsed(theRenderer);
  asiVisu_NotUsed(thePickRes);
  asiVisu_NotUsed(theSelNature);
}

//! Callback for highlighting reset.
void asiVisu_RESegmentPrs::unHighlight(vtkRenderer*                  theRenderer,
                                       const asiVisu_SelectionNature theSelNature) const
{
  asiVisu_NotUsed(theRenderer);
  asiVisu_NotUsed(theSelNature);
}

//! Callback for rendering.
void asiVisu_RESegmentPrs::renderPipelines(vtkRenderer* theRenderer) const
{
  asiVisu_NotUsed(theRenderer);
}

//! Callback for de-rendering.
void asiVisu_RESegmentPrs::deRenderPipelines(vtkRenderer* theRenderer) const
{
  asiVisu_NotUsed(theRenderer);
}
