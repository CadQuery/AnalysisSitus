//-----------------------------------------------------------------------------
// Created on: 22 August 2019
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
#include <asiVisu_DeviationPrs.h>

// asiVisu includes
#include <asiVisu_DeviationDataProvider.h>
#include <asiVisu_MeshNScalarPipeline.h>
#include <asiVisu_MeshResultUtils.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

//! Creates a Presentation object for the passed Node.
//! \param[in] N Node to create a Presentation for.
asiVisu_DeviationPrs::asiVisu_DeviationPrs(const Handle(ActAPI_INode)& N)
: asiVisu_DefaultPrs(N)
{
  Handle(asiData_DeviationNode)
    devNode = Handle(asiData_DeviationNode)::DownCast(N);

  // Create Data Provider.
  Handle(asiVisu_DeviationDataProvider) DP = new asiVisu_DeviationDataProvider(devNode);

  // Create Pipeline.
  this->addPipeline        ( Pipeline_Main, new asiVisu_MeshNScalarPipeline );
  this->assignDataProvider ( Pipeline_Main, DP );

  // Initialize scalar bar.
  m_scalarBarWidget = vtkSmartPointer<vtkScalarBarWidget>::New();
  asiVisu_MeshResultUtils::InitScalarBarWidget(m_scalarBarWidget);

  this->GetPipeline(Pipeline_Main)->Actor()->GetProperty()->SetPointSize(0);
}

//-----------------------------------------------------------------------------

//! Factory method for Presentation.
//! \param[in] N Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_DeviationPrs::Instance(const Handle(ActAPI_INode)& N)
{
  return new asiVisu_DeviationPrs(N);
}

//-----------------------------------------------------------------------------

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void asiVisu_DeviationPrs::afterUpdatePipelines() const
{
  m_scalarBarWidget->On();

  // Pipeline for shaded surface
  Handle(asiVisu_MeshNScalarPipeline)
    pl = Handle(asiVisu_MeshNScalarPipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

  // Initialize scalar bar Actor
  m_scalarBarWidget->GetScalarBarActor()->SetLookupTable( pl->Mapper()->GetLookupTable() );
  m_scalarBarWidget->GetScalarBarActor()->SetTitle("Deviation field");
}

//-----------------------------------------------------------------------------

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void asiVisu_DeviationPrs::renderPipelines(vtkRenderer* renderer) const
{
  // Scalar bar
  if ( !m_scalarBarWidget->GetCurrentRenderer() )
  {
    m_scalarBarWidget->SetInteractor( renderer->GetRenderWindow()->GetInteractor() );
    m_scalarBarWidget->SetDefaultRenderer(renderer);
    m_scalarBarWidget->SetCurrentRenderer(renderer);
  }
}

//-----------------------------------------------------------------------------

//! Callback for de-rendering.
//! \param theRenderer [in] renderer.
void asiVisu_DeviationPrs::deRenderPipelines(vtkRenderer* renderer) const
{
  asiVisu_NotUsed(renderer);

  m_scalarBarWidget->Off();
}
