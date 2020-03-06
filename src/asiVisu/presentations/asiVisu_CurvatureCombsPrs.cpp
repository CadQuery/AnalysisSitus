//-----------------------------------------------------------------------------
// Created on: 30 January 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018, Sergey Slyadnev
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
#include <asiVisu_CurvatureCombsPrs.h>

// asiVisu includes
#include <asiVisu_CurvatureCombsDataProvider.h>
#include <asiVisu_CurvatureCombsPipeline.h>
#include <asiVisu_Utils.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

//! Creates a Presentation object for the passed Curvature Combs Node.
//! \param N [in] Geometry Curve Node to create a Presentation for.
asiVisu_CurvatureCombsPrs::asiVisu_CurvatureCombsPrs(const Handle(ActAPI_INode)& N)
: asiVisu_Prs(N)
{
  Handle(asiData_CurvatureCombsNode)
    combs_n = Handle(asiData_CurvatureCombsNode)::DownCast(N);

  Handle(asiData_PartNode)
    part_n = Handle(asiData_PartNode)::DownCast( combs_n->GetParentNode()->GetParentNode() );

  // Create Data Provider
  Handle(asiVisu_CurvatureCombsDataProvider)
    DP = new asiVisu_CurvatureCombsDataProvider(part_n, combs_n);

  // Pipeline for curvature combs
  this->addPipeline        ( Pipeline_Main, new asiVisu_CurvatureCombsPipeline );
  this->assignDataProvider ( Pipeline_Main, DP );

  // Configure presentation
  this->GetPipeline(Pipeline_Main)->Actor()->GetProperty()->SetPointSize(2.0f);
  this->GetPipeline(Pipeline_Main)->Actor()->GetProperty()->SetLineWidth(1.0f);
}

//-----------------------------------------------------------------------------

//! Factory method for Presentation.
//! \param[in] N Curve Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_CurvatureCombsPrs::Instance(const Handle(ActAPI_INode)& N)
{
  return new asiVisu_CurvatureCombsPrs(N);
}

//-----------------------------------------------------------------------------

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool asiVisu_CurvatureCombsPrs::IsVisible() const
{
  return m_node->HasUserFlags(NodeFlag_IsPresentationVisible);
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_CurvatureCombsPrs::beforeInitPipelines()
{
  // Do nothing...
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_CurvatureCombsPrs::afterInitPipelines()
{
  // Do nothing...
}

//-----------------------------------------------------------------------------

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void asiVisu_CurvatureCombsPrs::beforeUpdatePipelines() const
{}

//-----------------------------------------------------------------------------

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void asiVisu_CurvatureCombsPrs::afterUpdatePipelines() const
{}

//-----------------------------------------------------------------------------

//! Callback for highlighting.
void asiVisu_CurvatureCombsPrs::highlight(vtkRenderer*,
                                          const Handle(asiVisu_PickerResult)&,
                                          const asiVisu_SelectionNature) const
{}

//-----------------------------------------------------------------------------

//! Callback for highlighting reset.
void asiVisu_CurvatureCombsPrs::unHighlight(vtkRenderer*,
                                            const asiVisu_SelectionNature) const
{}

//-----------------------------------------------------------------------------

//! Callback for rendering.
void asiVisu_CurvatureCombsPrs::renderPipelines(vtkRenderer*) const
{}

//-----------------------------------------------------------------------------

//! Callback for de-rendering.
void asiVisu_CurvatureCombsPrs::deRenderPipelines(vtkRenderer*) const
{}
