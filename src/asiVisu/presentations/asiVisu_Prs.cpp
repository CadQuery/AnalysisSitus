//-----------------------------------------------------------------------------
// Created on: 25 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2015-present, Sergey Slyadnev
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
#include <asiVisu_Prs.h>

// Active Data (API) includes
#include <ActAPI_INode.h>

// OCCT includes
#include <Standard_ProgramError.hxx>

// VTK includes
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

//! Creates Presentation object for the passed Node.
//! \param[in] N Data Node to create the presentation object for.
asiVisu_Prs::asiVisu_Prs(const Handle(ActAPI_INode)& N)
: Standard_Transient (),
  m_node             (N)
{
  // Pipelines.
  m_pipelineRepo.Bind( Group_Prs,    PipelineMap() );
  m_pipelineRepo.Bind( Group_Pick,   PipelineMap() );
  m_pipelineRepo.Bind( Group_Detect, PipelineMap() );

  // Data providers for the pipelines.
  m_dataPrvRepo.Bind( Group_Prs,    DataProviderMap() );
  m_dataPrvRepo.Bind( Group_Pick,   DataProviderMap() );
  m_dataPrvRepo.Bind( Group_Detect, DataProviderMap() );
}

//-----------------------------------------------------------------------------

bool asiVisu_Prs::IsColorizable() const
{
  return false;
}

//-----------------------------------------------------------------------------

void asiVisu_Prs::SetColor(const QColor& color) const
{
  for ( PipelineMap::Iterator pit( m_pipelineRepo.Find(Group_Prs) );
        pit.More(); pit.Next() )
  {
    // Get pipeline and actor.
    const Handle(asiVisu_Pipeline)& pipeline = pit.Value();
    //
    vtkActor*    actor = pipeline->Actor();
    vtkProperty* prop  = actor->GetProperty();

    // Set color for VTK property.
    prop->SetColor( color.redF(), color.greenF(), color.blueF() );
  }
}

//-----------------------------------------------------------------------------

//! Initializes the underlying pipelines by calling their SetInput() methods
//! passing the actual Node Parameters there. This normally results in
//! reconstructing of the underlying VTK polygonal data sets playing as
//! sources for the visualization pipelines. This process must be
//! repeated each time the concerned Node Parameters are changed, e.g. due
//! execution of a Tree Function.
void asiVisu_Prs::InitPipelines()
{
  // Allow descendants to do some job.
  this->beforeInitPipelines();

  // Perform actual initialization of pipelines in PRS group.
  const DataProviderMap& dataPrvMap = m_dataPrvRepo.Find(Group_Prs);
 //
  for ( PipelineMap::Iterator pit( m_pipelineRepo.Find(Group_Prs) );
        pit.More(); pit.Next() )
  {
    // Get pipeline.
    const Handle(asiVisu_Pipeline)& pipeline   = pit.Value();
    const int                       pipelineId = pit.Key();

    // Get data drovider.
    if ( !dataPrvMap.IsBound(pipelineId) )
      Standard_ProgramError::Raise("No Data Provider bound to pipeline");
    //
    Handle(asiVisu_DataProvider) dp = this->dataProvider(pipelineId);

    // Initialize pipeline.
    pipeline->SetInput(dp);
  }

  // Allow descendants doing whatever job.
  this->afterInitPipelines();
}

//-----------------------------------------------------------------------------

//! Attempts to build the underlying pipelines (if not yet) and then
//! sends VTK "Update" request in order to re-pass them.
void asiVisu_Prs::UpdatePipelines() const
{
  // Allow descendants to do whatever job.
  this->beforeUpdatePipelines();

  // Loop over the pipelines.
  for ( PipelineMap::Iterator pit( m_pipelineRepo.Find(Group_Prs) );
        pit.More(); pit.Next() )
  {
    // Update pipeline.
    const Handle(asiVisu_Pipeline)& pipeline = pit.Value();
    //
    pipeline->Update();
  }

  // Allow descendants to do whatever job.
  this->afterUpdatePipelines();
}

