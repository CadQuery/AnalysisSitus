//-----------------------------------------------------------------------------
// Created on: 25 September 2015
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
#include <asiVisu_Pipeline.h>

// OCCT includes
#include <Standard_ProgramError.hxx>

// VTK includes
#include <vtkActor.h>
#include <vtkDataSet.h>
#include <vtkMapper.h>

//! Allocates a basic pipeline using the passed mapper and actor.
//! \param mapper [in] mapper for the pipeline.
//! \param actor  [in] actor corresponding to the visualization pipeline.
asiVisu_Pipeline::asiVisu_Pipeline(const vtkSmartPointer<vtkMapper>& mapper,
                                   const vtkSmartPointer<vtkActor>&  actor)
//
  : m_bInitialized ( false ),
    m_bBuilt       ( false ),
    m_mapper       ( mapper ),
    m_actor        ( actor ),
    m_dummyFilter  ( vtkSmartPointer<vtkPassThrough>::New() )
{}

//! Sets input data set for the pipeline.
//! \param data [in] input data set in VTK generic form.
void asiVisu_Pipeline::SetInputData(const vtkSmartPointer<vtkDataSet>& data)
{
  m_dummyFilter->SetInputData(data);
  m_bInitialized = true;
}

//! Sets input connection to the output of some external precedent filtering
//! routine. This allows append our visualization pipeline to some
//! existing pipeline.
//! \param out [in] output port of the precedent filter.
void asiVisu_Pipeline::SetInputConnection(vtkAlgorithmOutput* out)
{
  m_dummyFilter->SetInputConnection(out);
  m_bInitialized = true;
}

//! Requests update on the GL-mapper of the pipeline. Notice, that
//! the pipeline is built by demand, so you need to call update() at least once.
void asiVisu_Pipeline::Update()
{
  // Build the pipeline (if not yet)
  this->Build();

  // Request updated output if pipeline is initialized
  if ( m_bInitialized )
    m_mapper->Update();

  // Perform user callback AFTER the data has been updated
  this->callback_update();
}

//! Builds the transient VTK pipeline (ONCE).
void asiVisu_Pipeline::Build()
{
  if ( m_bBuilt )
    return; // Already built

  if ( m_mapper.GetPointer() == nullptr || m_actor.GetPointer() == nullptr )
    return; // Pipeline cannot be built due to incorrect initialization

  // Let external precedent filters to be the source
  vtkAlgorithmOutput* out_port = m_dummyFilter->GetOutputPort();

  // Connect all filters in the m_filters array, ending up with the mapper
  for ( int i = 1; i <= this->numFilters(); ++i )
  {
    vtkAlgorithm* filter = m_filters.ChangeValue(i);
    filter->SetInputConnection(out_port);
    out_port = filter->GetOutputPort();
  }

  m_mapper->SetInputConnection(out_port);
  m_actor->SetMapper(m_mapper);
  m_bBuilt = true;
}

//! Accessor to the pipeline's actor.
//! \return smart pointer to the actor.
vtkActor* asiVisu_Pipeline::Actor()
{
  return m_actor;
}

//! Accessor to the mapper.
//! \return smart pointer to the mapper.
vtkMapper* asiVisu_Pipeline::Mapper()
{
  return m_mapper;
}

//! Adds the actor of this pipeline to the passed renderer.
//! \param renderer [in] renderer to add the pipeline's actor to.
void asiVisu_Pipeline::AddToRenderer(vtkRenderer* renderer)
{
  renderer->AddActor( this->Actor() );

  // Callback for customization
  this->callback_add_to_renderer(renderer);
}

//! Removes the actor of this pipeline from the passed renderer.
//! \param renderer [in] renderer to remove the pipeline's actor from.
void asiVisu_Pipeline::RemoveFromRenderer(vtkRenderer* renderer)
{
  renderer->RemoveActor( this->Actor() );

  // Callback for customization
  this->callback_remove_from_renderer(renderer);
}

//! Sets newly generated modification timestamp for this pipeline.
void asiVisu_Pipeline::Modified()
{
  m_MTime = ActAux_TimeStampTool::Generate();
}

//! Returns actual modification timestamp for this pipeline.
//! \return actual modification timestamp.
Handle(ActAux_TimeStamp) asiVisu_Pipeline::GetMTime() const
{
  return m_MTime.IsNull() ? new ActAux_TimeStamp() : m_MTime;
}

//! Appends a filter at the end of the pipeline. Derived classes use this
//! method to build the custom pipeline. Note that the mapper specified
//! during construction is appended after the last filter.
//! \param filter [in] filter to append.
void asiVisu_Pipeline::append(const vtkSmartPointer<vtkAlgorithm>& filter)
{
  m_filters.Append(filter);
}

//! Accessor for the filters in the pipeline.
//! \param index [in] index in range [1, _num_filters()].
vtkSmartPointer<vtkAlgorithm> asiVisu_Pipeline::filter(const int index) const
{
  return m_filters.Value(index);
}

//! Returns the number of filters in the pipeline.
//! \return number of filters.
int asiVisu_Pipeline::numFilters() const
{
  return m_filters.Length();
}
