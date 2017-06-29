//-----------------------------------------------------------------------------
// Created on: 08 December 2016
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
#include <asiVisu_RENormalsPrs.h>

// asiVisu includes
#include <asiVisu_RENormalsDataProvider.h>
#include <asiVisu_Utils.h>
#include <asiVisu_VectorsPipeline.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//! Creates a Presentation object for the passed Node.
//! \param node [in] Node to create a Presentation for.
asiVisu_RENormalsPrs::asiVisu_RENormalsPrs(const Handle(ActAPI_INode)& node)
: asiVisu_Prs(node)
{
  Handle(asiData_RENormalsNode)
    normals_n = Handle(asiData_RENormalsNode)::DownCast(node);
  //
  Handle(asiData_REPointsNode)
    points_n = Handle(asiData_REPointsNode)::DownCast( normals_n->GetParentNode() );

  // Create Data Provider
  Handle(asiVisu_RENormalsDataProvider)
    DP = new asiVisu_RENormalsDataProvider(points_n, normals_n);

  // Pipeline for points
  this->addPipeline        ( Pipeline_Main, new asiVisu_VectorsPipeline );
  this->assignDataProvider ( Pipeline_Main, DP );
}

//! Factory method for Presentation.
//! \param node [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_RENormalsPrs::Instance(const Handle(ActAPI_INode)& node)
{
  return new asiVisu_RENormalsPrs(node);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool asiVisu_RENormalsPrs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_RENormalsPrs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void asiVisu_RENormalsPrs::afterInitPipelines()
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void asiVisu_RENormalsPrs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void asiVisu_RENormalsPrs::afterUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for highlighting.
void asiVisu_RENormalsPrs::highlight(vtkRenderer*                  renderer,
                                     const asiVisu_PickResult&     pickRes,
                                     const asiVisu_SelectionNature selNature) const
{
  asiVisu_NotUsed(renderer);
  asiVisu_NotUsed(pickRes);
  asiVisu_NotUsed(selNature);
}

//! Callback for highlighting reset.
void asiVisu_RENormalsPrs::unHighlight(vtkRenderer*                  renderer,
                                       const asiVisu_SelectionNature selNature) const
{
  asiVisu_NotUsed(renderer);
  asiVisu_NotUsed(selNature);
}

//! Callback for rendering.
void asiVisu_RENormalsPrs::renderPipelines(vtkRenderer* renderer) const
{
  asiVisu_NotUsed(renderer);
}

//! Callback for de-rendering.
void asiVisu_RENormalsPrs::deRenderPipelines(vtkRenderer* renderer) const
{
  asiVisu_NotUsed(renderer);
}