//-----------------------------------------------------------------------------

//! Accessor for a visualization pipeline by its ID.
//! \param[in] id pipeline ID.
//! \return requested visualization pipeline.
Handle(asiVisu_Pipeline) asiVisu_Prs::GetPipeline(const int id) const
{
  // Get presentation pipelines.
  const PipelineMap& plMap = m_pipelineRepo.Find(Group_Prs);
  //
  if ( !plMap.IsBound(id) )
    return NULL;

  return plMap.Find(id);
}

//-----------------------------------------------------------------------------

//! Returns the list of visualization pipelines dedicated to presentation.
//! Highlighting pipelines are not included in the list.
//! \return list of visualization pipelines.
Handle(asiVisu_HPipelineList) asiVisu_Prs::GetPipelineList() const
{
  Handle(asiVisu_HPipelineList) result = new asiVisu_HPipelineList;
  //
  for ( PipelineMap::Iterator pit( m_pipelineRepo.Find(Group_Prs) );
        pit.More(); pit.Next() )
    result->Append( pit.Value() );

  return result;
}

//-----------------------------------------------------------------------------

//! Accessor for a visualization pipeline used for picking.
//! \param[in] idx index of the picking pipeline to access.
//! \return requested visualization pipeline.
Handle(asiVisu_Pipeline) asiVisu_Prs::GetPickPipeline(const int idx) const
{
  // Get picking pipelines.
  const PipelineMap& plMap = m_pipelineRepo.Find(Group_Pick);
  //
  if ( !plMap.IsBound(idx) )
    return NULL;

  return plMap.Find(idx);
}

//-----------------------------------------------------------------------------

//! Returns the list of picking pipelines.
//! \return list of picking pipelines.
Handle(asiVisu_HPipelineList) asiVisu_Prs::GetPickPipelineList() const
{
  Handle(asiVisu_HPipelineList) result = new asiVisu_HPipelineList;
  //
  for ( PipelineMap::Iterator pit( m_pipelineRepo.Find(Group_Pick) );
        pit.More(); pit.Next() )
    result->Append( pit.Value() );

  return result;
}

//-----------------------------------------------------------------------------

//! Accessor for a visualization pipeline used for detection.
//! \param[in] idx index of the detection pipeline to access.
//! \return requested visualization pipeline.
Handle(asiVisu_Pipeline) asiVisu_Prs::GetDetectPipeline(const int idx) const
{
  // Get detection pipelines.
  const PipelineMap& plMap = m_pipelineRepo.Find(Group_Detect);
  //
  if ( !plMap.IsBound(idx) )
    return NULL;

  return plMap.Find(idx);
}

//-----------------------------------------------------------------------------

//! Gets list of detection pipelines.
//! \return list of detection pipelineû.
Handle(asiVisu_HPipelineList) asiVisu_Prs::GetDetectPipelineList() const
{
  Handle(asiVisu_HPipelineList) result = new asiVisu_HPipelineList;
  //
  for ( PipelineMap::Iterator pit( m_pipelineRepo.Find(Group_Detect) );
        pit.More(); pit.Next() )
    result->Append( pit.Value() );

  return result;
}

//-----------------------------------------------------------------------------

//! Return the associated data object.
//! \return data object.
const Handle(ActAPI_INode)& asiVisu_Prs::GetNode() const
{
  return m_node;
}

//-----------------------------------------------------------------------------

//! Adds the underlying pipelines to the passed renderer.
//! \param[in] renderer renderer to add the pipelines to.
void asiVisu_Prs::RenderPipelines(vtkRenderer* renderer) const
{
  for ( PipelineMap::Iterator pit( m_pipelineRepo.Find(Group_Prs) );
        pit.More(); pit.Next() )
  {
    const Handle(asiVisu_Pipeline)& pl = pit.Value();
    //
    pl->AddToRenderer(renderer);
  }

  // Allow descendants doing whatever job.
  this->renderPipelines(renderer);
}

