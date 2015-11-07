//-----------------------------------------------------------------------------
// Created on: 25 September 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su
//-----------------------------------------------------------------------------

// Own include
#include <visu_pipeline.h>

// OCCT includes
#include <Standard_ProgramError.hxx>

// VTK includes
#include <vtkActor.h>
#include <vtkDataSet.h>
#include <vtkMapper.h>

//! Allocates a basic pipeline using the passed mapper and actor.
//! \param mapper [in] mapper for the pipeline.
//! \param actor  [in] actor corresponding to the visualization pipeline.
visu_pipeline::visu_pipeline(const vtkSmartPointer<vtkMapper>& mapper,
                             const vtkSmartPointer<vtkActor>&  actor)
  : m_dummyFilter  ( vtkSmartPointer<vtkPassThroughFilter>::New() ),
    m_mapper       ( theMapper ),
    m_actor        ( theActor ),
    m_bInitialized ( false ),
    m_bBuilt       ( false )
{}

//! Sets input data set for the pipeline.
//! \param data [in] input data set in VTK generic form.
void visu_pipeline::set_input_data(const vtkSmartPointer<vtkDataSet>& data)
{
  m_dummy_filter->SetInputData(data);
  m_bInitialized = true;
}

//! Sets input connection to the output of some external precedent filtering
//! routine. This allows append our visualization pipeline to some
//! existing pipeline.
//! \param out [in] output port of the precedent filter.
void visu_pipeline::set_input_connection(vtkAlgorithmOutput* out)
{
  m_dummyFilter->SetInputConnection(out);
  m_bInitialized = true;
}

//! Requests update on the GL-mapper of the pipeline. Notice, that
//! the pipeline is built by demand, so you need to call update() at least once.
void visu_pipeline::update()
{
  // Build the pipeline (if not yet)
  this->build();

  // Perform user callback
  this->callback_update();

  // Request updated output if pipeline is initialized
  if ( m_bInitialized )
    m_mapper->Update();
}

//! Builds the transient VTK pipeline (ONCE).
void visu_pipeline::build()
{
  if ( m_bBuilt )
    return; // Already built

  if ( m_mapper.GetPointer() == NULL || m_actor.GetPointer() == NULL )
    return; // Pipeline cannot be built due to incorrect initialization

  // Let external precedent filters to be the source
  vtkAlgorithmOutput* out_port = m_dummyFilter->GetOutputPort();

  // Connect all filters in the m_filters array, ending up with the mapper
  for ( int i = 1; i <= this->_num_filters(); ++i )
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
vtkActor* visu_pipeline::Actor()
{
  return m_actor;
}

//! Accessor to the mapper.
//! \return smart pointer to the mapper.
vtkMapper* visu_pipeline::Mapper()
{
  return m_mapper;
}

//! Adds the actor of this pipeline to the passed renderer.
//! \param renderer [in] renderer to add the pipeline's actor to.
void visu_pipeline::add_to_renderer(vtkRenderer* renderer)
{
  renderer->AddActor( this->actor() );

  // Callback for customization
  this->callback_add_to_renderer(renderer);
}

//! Removes the actor of this pipeline from the passed renderer.
//! \param renderer [in] renderer to remove the pipeline's actor from.
void visu_pipeline::remove_from_renderer(vtkRenderer* renderer)
{
  renderer->RemoveActor( this->Actor() );

  // Callback for customization
  this->callback_remove_from_renderer(renderer);
}

//! Sets newly generated modification timestamp for this pipeline.
void visu_pipeline::modified()
{
  m_MTime = ActAux_TimeStampTool::Generate();
}

//! Returns actual modification timestamp for this pipeline.
//! \return actual modification timestamp.
Handle(ActAux_TimeStamp) visu_pipeline::get_MTime() const
{
  return m_MTime.IsNull() ? new ActAux_TimeStamp() : m_MTime;
}

//! Appends a filter at the end of the pipeline. Derived classes use this
//! method to build the custom pipeline. Note that the mapper specified
//! during construction is appended after the last filter.
//! \param filter [in] filter to append.
void visu_pipeline::append(const vtkSmartPointer<vtkAlgorithm>& filter)
{
  m_filters.Append(filter);
}

//! Accessor for the filters in the pipeline.
//! \param index [in] index in range [1, _num_filters()].
vtkSmartPointer<vtkAlgorithm> visu_pipeline::filter(const int index) const
{
  return m_filters.Value(theIndex);
}

//! Returns the number of filters in the pipeline.
//! \return number of filters.
int visu_pipeline::_num_filters() const
{
  return m_filters.Length();
}
