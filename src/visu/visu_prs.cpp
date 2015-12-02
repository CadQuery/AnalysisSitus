//-----------------------------------------------------------------------------
// Created on: 25 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <visu_prs.h>

// Active Data (API) includes
#include <ActAPI_INode.h>

// OCCT includes
#include <Standard_ProgramError.hxx>

//! Creates Presentation object for the passed Node.
//! \param theNode [in] Node to create the presentation object for.
visu_prs::visu_prs(const Handle(ActAPI_INode)& theNode)
: Standard_Transient(),
  m_node(theNode)
{
  // Pipelines
  m_pipelineRepo.Bind( Group_Prs,    PipelineMap() );
  m_pipelineRepo.Bind( Group_Pick,   PipelineMap() );
  m_pipelineRepo.Bind( Group_Detect, PipelineMap() );

  // Data providers for the pipelines
  m_dataPrvRepo.Bind( Group_Prs,    DataProviderMap() );
  m_dataPrvRepo.Bind( Group_Pick,   DataProviderMap() );
  m_dataPrvRepo.Bind( Group_Detect, DataProviderMap() );
}

//! Initializes the underlying pipelines by calling their SetInput() methods
//! passing the actual Node Parameters there. This normally results in
//! reconstructing of the underlying VTK polygonal data sets playing as
//! sources for the visualization pipelines. This process must be
//! repeated each time the concerned Node Parameters are changed, e.g. due
//! execution of a Tree Function.
void visu_prs::InitPipelines()
{
  // Allow descendants to do some job
  this->beforeInitPipelines();

  // Perform actual initialization of pipelines in PRS group
  const DataProviderMap& aDataPrvMap = m_dataPrvRepo.Find(Group_Prs);
  PipelineMap::Iterator aPipesIt( m_pipelineRepo.Find(Group_Prs) );
  for ( ; aPipesIt.More(); aPipesIt.Next() )
  {
    // Get pipeline
    const Handle(visu_pipeline)& aPipeline = aPipesIt.Value();
    const int aPipeKey = aPipesIt.Key();

    // Check Data Provider
    if ( !aDataPrvMap.IsBound(aPipeKey) )
      Standard_ProgramError::Raise("No Data Provider bound to pipeline");

    // Assemble Parameters
    Handle(visu_data_provider) aDataPrv = this->dataProvider(aPipeKey);

    // Initialize pipeline
    aPipeline->SetInput(aDataPrv);
  }

  // Allow descendants doing some job
  this->afterInitPipelines();
}

//! Attempts to build the underlying pipelines (if not yet) and then
//! sends VTK Update request in order to re-pass them.
void visu_prs::UpdatePipelines() const
{
  // Allow descendants to do some job
  this->beforeUpdatePipelines();

  // Core loop
  PipelineMap::Iterator aPipesIt( m_pipelineRepo.Find(Group_Prs) );
  for ( ; aPipesIt.More(); aPipesIt.Next() )
  {
    // Get pipeline
    const Handle(visu_pipeline)& aPipeline = aPipesIt.Value();
    aPipeline->Update();
  }

  // Allow descendants to do some job
  this->afterUpdatePipelines();
}

//! Accessor for a visualization pipeline by its ID.
//! \param theId [in] pipeline ID.
//! \return requested visualization pipeline.
const Handle(visu_pipeline)& visu_prs::GetPipeline(const int theId) const
{
  const PipelineMap& aPLMap = m_pipelineRepo.Find(Group_Prs);

  if ( !aPLMap.IsBound(theId) )
    Standard_ProgramError::Raise("Pipeline does not exist");

  return aPLMap.Find(theId);
}

//! Gets list of visualization pipelines.
//! Highlighting pipelines are not included in the list.
//! \return list of visualization pipeline.
const Handle(h_visu_pipeline_list) visu_prs::GetPipelineList() const
{
  Handle(h_visu_pipeline_list) aList = new h_visu_pipeline_list();
  visu_prs::PipelineMap::Iterator aMapIt( m_pipelineRepo.Find(Group_Prs) );
  for ( ; aMapIt.More(); aMapIt.Next() )
    aList->Append( aMapIt.Value() );

  return aList;
}

