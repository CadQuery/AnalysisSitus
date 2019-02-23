//-----------------------------------------------------------------------------
// Created on: 30 January 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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
#include <asiVisu_SurfDeviationPrs.h>

// asiVisu includes
#include <asiVisu_MeshResultUtils.h>
#include <asiVisu_SurfaceDeviationPipeline.h>
#include <asiVisu_SurfaceDeviationDataProvider.h>
#include <asiVisu_Utils.h>

// VTK includes
#pragma warning(push, 0)
#include <vtkMapper.h>
#include <vtkProperty.h>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Creates a Presentation object for the passed Node.
//! \param[in] N Node to create a Presentation for.
asiVisu_SurfDeviationPrs::asiVisu_SurfDeviationPrs(const Handle(ActAPI_INode)& N)
: asiVisu_DefaultPrs(N)
{
  Handle(asiData_SurfDeviationNode)
    sdNode = Handle(asiData_SurfDeviationNode)::DownCast(N);

  // Create Data Provider.
  Handle(asiVisu_SurfaceDeviationDataProvider)
    dp = new asiVisu_SurfaceDeviationDataProvider(N);

  // Create Pipeline.
  Handle(asiVisu_SurfaceDeviationPipeline)
    pl = new asiVisu_SurfaceDeviationPipeline;
  //
  pl->SetStepsNumber(100);

  // Main pipeline.
  this->addPipeline        (Pipeline_Main, pl);
  this->assignDataProvider (Pipeline_Main, dp);

  // Initialize scalar bar.
  m_scalarBarWidget = vtkSmartPointer<vtkScalarBarWidget>::New();
  asiVisu_MeshResultUtils::InitScalarBarWidget(m_scalarBarWidget);
}

//-----------------------------------------------------------------------------

//! Factory method for Presentation.
//! \param[in] N Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_SurfDeviationPrs::Instance(const Handle(ActAPI_INode)& N)
{
  return new asiVisu_SurfDeviationPrs(N);
}

//-----------------------------------------------------------------------------

//! Callback for update of Presentation pipelines.
void asiVisu_SurfDeviationPrs::afterUpdatePipelines() const
{
  // Actualize scalar bar.
  Handle(asiVisu_SurfaceDeviationPipeline)
    pl = Handle(asiVisu_SurfaceDeviationPipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

  // Initialize scalar bar Actor.
  m_scalarBarWidget->GetScalarBarActor()->SetLookupTable( pl->Mapper()->GetLookupTable() );

  // Set title.
  m_scalarBarWidget->GetScalarBarActor()->SetTitle("Surface deviation");

  // Enable widget.
  m_scalarBarWidget->On();
}

//-----------------------------------------------------------------------------

//! Callback for rendering.
//! \param[in] renderer renderer.
void asiVisu_SurfDeviationPrs::renderPipelines(vtkRenderer* renderer) const
{
  if ( !m_scalarBarWidget->GetCurrentRenderer() )
  {
    m_scalarBarWidget->SetInteractor( renderer->GetRenderWindow()->GetInteractor() );
    m_scalarBarWidget->SetDefaultRenderer(renderer);
    m_scalarBarWidget->SetCurrentRenderer(renderer);
  }
}

//-----------------------------------------------------------------------------

//! Callback for de-rendering.
//! \param[in] renderer renderer.
void asiVisu_SurfDeviationPrs::deRenderPipelines(vtkRenderer* renderer) const
{
  asiVisu_NotUsed(renderer);

  m_scalarBarWidget->Off();
}