//-----------------------------------------------------------------------------

//! Removes the underlying pipelines from the passed renderer.
//! \param[in] renderer renderer to remove the pipelines from.
void asiVisu_Prs::DeRenderPipelines(vtkRenderer* renderer) const
{
  for ( PipelineMap::Iterator pit( m_pipelineRepo.Find(Group_Prs) );
        pit.More(); pit.Next() )
  {
    const Handle(asiVisu_Pipeline)& pl = pit.Value();
    //
    pl->RemoveFromRenderer(renderer);
  }

  // Allow descendants doing whatever job.
  this->deRenderPipelines(renderer);
}

//-----------------------------------------------------------------------------

//! Default highlighting actions. This method does nothing but delegates the
//! highlighting logic to the sub-classes.
//! \param[in] renderer  renderer.
//! \param[in] pickRes   results of interactive picking.
//! \param[in] selNature selection nature (picking or detection).
void asiVisu_Prs::Highlight(vtkRenderer*                        renderer,
                            const Handle(asiVisu_PickerResult)& pickRes,
                            const asiVisu_SelectionNature       selNature) const
{
  this->highlight(renderer, pickRes, selNature);
}

//-----------------------------------------------------------------------------

//! Default un-highlighting actions.
//! \param[in] renderer  renderer.
//! \param[in] selNature selection nature (picking or detection).
void asiVisu_Prs::UnHighlight(vtkRenderer*                  renderer,
                              const asiVisu_SelectionNature selNature) const
{
  this->unHighlight(renderer, selNature);
}

//-----------------------------------------------------------------------------

//! Allows sub-classes to re-initialize the working cell picker according
//! to their particular needs. E.g. a Presentation may prepare a cell
//! locator tool on the available VTK data for fast picking.
//! \param[in] picker picker to initialize at Presentation level.
void asiVisu_Prs::InitializePicker(const vtkSmartPointer<vtkCellPicker>& picker) const
{
  asiVisu_NotUsed(picker); // Do nothing at super-class
}

//-----------------------------------------------------------------------------

//! Sets diagnostic tools to all existing Presentations.
//! \param[in] progress progress notifier.
//! \param[in] plotter  imperative plotter.
void asiVisu_Prs::SetDiagnosticTools(ActAPI_ProgressEntry progress,
                                     ActAPI_PlotterEntry  plotter)
{
  m_progress = progress;
  m_plotter  = plotter;
}

//-----------------------------------------------------------------------------

//! Adds a pipeline to the Presentation object. This method should be used
//! by descendant classes in order to construct custom Presentation objects.
//! \param[in] id       ID of the new pipeline.
//! \param[in] pipeline pipeline to register.
void asiVisu_Prs::addPipeline(const int                       id,
                              const Handle(asiVisu_Pipeline)& pipeline)
{
  m_pipelineRepo.ChangeFind(Group_Prs).Bind(id, pipeline);
}

//-----------------------------------------------------------------------------

//! Assigns Data Provider instance to the given Pipeline.
//! \param[in] id           ID of the pipeline to bind Data Provider to.
//! \param[in] dataProvider Data Provider.
void asiVisu_Prs::assignDataProvider(const int                           id,
                                     const Handle(asiVisu_DataProvider)& dataProvider)
{
  DataProviderMap& dataPrvMap = m_dataPrvRepo.ChangeFind(Group_Prs);
  //
  if ( dataPrvMap.IsBound(id) )
    dataPrvMap.UnBind(id);

  dataPrvMap.Bind(id, dataProvider);
}

//-----------------------------------------------------------------------------