//! Accessor for a visualization pipeline used for picking.
//! \param theIdx [in] index of the picking pipeline to access.
//! \return requested visualization pipeline.
Handle(visu_pipeline) visu_prs::GetPickPipeline(const int theIdx) const
{
  const PipelineMap& aPLMap = m_pipelineRepo.Find(Group_Pick);

  if ( !aPLMap.IsBound(theIdx) )
    return NULL;

  return aPLMap.Find(theIdx);
}

//! Gets list of picking pipelines.
//! \return list of picking pipeline.
const Handle(h_visu_pipeline_list) visu_prs::GetPickPipelineList() const
{
  Handle(h_visu_pipeline_list) aList = new h_visu_pipeline_list();
  visu_prs::PipelineMap::Iterator aMapIt( m_pipelineRepo.Find(Group_Pick) );
  for ( ; aMapIt.More(); aMapIt.Next() )
    aList->Append( aMapIt.Value() );

  return aList;
}

//! Accessor for a visualization pipeline used for detection.
//! \param theIdx [in] index of the detection pipeline to access.
//! \return requested visualization pipeline.
const Handle(visu_pipeline)& visu_prs::GetDetectPipeline(const int theIdx) const
{
  return m_pipelineRepo.Find(Group_Detect).Find(theIdx);
}

//! Gets list of detection pipelines.
//! \return list of detection pipeline.
const Handle(h_visu_pipeline_list) visu_prs::GetDetectPipelineList() const
{
  Handle(h_visu_pipeline_list) aList = new h_visu_pipeline_list();
  visu_prs::PipelineMap::Iterator aMapIt( m_pipelineRepo.Find(Group_Detect) );
  for ( ; aMapIt.More(); aMapIt.Next() )
    aList->Append( aMapIt.Value() );

  return aList;
}

//! Return the associated data object.
//! \return data object.
Handle(ActAPI_INode) visu_prs::GetNode() const
{
  return m_node;
}

//! Adds the underlying pipelines to the passed renderer.
//! \param theRenderer [in] render to add the pipelines to.
void visu_prs::RenderPipelines(vtkRenderer* theRenderer) const
{
  PipelineMap::Iterator aPipesIt( m_pipelineRepo.Find(Group_Prs) );
  for ( ; aPipesIt.More(); aPipesIt.Next() )
  {
    Handle(visu_pipeline)
      aBasePipeline = Handle(visu_pipeline)::DownCast( aPipesIt.Value() );
    aBasePipeline->AddToRenderer(theRenderer);
  }

  // Allow descendants doing some job
  this->renderPipelines(theRenderer);
}

//! Removes the underlying pipelines from the passed renderer.
//! \param theRenderer [in] render to remove the pipelines from.
void visu_prs::DeRenderPipelines(vtkRenderer* theRenderer) const
{
  PipelineMap::Iterator aPipesIt( m_pipelineRepo.Find(Group_Prs) );
  for ( ; aPipesIt.More(); aPipesIt.Next() )
  {
    Handle(visu_pipeline)
      aBasePipeline = Handle(visu_pipeline)::DownCast( aPipesIt.Value() );
    aBasePipeline->RemoveFromRenderer(theRenderer);
  }

  // Allow descendants doing some job
  this->deRenderPipelines(theRenderer);
}

//! Default highlighting actions.
//! \param theRenderer  [in] renderer.
//! \param thePickRes   [in] results of interactive picking.
//! \param theSelNature [in] selection nature (picking or detection).
void visu_prs::Highlight(vtkRenderer*                 theRenderer,
                         const visu_pick_result&      thePickRes,
                         const visu_selection_nature& theSelNature) const
{
  this->highlight(theRenderer, thePickRes, theSelNature);
}

//! Default un-highlighting actions.
//! \param theRenderer  [in] renderer.
//! \param theSelNature [in] selection nature (picking or detection).
void visu_prs::UnHighlight(vtkRenderer*                 theRenderer,
                           const visu_selection_nature& theSelNature) const
{
  this->unHighlight(theRenderer, theSelNature);
}

//! Adds a pipeline to the Presentation object. This method should be used
//! by descendant classes in order to construct custom Presentation objects.
//! \param theId       [in] ID of the new pipeline.
//! \param thePipeline [in] pipeline to register.
void visu_prs::addPipeline(const int                    theId,
                           const Handle(visu_pipeline)& thePipeline)
{
  m_pipelineRepo.ChangeFind(Group_Prs).Bind(theId, thePipeline);
}

