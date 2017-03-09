//-----------------------------------------------------------------------------
// Created on: 25 November 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_Prs.h>

// Active Data (API) includes
#include <ActAPI_INode.h>

// OCCT includes
#include <Standard_ProgramError.hxx>

//! Creates Presentation object for the passed Node.
//! \param theNode [in] Node to create the presentation object for.
asiVisu_Prs::asiVisu_Prs(const Handle(ActAPI_INode)& theNode)
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
void asiVisu_Prs::InitPipelines()
{
  // Allow descendants to do some job
  this->beforeInitPipelines();

  // Perform actual initialization of pipelines in PRS group
  const DataProviderMap& aDataPrvMap = m_dataPrvRepo.Find(Group_Prs);
  PipelineMap::Iterator aPipesIt( m_pipelineRepo.Find(Group_Prs) );
  for ( ; aPipesIt.More(); aPipesIt.Next() )
  {
    // Get pipeline
    const Handle(asiVisu_Pipeline)& aPipeline = aPipesIt.Value();
    const int aPipeKey = aPipesIt.Key();

    // Check Data Provider
    if ( !aDataPrvMap.IsBound(aPipeKey) )
      Standard_ProgramError::Raise("No Data Provider bound to pipeline");

    // Assemble Parameters
    Handle(asiVisu_DataProvider) aDataPrv = this->dataProvider(aPipeKey);

    // Initialize pipeline
    aPipeline->SetInput(aDataPrv);
  }

  // Allow descendants doing some job
  this->afterInitPipelines();
}

//! Attempts to build the underlying pipelines (if not yet) and then
//! sends VTK Update request in order to re-pass them.
void asiVisu_Prs::UpdatePipelines() const
{
  // Allow descendants to do some job
  this->beforeUpdatePipelines();

  // Core loop
  PipelineMap::Iterator aPipesIt( m_pipelineRepo.Find(Group_Prs) );
  for ( ; aPipesIt.More(); aPipesIt.Next() )
  {
    // Get pipeline
    const Handle(asiVisu_Pipeline)& aPipeline = aPipesIt.Value();
    aPipeline->Update();
  }

  // Allow descendants to do some job
  this->afterUpdatePipelines();
}

//! Accessor for a visualization pipeline by its ID.
//! \param theId [in] pipeline ID.
//! \return requested visualization pipeline.
const Handle(asiVisu_Pipeline)& asiVisu_Prs::GetPipeline(const int theId) const
{
  const PipelineMap& aPLMap = m_pipelineRepo.Find(Group_Prs);

  if ( !aPLMap.IsBound(theId) )
    Standard_ProgramError::Raise("Pipeline does not exist");

  return aPLMap.Find(theId);
}

//! Gets list of visualization pipelines.
//! Highlighting pipelines are not included in the list.
//! \return list of visualization pipeline.
const Handle(asiVisu_HPipelineList) asiVisu_Prs::GetPipelineList() const
{
  Handle(asiVisu_HPipelineList) aList = new asiVisu_HPipelineList();
  asiVisu_Prs::PipelineMap::Iterator aMapIt( m_pipelineRepo.Find(Group_Prs) );
  for ( ; aMapIt.More(); aMapIt.Next() )
    aList->Append( aMapIt.Value() );

  return aList;
}

//! Accessor for a visualization pipeline used for picking.
//! \param theIdx [in] index of the picking pipeline to access.
//! \return requested visualization pipeline.
Handle(asiVisu_Pipeline) asiVisu_Prs::GetPickPipeline(const int theIdx) const
{
  const PipelineMap& aPLMap = m_pipelineRepo.Find(Group_Pick);

  if ( !aPLMap.IsBound(theIdx) )
    return NULL;

  return aPLMap.Find(theIdx);
}

//! Gets list of picking pipelines.
//! \return list of picking pipeline.
const Handle(asiVisu_HPipelineList) asiVisu_Prs::GetPickPipelineList() const
{
  Handle(asiVisu_HPipelineList) aList = new asiVisu_HPipelineList();
  asiVisu_Prs::PipelineMap::Iterator aMapIt( m_pipelineRepo.Find(Group_Pick) );
  for ( ; aMapIt.More(); aMapIt.Next() )
    aList->Append( aMapIt.Value() );

  return aList;
}

//! Accessor for a visualization pipeline used for detection.
//! \param theIdx [in] index of the detection pipeline to access.
//! \return requested visualization pipeline.
const Handle(asiVisu_Pipeline)& asiVisu_Prs::GetDetectPipeline(const int theIdx) const
{
  return m_pipelineRepo.Find(Group_Detect).Find(theIdx);
}

//! Gets list of detection pipelines.
//! \return list of detection pipeline.
const Handle(asiVisu_HPipelineList) asiVisu_Prs::GetDetectPipelineList() const
{
  Handle(asiVisu_HPipelineList) aList = new asiVisu_HPipelineList();
  asiVisu_Prs::PipelineMap::Iterator aMapIt( m_pipelineRepo.Find(Group_Detect) );
  for ( ; aMapIt.More(); aMapIt.Next() )
    aList->Append( aMapIt.Value() );

  return aList;
}

