//-----------------------------------------------------------------------------
// Created on: 05 September 2017
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
  pl -> Actor()->GetProperty()->SetLighting(false);
  
  // Make contour be visualized always on top of the scene
  pl->Mapper()->SetResolveCoincidentTopologyToPolygonOffset();
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
  return m_node->HasUserFlags(NodeFlag_IsPresentationVisible);
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
void asiVisu_GeomFaceContourPrs::highlight(vtkRenderer*,
                                           const Handle(asiVisu_PickerResult)&,
                                           const asiVisu_SelectionNature) const
{}

//! Callback for highlighting reset.
void asiVisu_GeomFaceContourPrs::unHighlight(vtkRenderer*,
                                             const asiVisu_SelectionNature) const
{}

//! Callback for rendering.
void asiVisu_GeomFaceContourPrs::renderPipelines(vtkRenderer*) const
{}

//! Callback for de-rendering.
void asiVisu_GeomFaceContourPrs::deRenderPipelines(vtkRenderer*) const
{}