//! Assigns Data Provider instance to the given Pipeline.
//! \param theId           [in] ID of the pipeline to bind Data Provider to.
//! \param theDataProvider [in] Data Provider.
void visu_prs::assignDataProvider(const int                         theId,
                                  const Handle(visu_data_provider)& theDataProvider)
{
  DataProviderMap& aDataPrvMap = m_dataPrvRepo.ChangeFind(Group_Prs);

  if ( aDataPrvMap.IsBound(theId) )
    aDataPrvMap.UnBind(theId);

  aDataPrvMap.Bind(theId, theDataProvider);
}

//! Accessor for Data Provider associated with a pipeline referenced to by the
//! passed ID.
//! \param theId [in] ID of the pipeline to get Data Provider for.
//! \return requested Data Provider.
Handle(visu_data_provider) visu_prs::dataProvider(const int theId) const
{
  const DataProviderMap& aDataPrvMap = m_dataPrvRepo.Find(Group_Prs);

  if ( !aDataPrvMap.IsBound(theId) )
    return NULL;

  return aDataPrvMap.Find(theId);
}

//! Accessor for Data Provider associated with the given pipeline.
//! \param theId [in] pipeline to get Data Provider for.
//! \param theGroup [in] group.
//! \return requested Data Provider.
Handle(visu_data_provider)
  visu_prs::dataProvider(const Handle(visu_pipeline)& thePipeline,
                         const PipelineGroup          theGroup) const
{
  const PipelineMap& aPipelineMap = m_pipelineRepo.Find(theGroup);

  // Get ID of the requested pipeline
  int ID = -1;
  for ( PipelineMap::Iterator it(aPipelineMap); it.More(); it.Next() )
    if ( it.Value() == thePipeline )
    {
      ID = it.Key();
      break;
    }

  if ( ID == -1 )
    return NULL;

  // Access Data Provides
  const DataProviderMap& aDataPrvMap = m_dataPrvRepo.Find(theGroup);
  return aDataPrvMap.Find(ID);
}

//! Accessor for Data Provider associated with a pipeline referenced to by the
//! passed ID. This method is specialized for PICKING pipelines.
//! \param theId [in] ID of the pipeline to get Data Provider for.
//! \return requested Data Provider.
Handle(visu_data_provider) visu_prs::dataProviderPick(const int theId) const
{
  const DataProviderMap& aDataPrvMap = m_dataPrvRepo.Find(Group_Pick);

  if ( !aDataPrvMap.IsBound(theId) )
    return NULL;

  return aDataPrvMap.Find(theId);
}

//! Accessor for Data Provider associated with a pipeline referenced to by the
//! passed ID. This method is specialized for DETECTION pipelines.
//! \param theId [in] ID of the pipeline to get Data Provider for.
//! \return requested Data Provider.
Handle(visu_data_provider) visu_prs::dataProviderDetect(const int theId) const
{
  const DataProviderMap& aDataPrvMap = m_dataPrvRepo.Find(Group_Detect);

  if ( !aDataPrvMap.IsBound(theId) )
    return NULL;

  return aDataPrvMap.Find(theId);
}

//! Sets up a specific pipeline used for picking.
//! \param thePipeline     [in] pipeline to set.
//! \param theDataProvider [in] Data Provider for the pipeline.
//! \param theIdx          [in] pipeline index (1 by default).
void visu_prs::installPickPipeline(const Handle(visu_pipeline)&      thePipeline,
                                   const Handle(visu_data_provider)& theDataProvider,
                                   const int                         theIdx)
{
  m_pipelineRepo.ChangeFind(Group_Pick).Bind(theIdx, thePipeline);
  m_dataPrvRepo.ChangeFind(Group_Pick).Bind(theIdx, theDataProvider);
}

//! Sets up a specific pipeline used for mouse tracking detection.
//! \param thePipeline     [in] pipeline to set.
//! \param theDataProvider [in] Data Provider for the pipeline.
//! \param theIdx          [in] pipeline index (1 by default).
void visu_prs::installDetectPipeline(const Handle(visu_pipeline)&      thePipeline,
                                     const Handle(visu_data_provider)& theDataProvider,
                                     const int                         theIdx)
{
  m_pipelineRepo.ChangeFind(Group_Detect).Bind(theIdx, thePipeline);
  m_dataPrvRepo.ChangeFind(Group_Detect).Bind(theIdx, theDataProvider);
}
