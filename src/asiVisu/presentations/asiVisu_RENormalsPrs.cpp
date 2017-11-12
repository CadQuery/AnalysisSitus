//-----------------------------------------------------------------------------
// Created on: 08 December 2016
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