//! Return the associated data object.
//! \return data object.
Handle(ActAPI_INode) asiVisu_Prs::GetNode() const
{
  return m_node;
}

//! Adds the underlying pipelines to the passed renderer.
//! \param theRenderer [in] render to add the pipelines to.
void asiVisu_Prs::RenderPipelines(vtkRenderer* theRenderer) const
{
  PipelineMap::Iterator aPipesIt( m_pipelineRepo.Find(Group_Prs) );
  for ( ; aPipesIt.More(); aPipesIt.Next() )
  {
    Handle(asiVisu_Pipeline) aBasePipeline = aPipesIt.Value();
    aBasePipeline->AddToRenderer(theRenderer);
  }

  // Allow descendants doing some job
  this->renderPipelines(theRenderer);
}

//! Removes the underlying pipelines from the passed renderer.
//! \param theRenderer [in] render to remove the pipelines from.
void asiVisu_Prs::DeRenderPipelines(vtkRenderer* theRenderer) const
{
  PipelineMap::Iterator aPipesIt( m_pipelineRepo.Find(Group_Prs) );
  for ( ; aPipesIt.More(); aPipesIt.Next() )
  {
    Handle(asiVisu_Pipeline) aBasePipeline = aPipesIt.Value();
    aBasePipeline->RemoveFromRenderer(theRenderer);
  }

  // Allow descendants doing some job
  this->deRenderPipelines(theRenderer);
}

//! Default highlighting actions.
//! \param theRenderer  [in] renderer.
//! \param thePickRes   [in] results of interactive picking.
//! \param theSelNature [in] selection nature (picking or detection).
void asiVisu_Prs::Highlight(vtkRenderer*                   theRenderer,
                            const asiVisu_PickResult&      thePickRes,
                            const asiVisu_SelectionNature& theSelNature) const
{
  this->highlight(theRenderer, thePickRes, theSelNature);
}

//! Default un-highlighting actions.
//! \param theRenderer  [in] renderer.
//! \param theSelNature [in] selection nature (picking or detection).
void asiVisu_Prs::UnHighlight(vtkRenderer*                   theRenderer,
                              const asiVisu_SelectionNature& theSelNature) const
{
  this->unHighlight(theRenderer, theSelNature);
}

//! Adds a pipeline to the Presentation object. This method should be used
//! by descendant classes in order to construct custom Presentation objects.
//! \param theId       [in] ID of the new pipeline.
//! \param thePipeline [in] pipeline to register.
void asiVisu_Prs::addPipeline(const int                       theId,
                              const Handle(asiVisu_Pipeline)& thePipeline)
{
  m_pipelineRepo.ChangeFind(Group_Prs).Bind(theId, thePipeline);
}

//! Assigns Data Provider instance to the given Pipeline.
//! \param theId           [in] ID of the pipeline to bind Data Provider to.
//! \param theDataProvider [in] Data Provider.
void asiVisu_Prs::assignDataProvider(const int                           theId,
                                     const Handle(asiVisu_DataProvider)& theDataProvider)
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
Handle(asiVisu_DataProvider) asiVisu_Prs::dataProvider(const int theId) const
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
Handle(asiVisu_DataProvider)
  asiVisu_Prs::dataProvider(const Handle(asiVisu_Pipeline)& thePipeline,
                            const PipelineGroup             theGroup) const
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
Handle(asiVisu_DataProvider) asiVisu_Prs::dataProviderPick(const int theId) const
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
Handle(asiVisu_DataProvider) asiVisu_Prs::dataProviderDetect(const int theId) const
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
void asiVisu_Prs::installPickPipeline(const Handle(asiVisu_Pipeline)&     thePipeline,
                                      const Handle(asiVisu_DataProvider)& theDataProvider,
                                      const int                           theIdx)
{
  m_pipelineRepo.ChangeFind(Group_Pick).Bind(theIdx, thePipeline);
  m_dataPrvRepo.ChangeFind(Group_Pick).Bind(theIdx, theDataProvider);
}

//! Sets up a specific pipeline used for mouse tracking detection.
//! \param thePipeline     [in] pipeline to set.
//! \param theDataProvider [in] Data Provider for the pipeline.
//! \param theIdx          [in] pipeline index (1 by default).
void asiVisu_Prs::installDetectPipeline(const Handle(asiVisu_Pipeline)&     thePipeline,
                                        const Handle(asiVisu_DataProvider)& theDataProvider,
                                        const int                           theIdx)
{
  m_pipelineRepo.ChangeFind(Group_Detect).Bind(theIdx, thePipeline);
  m_dataPrvRepo.ChangeFind(Group_Detect).Bind(theIdx, theDataProvider);
}
