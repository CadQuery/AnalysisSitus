//-----------------------------------------------------------------------------
// Created on: 05 September 2017
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
#include <asiVisu_GeomFaceContourPrs.h>

// asiVisu includes
#include <asiVisu_FaceContourDataProvider.h>
#include <asiVisu_FaceContourPipeline.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>

//-----------------------------------------------------------------------------

//! Creates a Presentation object for the passed Face Contour Node.
//! \param N [in] Face Contour Node to create a Presentation for.
asiVisu_GeomFaceContourPrs::asiVisu_GeomFaceContourPrs(const Handle(ActAPI_INode)& N)
: asiVisu_Prs(N)
{
  Handle(asiData_FaceContourNode)
    contour_n = Handle(asiData_FaceContourNode)::DownCast(N);

  // Create Data Provider for face contour
  Handle(asiVisu_FaceContourDataProvider)
    dp = new asiVisu_FaceContourDataProvider(contour_n);

  // Pipeline for face contour
  Handle(asiVisu_FaceContourPipeline) pl = new asiVisu_FaceContourPipeline;
  //
  this->addPipeline        (Pipeline_Main, pl);
  this->assignDataProvider (Pipeline_Main, dp);

  // Configure presentation
  pl -> Actor()->GetProperty()->SetLineWidth(2.0f);
  pl -> Actor()->GetProperty()->SetLighting(0);
  
  // Make contour be visualized always on top of the scene
  pl->Mapper()->SetRelativeCoincidentTopologyLineOffsetParameters(0,-66000);
  pl->Mapper()->SetRelativeCoincidentTopologyPolygonOffsetParameters(0,-66000);
  pl->Mapper()->SetRelativeCoincidentTopologyPointOffsetParameter(-66000);
}

//! Factory method for Presentation.
//! \param N [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_GeomFaceContourPrs::Instance(const Handle(ActAPI_INode)& N)
{
  return new asiVisu_GeomFaceContourPrs(N);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool asiVisu_GeomFaceContourPrs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_GeomFaceContourPrs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void asiVisu_GeomFaceContourPrs::afterInitPipelines()
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void asiVisu_GeomFaceContourPrs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void asiVisu_GeomFaceContourPrs::afterUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for highlighting.
//! \param renderer  [in] renderer.
//! \param pickRes   [in] picking results.
//! \param selNature [in] selection nature (picking or detecting).
void asiVisu_GeomFaceContourPrs::highlight(vtkRenderer*                  renderer,
                                           const asiVisu_PickResult&     pickRes,
                                           const asiVisu_SelectionNature selNature) const
{
  asiVisu_NotUsed(renderer);
  asiVisu_NotUsed(pickRes);
  asiVisu_NotUsed(selNature);
}

//! Callback for highlighting reset.
//! \param renderer  [in] renderer.
//! \param selNature [in] selection nature (picking or detecting).
void asiVisu_GeomFaceContourPrs::unHighlight(vtkRenderer*                  renderer,
                                             const asiVisu_SelectionNature selNature) const
{
  asiVisu_NotUsed(renderer);
  asiVisu_NotUsed(selNature);
}

//! Callback for rendering.
//! \param renderer [in] renderer.
void asiVisu_GeomFaceContourPrs::renderPipelines(vtkRenderer* renderer) const
{
  asiVisu_NotUsed(renderer);
}

//! Callback for de-rendering.
//! \param renderer [in] renderer.
void asiVisu_GeomFaceContourPrs::deRenderPipelines(vtkRenderer* renderer) const
{
  asiVisu_NotUsed(renderer);
}