//! Accessor for Data Provider associated with a pipeline referenced to by the
//! passed ID.
//! \param[in] id ID of the pipeline to get Data Provider for.
//! \return requested Data Provider.
Handle(asiVisu_DataProvider) asiVisu_Prs::dataProvider(const int id) const
{
  const DataProviderMap& dataPrvMap = m_dataPrvRepo.Find(Group_Prs);
  //
  if ( !dataPrvMap.IsBound(id) )
    return NULL;

  return dataPrvMap.Find(id);
}

//-----------------------------------------------------------------------------

//! Accessor for Data Provider associated with the given pipeline.
//! \param[in] pipeline pipeline to get Data Provider for.
//! \param[in] group    group of pipelines to access.
//! \return requested Data Provider.
Handle(asiVisu_DataProvider)
  asiVisu_Prs::dataProvider(const Handle(asiVisu_Pipeline)& pipeline,
                            const PipelineGroup             group) const
{
  const PipelineMap& pipelineMap = m_pipelineRepo.Find(group);

  // Get ID of the requested pipeline.
  int ID = -1;
  for ( PipelineMap::Iterator it(pipelineMap); it.More(); it.Next() )
    if ( it.Value() == pipeline )
    {
      ID = it.Key();
      break;
    }

  if ( ID == -1 )
    return NULL;

  // Access Data Providers.
  const DataProviderMap& dataPrvMap = m_dataPrvRepo.Find(group);
  return dataPrvMap.Find(ID);
}

//-----------------------------------------------------------------------------

//! Accessor for the Data Provider associated with a pipeline referenced
//! by the passed ID. This method is specialized for PICKING pipelines.
//! \param[in] id ID of the pipeline to get Data Provider for.
//! \return requested Data Provider.
Handle(asiVisu_DataProvider) asiVisu_Prs::dataProviderPick(const int id) const
{
  const DataProviderMap& dataPrvMap = m_dataPrvRepo.Find(Group_Pick);
  //
  if ( !dataPrvMap.IsBound(id) )
    return NULL;

  return dataPrvMap.Find(id);
}

//-----------------------------------------------------------------------------

//! Accessor for Data Provider associated with a pipeline referenced to by the
//! passed ID. This method is specialized for DETECTION pipelines.
//! \param[in] id ID of the pipeline to get Data Provider for.
//! \return requested Data Provider.
Handle(asiVisu_DataProvider) asiVisu_Prs::dataProviderDetect(const int id) const
{
  const DataProviderMap& dataPrvMap = m_dataPrvRepo.Find(Group_Detect);
  //
  if ( !dataPrvMap.IsBound(id) )
    return NULL;

  return dataPrvMap.Find(id);
}

//-----------------------------------------------------------------------------

//! Sets up a specific pipeline used for picking.
//! \param[in] pipeline     pipeline to set.
//! \param[in] dataProvider Data Provider for the pipeline.
//! \param[in] idx          pipeline index (1 by default).
void asiVisu_Prs::installPickPipeline(const Handle(asiVisu_Pipeline)&     pipeline,
                                      const Handle(asiVisu_DataProvider)& dataProvider,
                                      const int                           idx)
{
  m_pipelineRepo.ChangeFind(Group_Pick).Bind(idx, pipeline);
  m_dataPrvRepo.ChangeFind(Group_Pick).Bind(idx, dataProvider);
}

//-----------------------------------------------------------------------------

//! Sets up a specific pipeline used for mouse tracking detection.
//! \param[in] pipeline     pipeline to set.
//! \param[in] dataProvider Data Provider for the pipeline.
//! \param[in] idx          pipeline index (1 by default).
void asiVisu_Prs::installDetectPipeline(const Handle(asiVisu_Pipeline)&     pipeline,
                                        const Handle(asiVisu_DataProvider)& dataProvider,
                                        const int                           idx)
{
  m_pipelineRepo.ChangeFind(Group_Detect).Bind(idx, pipeline);
  m_dataPrvRepo.ChangeFind(Group_Detect).Bind(idx, dataProvider);
}
