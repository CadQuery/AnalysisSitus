//-----------------------------------------------------------------------------
// Created on: 25 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <visu_prs_manager.h>

// Visualization includes
#include <visu_common.h>
#include <visu_node_info.h>
#include <visu_prs.h>
#include <visu_pipeline.h>
#include <visu_selection.h>
#include <visu_utils.h>

// VTK includes
#include <vtkCamera.h>
#include <vtkCellData.h>
#include <vtkIdTypeArray.h>
#include <vtkInformation.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>

// VIS includes
#pragma warning(push, 0)
#include <IVtkTools_ShapeDataSource.hxx>
#include <IVtkTools_ShapeObject.hxx>
#pragma warning(pop)

// OCCT includes
#include <NCollection_Sequence.hxx>
#include <Standard_ProgramError.hxx>

#undef COUT_DEBUG

//-----------------------------------------------------------------------------

vtkStandardNewMacro(visu_prs_manager);

//-----------------------------------------------------------------------------

void visu_prs_manager::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//-----------------------------------------------------------------------------

//! Constructs Presentation Manager setting up the underlying QVTK widget
//! as a child for the passed arbitrary Qt widget. It allows creation of
//! Presentation Manager providing basic visualization suite in any window.
//!
//! \param theParent   [in] parent Qt widget for underlying QVTK widget.
//! \param isOffscreen [in] indicates whether Presentation Manager should
//!                         operate in off-screen rendering mode. In that
//!                         case QVTK widget is not created.
visu_prs_manager::visu_prs_manager() : vtkObject(), m_widget(NULL)
{
  // Initialize renderer
  m_renderer = vtkSmartPointer<vtkRenderer>::New();
  m_renderer->GetActiveCamera()->ParallelProjectionOn();
  m_renderer->LightFollowCameraOn();
  m_renderer->TwoSidedLightingOn();
  m_renderer->SetBackground(0.1, 0.1, 0.1);

  // Initialize cell picker
  m_cellPicker = vtkSmartPointer<vtkCellPicker>::New();
  m_cellPicker->SetTolerance(0.001);

  // Create a picker for OCCT shapes
  m_shapePicker = vtkSmartPointer<IVtkTools_ShapePicker>::New();
  m_shapePicker->SetTolerance(0.025);
  m_shapePicker->SetRenderer(m_renderer);

  // Set default selection mode
  m_iSelectionModes = SelectionMode_None;

  // Initialize Render Window
  m_renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  m_renderWindow->AddRenderer(m_renderer);

  // Initialize Interactor Style instance for normal operation mode
  m_interactorStyleTrackball
    = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();

  // Initialize Render Window Interactor
  m_renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  m_renderWindowInteractor->SetRenderWindow(m_renderWindow);
  m_renderWindowInteractor->SetInteractorStyle(m_interactorStyleTrackball);
 
  // Initialize trihedron
  m_trihedron = vtkSmartPointer<vtkAxesActor>::New();
  m_trihedron->SetAxisLabels(0);
  m_trihedron->SetConeRadius(0);
  m_renderer->AddActor(m_trihedron);

  // Set selection mode
  this->SetSelectionMode(SelectionMode_Face);
}

//-----------------------------------------------------------------------------
// Presentation management
//-----------------------------------------------------------------------------

//! Re-initializes and updates the Node's Presentation.
//! \param theNode       [in] Data Node to actualize Presentation for.
//! \param withChildren  [in] indicates whether child Nodes should also be
//!                           actualized.
//! \param doFitContents [in] indicates whether to fit the viewport contents
//!                           after actualization of Presentations.
//! \param withRepaint   [in] if true, repaint view window.
void visu_prs_manager::Actualize(const Handle(ActAPI_INode)& theNode,
                                 const bool                  withChildren,
                                 const bool                  doFitContents,
                                 const bool                  withRepaint)
{
  if ( theNode.IsNull() )
    return;

  // Filter Nodes with bad domain-specific data
  if ( !theNode->IsValidData() )
    return;

  // Initialize Presentation if not yet
  bool isPrsOk = true;
  if ( !this->IsPresented(theNode) )
  {
    if ( !this->SetPresentation(theNode) )
      isPrsOk = false;
  }

  if ( isPrsOk )
  {
    // Clean up current selection
    m_currentSelection.PopAll(m_renderer, SelectionNature_Pick);
    m_currentSelection.PopAll(m_renderer, SelectionNature_Detection);

    // Finally, update Presentation
    if ( this->GetPresentation(theNode)->IsVisible() )
    {
      this->InitPresentation(theNode);
      this->RenderPresentation(theNode); // Render before update to adjust trihedron correctly
      this->UpdatePresentation(theNode, false);
    }
    else
    {
      this->DeRenderPresentation(theNode);
    }
  }

  // Proceed with children if requested
  if ( withChildren )
  {
    for ( Handle(ActAPI_IChildIterator) it = theNode->GetChildIterator(); it->More(); it->Next() )
      this->Actualize(it->Value(), true, false, false);
  }

  if ( doFitContents )
    visu_utils::AdjustCamera( m_renderer, this->PropsByTrihedron() );

  // Update view window
  if ( withRepaint && m_widget )
    m_widget->repaint();
}

//! Re-initializes and updates the Node's Presentation.
//! \param theNodeList   [in] list of Data Nodes to actualize Presentation for.
//! \param withChildren  [in] indicates whether child Nodes should also be
//!                           actualized.
//! \param doFitContents [in] indicates whether to fit the viewport contents
//!                           after actualization of Presentations.
//! \param withRepaint   [in] if true, repaint view window.
void visu_prs_manager::Actualize(const Handle(ActAPI_HNodeList)& theNodeList,
                                 const bool                      withChildren,
                                 const bool                      doFitContents,
                                 const bool                      withRepaint)
{
  if ( theNodeList.IsNull() )
    return;

  ActAPI_NodeList::Iterator aNodeIt( *theNodeList.operator->() );
  for ( ; aNodeIt.More(); aNodeIt.Next() )
    this->Actualize(aNodeIt.Value(), withChildren, false, false);

  if ( doFitContents )
    visu_utils::AdjustCamera( m_renderer, this->PropsByTrihedron() );

  // Update view window
  if ( withRepaint && m_widget )
    m_widget->repaint();
}

//! Rebuilds the entire scene so that it fully corresponds to the given
//! collection of Nodes exclusively.
//! \param theNodeList   [in] Data Nodes to visualize.
//! \param doFitContents [in] indicates whether to fit the viewport contents
//!                           after actualization of Presentations.
void visu_prs_manager::ActualizeExclusively(const Handle(ActAPI_HNodeList)& theNodeList,
                                            const bool                      doFitContents)
{
  if ( theNodeList.IsNull() )
    return;

  // Delete all existing Presentations except the passed ones
  ActAPI_DataObjectIdList aListToDel;
  for ( TNodePrsMap::Iterator it(m_nodePresentations); it.More(); it.Next() )
  {
    ActAPI_DataObjectId aCurrentID = it.Key();
    bool isInList = false;
    for ( ActAPI_NodeList::Iterator nit( *theNodeList.operator->() ); nit.More(); nit.Next() )
    {
      ActAPI_DataObjectId aPassedID = nit.Value()->GetId();
      if ( IsEqual(aPassedID, aCurrentID) )
      {
        isInList = true;
        break;
      }
    }

    if ( !isInList )
      aListToDel.Append(aCurrentID);
  }
  for ( ActAPI_DataObjectIdList::Iterator it(aListToDel); it.More(); it.Next() )
    this->DeletePresentation( it.Value() );

  // Actualize: it will build all necessary Presentations
  this->Actualize(theNodeList, false, doFitContents, true);
}

//! Checks whether the passed Node has been already registered in the
//! Presentation Manager.
//! \param theNode [in] Node to check.
//! \return true/false.
bool visu_prs_manager::IsPresented(const Handle(ActAPI_INode)& theNode)
{
  return this->IsPresented( theNode->GetId() );
}

//! Checks whether the passed Node has been already registered in the
//! Presentation Manager by the passed ID.
//! \param theNodeID [in] ID of the Node to check.
//! \return true/false.
bool visu_prs_manager::IsPresented(const ActAPI_DataObjectId& theNodeId)
{
  return m_nodePresentations.IsBound(theNodeId) > 0;
}

//! Sets up a presentation for the passed Node. If no presentation exists,
//! this method returns false.
//! \param theNode [in] Node to register a Presentation for.
//! \return true if presentation exists and can be registered so, false --
//!         otherwise.
bool visu_prs_manager::SetPresentation(const Handle(ActAPI_INode)& theNode)
{
  TCollection_AsciiString aNodeID = theNode->GetId();

  if ( m_nodePresentations.IsBound(aNodeID) )
    m_nodePresentations.UnBind(aNodeID); // If you do not want us to do this,
                                         // use HasPresentation method

  const visu_utils::TPrsAllocMap& anAllocMap = visu_utils::GetAllocMap();
  Standard_CString aNodeType = theNode->DynamicType()->Name();

  if ( !anAllocMap.IsBound(aNodeType) )
    return false; // No Presentation exists for Node

  const visu_prs_allocator& aPrsAlloc = anAllocMap.Find(aNodeType);
  m_nodePresentations.Bind( aNodeID, (*aPrsAlloc)(theNode) );

  return true;
}

//! Returns Presentation registered for the passed Node. If no Presentation
//! is registered, returns NULL.
//! \param theNode [in] Node to access Presentation for.
//! \return requested Presentation or NULL.
Handle(visu_prs)
  visu_prs_manager::GetPresentation(const Handle(ActAPI_INode)& theNode)
{
  return this->GetPresentation( theNode->GetId() );
}

//! Returns Presentation registered for the passed Node. If no Presentation
//! is registered, returns NULL.
//! \param theNodeId [in] ID of the Node to access Presentation for.
//! \return requested Presentation or NULL.
Handle(visu_prs)
  visu_prs_manager::GetPresentation(const ActAPI_DataObjectId& theNodeId)
{
  if ( !m_nodePresentations.IsBound(theNodeId) )
    return NULL;

  return m_nodePresentations.Find(theNodeId);
}

//! Performs initialization of the Node's Presentation by rebuilding the
//! input data sets for entire set of the underlying visualization pipelines.
//! Normally, this method should be invoked once at the very first
//! visualization request for a Node, and again each time when the pipelined
//! data is changed (e.g. due to Tree Function execution).
//! \param theNode [in] Node to (re-)initialize pipelines for.
void visu_prs_manager::InitPresentation(const Handle(ActAPI_INode)& theNode)
{
  this->InitPresentation( theNode->GetId() );
}

//! Performs initialization of the Node's Presentation by rebuilding the
//! input data sets for entire set of the underlying visualization pipelines.
//! Normally, this method should be invoked once at the very first
//! visualization request for a Node, and again each time when Nodal data
//! is changed (e.g. due to Tree Function execution).
//! \param theNodeID [in] ID of the Node to (re-)initialize pipelines for.
void visu_prs_manager::InitPresentation(const ActAPI_DataObjectId& theNodeId)
{
  if ( !m_nodePresentations.IsBound(theNodeId) )
    Standard_ProgramError::Raise("Presentation does not exist");

  const Handle(visu_prs)& aPrs = m_nodePresentations.Find(theNodeId);
  aPrs->InitPipelines();
}

//! Pushes the Node's Presentation to the renderer. Notice that Presentation
//! should exist and be registered by SetPresentation() method, otherwise
//! an exception is thrown.
//! \param theNode [in] Node to render the Presentation for.
void visu_prs_manager::RenderPresentation(const Handle(ActAPI_INode)& theNode)
{
  this->RenderPresentation( theNode->GetId() );
}

//! Pushes the Node's Presentation to the renderer. Notice that Presentation
//! should exist and be registered by SetPresentation() method, otherwise
//! an exception is thrown.
//! \param theNodeId [in] ID of the Node to render the Presentation for.
void visu_prs_manager::RenderPresentation(const ActAPI_DataObjectId& theNodeId)
{
  if ( !m_nodePresentations.IsBound(theNodeId) )
    Standard_ProgramError::Raise("Presentation does not exist");

  if ( m_renderer.GetPointer() == NULL )
    Standard_ProgramError::Raise("Renderer is NULL");

  Handle(visu_prs)
    aPrs = Handle(visu_prs)::DownCast( m_nodePresentations.Find(theNodeId) );
  aPrs->RenderPipelines(m_renderer);
}

//! Removes the Node's Presentation from the renderer. Notice that
//! Presentation should exist and be registered by SetPresentation()
//! method, otherwise an exception is thrown.
//! \param theNode [in] Node to de-render the Presentation for.
void visu_prs_manager::DeRenderPresentation(const Handle(ActAPI_INode)& theNode)
{
  this->DeRenderPresentation( theNode->GetId() );
}

//! Attempts to remove the Node's Presentation from the renderer.
//! \param theNodeId [in] ID of the Node to de-render the Presentation for.
void visu_prs_manager::DeRenderPresentation(const ActAPI_DataObjectId& theNodeId)
{
  if ( !m_nodePresentations.IsBound(theNodeId) )
    return; // Presentation does not exist --> be safe, do not fail here

  if ( m_renderer.GetPointer() == NULL )
    Standard_ProgramError::Raise("Renderer is NULL");

  Handle(visu_prs)
    aPrs = Handle(visu_prs)::DownCast( m_nodePresentations.Find(theNodeId) );
  aPrs->DeRenderPipelines(m_renderer);
}

//! Removes all existing Presentations from renderer.
void visu_prs_manager::DeRenderAllPresentations()
{
  for ( TNodePrsMap::Iterator it(m_nodePresentations); it.More(); it.Next() )
   this->DeRenderPresentation( it.Key() );
}

//! Builds (if not yet) visualization pipelines for the Node's Presentation
//! and sends VTK Update request for pipeline execution. Normally, you invoke
//! this method once you somehow affect the Node's data. Notice that a
//! Presentation should exist and be registered with SetPresentation()
//! method, otherwise an exception is thrown.
//!
//! \param theNode       [in] Node to update the Presentation for.
//! \param doFitContents [in] indicates whether to adjust camera in order for
//!                           the viewer contents to fit the rendering window.
void visu_prs_manager::UpdatePresentation(const Handle(ActAPI_INode)& theNode,
                                          const bool                  doFitContents)
{
  this->UpdatePresentation(theNode->GetId(), doFitContents);
}

//! Builds (if not yet) visualization pipelines for the Node's Presentation
//! and sends VTK Update request for pipeline execution. Normally, you invoke
//! this method once you somehow affect the Node's data. Notice that a
//! Presentation should exist and be registered with SetPresentation()
//! method, otherwise an exception is thrown.
//!
//! \param theNodeId     [in] ID of the Node to update the Presentation for.
//! \param doFitContents [in] indicates whether to adjust camera in order for
//!                           the viewer contents to fit the rendering window.
void visu_prs_manager::UpdatePresentation(const ActAPI_DataObjectId& theNodeId,
                                          const bool                 doFitContents)
{
  if ( !m_nodePresentations.IsBound(theNodeId) )
    Standard_ProgramError::Raise("Presentation does not exist");

  // Update presentation
  const Handle(visu_prs)& aPrs = m_nodePresentations.Find(theNodeId);
  aPrs->UpdatePipelines();

  // Update callbacks
  for ( NCollection_Sequence<unsigned long>::Iterator anIt(m_updateCallbackIds);
        anIt.More(); anIt.Next() )
  {
    InvokeEvent( anIt.Value() );
  }

  // Adjust trihedron
  this->adjustTrihedron();

  // Adjust camera if requested
  if ( doFitContents )
    visu_utils::AdjustCamera( m_renderer, this->PropsByTrihedron() );
}

//! Unbinds the Presentation instance for the given Node. If a Presentation
//! does not exist, returns false, otherwise -- true.
//! \param theNode [in] Node to remove the Presentation for.
//! \return true/false.
bool visu_prs_manager::DeletePresentation(const Handle(ActAPI_INode)& theNode)
{
  return this->DeletePresentation( theNode->GetId() );
}

//! Unbinds the Presentation instance for the given Node. If a Presentation
//! does not exist, returns false, otherwise -- true.
//! \param theNodeId [in] ID of the Node to remove the Presentation for.
//! \return true/false.
bool visu_prs_manager::DeletePresentation(const ActAPI_DataObjectId& theNodeId)
{
  if ( !this->IsPresented(theNodeId) )
    return false;

  this->DeRenderPresentation(theNodeId);
  m_nodePresentations.UnBind(theNodeId);
  return true;
}

//! Unbinds all registered Presentations.
void visu_prs_manager::DeleteAllPresentations()
{
  ActAPI_DataObjectIdList aListToDel;
  for ( TNodePrsMap::Iterator it(m_nodePresentations); it.More(); it.Next() )
  {
    ActAPI_DataObjectId aCurrentID = it.Key();
    aListToDel.Append(aCurrentID);
  }
  for ( ActAPI_DataObjectIdList::Iterator it(aListToDel); it.More(); it.Next() )
    this->DeletePresentation( it.Value() );
}

//-----------------------------------------------------------------------------
// Selection management
//-----------------------------------------------------------------------------

//! Sets selection modes.
//! \param theMode [in] selection modes to set.
void visu_prs_manager::SetSelectionMode(const int theMode)
{
  // We erase current highlighting only if user is not switching to NONE
  // selection mode
  if ( !(theMode & SelectionMode_None) )
  {
    // If user applies the same (or sub-set) selection mode as the currently
    // active one, then we do not erase anything. This could happen if user
    // re-enables some selection mode after neutral (NONE) one
    if ( !m_currentSelection.PickResult(SelectionNature_Pick).DoesSelectionCover(theMode) )
    {
      m_currentSelection.PopAll(m_renderer, SelectionNature_Pick);
      m_currentSelection.PopAll(m_renderer, SelectionNature_Detection);

      if ( m_widget )
        m_widget->repaint();
    }
  }

  // Set selection mode
  m_iSelectionModes = theMode;
}

//! Returns currently active selection modes.
//! \return selection modes.
int visu_prs_manager::GetSelectionMode() const
{
  return m_iSelectionModes;
}

//! Perform picking or detection by the passed display coordinates.
//! \param thePickInput [in] picking input data.
//! \param theSelNature [in] selection nature (picking or detection).
//! \return list of affected Data Node IDs.
ActAPI_DataObjectIdList
  visu_prs_manager::Pick(visu_pick_input*            thePickInput,
                         const visu_selection_nature theSelNature)
{
  /* ===================
   *  Some preparations
   * =================== */

  // Prepare a resulting collection
  ActAPI_DataObjectIdList aResult;

  // Selection is disabled
  if ( m_iSelectionModes & SelectionMode_None )
    return aResult;

  int  XStart     = thePickInput->Start.x();
  int  YStart     = thePickInput->Start.y();
  bool isMultiple = thePickInput->IsMultiple;

  // Reset current selection (if any)
  if ( !isMultiple )
    m_currentSelection.PopAll(m_renderer, theSelNature);

  /* =====================
   *  Perform actual pick
   * ===================== */

  // Prepare picking results
  vtkActor*         aPickedActor = NULL;
  visu_pick_result& aPickRes     = m_currentSelection.ChangePickResult(theSelNature);
  aPickRes.SetSelectionModes(m_iSelectionModes);

  if ( m_iSelectionModes & SelectionMode_Workpiece ) // Non-partial selection
  {
    m_cellPicker->Pick(XStart, YStart, 0, m_renderer);
    vtkIdType aPickedId = m_cellPicker->GetCellId();
    aPickedActor = m_cellPicker->GetActor();

    if ( !aPickedActor )
      return aResult; // Nothing has been picked

    aPickRes << aPickedActor << aPickedId;
  }
  else // Partial selection: for topological shapes only
  {
    this->actualizeShapeSelectionMode();

    // PICK (!!!)
    m_shapePicker->Pick(XStart, YStart, 0);

    // Traversing results
    visu_node_info*     aNodeInfo         = NULL;
    vtkActorCollection* anActorCollection = m_shapePicker->GetPickedActors();
    if ( anActorCollection )
    {
      anActorCollection->InitTraversal();
      while ( vtkActor* anActor = anActorCollection->GetNextActor() )
      {
        aPickedActor = anActor;
        aPickRes << aPickedActor;
        IVtkTools_ShapeDataSource* aDataSource = IVtkTools_ShapeObject::GetShapeSource(anActor);
        if ( !aDataSource )
          continue;

        // Access initial shape wrapper
        IVtkOCC_Shape::Handle aShapeWrapper = aDataSource->GetShape();
        if ( aShapeWrapper.IsNull() )
          continue;

        IVtk_IdType aShapeID = aShapeWrapper->GetId();
        IVtk_ShapeIdList subShapeIds = m_shapePicker->GetPickedSubShapesIds(aShapeID);

        // Get IDs of cells for picked sub-shapes.
        for ( IVtk_ShapeIdList::Iterator sIt(subShapeIds); sIt.More(); sIt.Next() )
        {
#if defined COUT_DEBUG
          const TopoDS_Shape& aSubShape = aShapeWrapper->GetSubShape( sIt.Value() );
          cout << "--------------------------------------------------------------" << endl;
          cout << "Sub-shape ID: " << sIt.Value() << endl;
          cout << "Sub-shape type: " << aSubShape.TShape()->DynamicType()->Name() << endl;
#endif
          aPickRes << sIt.Value();
        }

        aNodeInfo = visu_node_info::Retrieve(aPickedActor);
        if ( aNodeInfo )
          break;
      }
    }
#if defined COUT_DEBUG
    if ( !aNodeInfo )
      cout << "WARNING: Node Information hook of picked actor is NULL" << endl;
#endif
  }

  if ( !aPickedActor )
  {
    if ( m_widget && theSelNature == SelectionNature_Detection )
      m_widget->repaint(); // Clean up detection highlighting

    return aResult;
  }

  /* ======================================
   *  Ask Presentation to highlight itself
   * ====================================== */

  // Retrieve the corresponding Presentation by data object's ID
  ActAPI_DataObjectId
    aNodeId = visu_node_info::Retrieve(aPickedActor)->GetNodeId();

  if ( m_iSelectionModes & SelectionMode_Workpiece )
    aResult.Append(aNodeId);

  Handle(visu_prs)
    aPrs3D = Handle(visu_prs)::DownCast( this->GetPresentation(aNodeId) );

  if ( aPrs3D.IsNull() )
    Standard_ProgramError::Raise("Picked object without Presentation");

  // When picking erase detection at first in order to prevent blinking
  if ( theSelNature == SelectionNature_Pick )
    m_currentSelection.PopAll(m_renderer, SelectionNature_Detection);

  // Push selection to renderer
  m_currentSelection.PushToRender(aPrs3D, m_renderer, theSelNature);

  // Update view window
  if ( m_widget )
  {
    m_widget->repaint();
  }

  return aResult;
}

//! Set list of Nodes allowed for picking. This can be used to limit
//! the picking "perimeter".
//! \param theNodeList [in] list of Nodes allowed for picking.
void visu_prs_manager::SetPickList(const Handle(ActAPI_HNodeList)& theNodeList)
{
  m_bAllowedNodes = theNodeList;

  m_cellPicker->InitializePickList();

  if ( m_bAllowedNodes.IsNull() )
    return;

  ActAPI_NodeList::Iterator aListIt( *m_bAllowedNodes.operator->() );
  for ( ; aListIt.More(); aListIt.Next() )
  {
    Handle(visu_prs) aPrs = GetPresentation(aListIt.Value());
    if ( aPrs.IsNull() )
      continue;

    Handle(h_visu_pipeline_list) aPipelines = aPrs->GetPipelineList();
    if ( aPipelines.IsNull() )
      continue;

    visu_pipeline_list::Iterator aPipeIt( *aPipelines.operator->() );
    for ( ; aPipeIt.More(); aPipeIt.Next() )
    {
      Handle(visu_pipeline)
        aPipeline = Handle(visu_pipeline)::DownCast(aPipeIt.Value());

      if ( aPipeline.IsNull() )
        continue;

      m_cellPicker->AddPickList( aPipeline->Actor() );
    }
  }
}

//! Returns the list of Nodes allowed for picking when picking
//! from list is enabled.
//! \return list of Nodes allowed for picking.
Handle(ActAPI_HNodeList) visu_prs_manager::GetPickList() const
{
  return m_bAllowedNodes;
}

//! Enables or disables picking from list.
//! \param isEnabled [in] true/false.
void visu_prs_manager::SetPickFromList(const bool isEnabled) const
{
  m_cellPicker->SetPickFromList(isEnabled ? 1 : 0);
}

//! Checks whether the picking from list is enabled or not.
//! \return true/false.
bool visu_prs_manager::IsPickFromList() const
{
  return m_cellPicker->GetPickFromList() ? true : false;
}

//! Highlights Presentation for the passed Nodes.
//! \param theNodes [in] Nodes to highlight.
void visu_prs_manager::Highlight(const Handle(ActAPI_HNodeList)& theNodes)
{
  // Reset current selection (if any)
  m_currentSelection.PopAll(m_renderer, SelectionNature_Pick);
  m_currentSelection.ChangePickResult(SelectionNature_Pick).Clear();
  m_currentSelection.ChangePickResult(SelectionNature_Pick).SetSelectionModes(SelectionMode_Workpiece);

  if ( !(m_iSelectionModes & SelectionMode_None) )
  {
    // Iterate over the passed Data Nodes accessing & highlighting
    // their Presentations
    ActAPI_NodeList::Iterator aNodeIt( *theNodes.operator->() );
    for ( ; aNodeIt.More(); aNodeIt.Next() )
    {
      const Handle(ActAPI_INode)& aNode = aNodeIt.Value();

      // Access Presentation
      Handle(visu_prs)
        aPrs3D = Handle(visu_prs)::DownCast( this->GetPresentation(aNode) );

      if ( aPrs3D.IsNull() || !aPrs3D->IsVisible() )
        continue; // Node does not have Presentation to highlight

      // Push selection to renderer
      m_currentSelection.PushToRender(aPrs3D, m_renderer, SelectionNature_Pick);
    }
  }

  // Update view window
  if ( m_widget )
    m_widget->repaint();
}

//! Highlights Presentation for the passed Node.
//! \param theNode [in] Node to highlight.
void visu_prs_manager::Highlight(const Handle(ActAPI_INode)& theNode)
{
  Handle(ActAPI_HNodeList) aDummyList = new ActAPI_HNodeList;
  aDummyList->Append(theNode);
  this->Highlight(aDummyList);
}

//! Highlights the given set of elements in the actors corresponding
//! to the passed Node Presentation.
//! \param theNodes      [in] Data Nodes to highlight.
//! \param theActorElems [in] actors along with their cells to highlight.
//! \param theModes      [in] active selection modes.
void visu_prs_manager::Highlight(const Handle(ActAPI_HNodeList)& theNodes,
                                 const visu_actor_elem_map&      theActorElems,
                                 const int                       theModes)
{
  // Reset current selection (if any)
  m_currentSelection.PopAll(m_renderer, SelectionNature_Pick);

  // Populate Pick resulting structure to be used by Presentation's
  // highlight method
  visu_pick_result& aPickRes = m_currentSelection.ChangePickResult(SelectionNature_Pick);
  aPickRes.Clear();
  aPickRes.SetSelectionModes(theModes);
  for ( visu_actor_elem_map::Iterator it(theActorElems); it.More(); it.Next() )
    aPickRes << it.Key() << it.Value();

  // Ask involved Presentations to highlight themselves
  for ( ActAPI_NodeList::Iterator it( *theNodes.operator->() ); it.More(); it.Next() )
  {
    // Current Node
    Handle(ActAPI_INode) aNextNode = it.Value();

    // Get Presentation
    Handle(visu_prs)
      aPrs3D = Handle(visu_prs)::DownCast( this->GetPresentation(aNextNode) );

    // Push selection to renderer
    m_currentSelection.PushToRender(aPrs3D, m_renderer, SelectionNature_Pick);
  }

  // Update view window
  if ( m_widget )
    m_widget->repaint();
}

//! Cleans up detection.
void visu_prs_manager::CleanDetection()
{
  m_currentSelection.PopAll(m_renderer, SelectionNature_Detection);

  // Update view window
  if ( m_widget )
    m_widget->repaint();
}

//! \return list of the highlighted Nodes.
Handle(ActAPI_HNodeList) visu_prs_manager::GetHighlighted() const
{
  Handle(ActAPI_HNodeList) aList = new ActAPI_HNodeList();

  const visu_actual_selection::PrsSeq&
    aSeq = m_currentSelection.RenderedPresentations(SelectionNature_Pick);

  visu_actual_selection::PrsSeq::Iterator anIt(aSeq);
  for ( ; anIt.More(); anIt.Next() )
    aList->Append( anIt.Value()->GetNode() );

  return aList;
}

//! Returns data structure representing the currently selected entities in viewer.
//! \return current selection (picking and detection).
const visu_actual_selection& visu_prs_manager::GetCurrentSelection() const
{
  return m_currentSelection;
}

//-----------------------------------------------------------------------------
// Viewer-wise methods
//-----------------------------------------------------------------------------

//! Sets up a renderer for the Presentation Manager. Notice that Presentation
//! Manager is always created with default renderer. Use this method only if
//! you really need to populate your Presentation Manager with some external
//! renderer.
//! \param theRenderer [in] renderer to set.
void visu_prs_manager::SetRenderer(const vtkSmartPointer<vtkRenderer>& theRenderer)
{
  m_renderer = theRenderer;
}

//! Accessor for the renderer.
//! \return renderer.
vtkRenderer* visu_prs_manager::GetRenderer() const
{
  return m_renderer;
}

//! Accessor for the render window.
//! \return render window.
vtkRenderWindow* visu_prs_manager::GetRenderWindow() const
{
  return m_renderWindow;
}

//! Initializes rendering process for the input QVTK widget
//! and VTK render window handled by Presentation Manager.
//! \param theWidget   [in] the widget to set.
//! \param isOffscreen [in] off-screen rendering mode.
void visu_prs_manager::Initialize(QWidget* theWidget, const bool isOffscreen)
{
  // Initialize QVTK widget depending on rendering mode
  if ( isOffscreen )
  {
    m_widget = NULL;
    m_renderWindow->SetOffScreenRendering(1);
  }
  else
  {
    m_widget = new QVTKWidget(theWidget);
    m_widget->SetRenderWindow(m_renderWindow);
  }
  m_renderWindowInteractor->Initialize();
}

//! Returns QVTK widget handled by Presentation Manager.
//! \return QVTK widget.
QVTKWidget* visu_prs_manager::GetQVTKWidget() const
{
  return m_widget;
}

//! Returns default Interactor Style created by Presentation Manager.
//! \return Interactor Style instance.
vtkInteractorStyle* visu_prs_manager::GetDefaultInteractorStyle() const
{
  return m_interactorStyleTrackball;
}

//! Returns trihedron actor created by Presentation Manager.
//! \return trihedron actor.
vtkAxesActor* visu_prs_manager::GetTrihedron() const
{
  return m_trihedron;
}

//! Accessor for internal props representing the trihedron actor.
//! \return collection of props comprising the trihedron actor.
vtkSmartPointer<vtkPropCollection> visu_prs_manager::PropsByTrihedron() const
{
  vtkSmartPointer<vtkPropCollection> aRes = vtkSmartPointer<vtkPropCollection>::New();
  m_trihedron->GetActors(aRes);
  return aRes;
}

//! Modifies the size of the trihedron so that to make its size comparable to
//! the bounding box of the currently rendered scene.
void visu_prs_manager::adjustTrihedron()
{
  visu_utils::AdjustTrihedron( m_renderer, m_trihedron, this->PropsByTrihedron() );
}

//! Sets selection mode for Shape Picker.
void visu_prs_manager::actualizeShapeSelectionMode()
{
  if ( m_iSelectionModes & SelectionMode_Face )
  {
    m_shapePicker->SetSelectionMode(SM_Vertex, false);
    m_shapePicker->SetSelectionMode(SM_Edge,   false);
    m_shapePicker->SetSelectionMode(SM_Face);
  }
  if ( m_iSelectionModes & SelectionMode_Edge )
  {
    m_shapePicker->SetSelectionMode(SM_Vertex, false);
    m_shapePicker->SetSelectionMode(SM_Face,   false);
    m_shapePicker->SetSelectionMode(SM_Edge);
  }
  if ( m_iSelectionModes & SelectionMode_Vertex )
  {
    m_shapePicker->SetSelectionMode(SM_Edge, false);
    m_shapePicker->SetSelectionMode(SM_Face, false);
    m_shapePicker->SetSelectionMode(SM_Vertex);
  }
}

//! Adds callback to be activated in UpdatePresentation() method.
//! \param theEventID  [in] ID of callback action
//! \param theCallback [in] callback to add.
//! \return tag of the event.
long int visu_prs_manager::AddUpdateCallback(unsigned long theEventID, vtkCommand* theCallback)
{
  m_updateCallbackIds.Append(theEventID);
  return this->AddObserver(theEventID, theCallback);
}

//! Removes callback with the given tag.
//! \param theEventID [in] event ID on which events are invoked.
//! \param theTag     [in] tag which is assigned to particular event entity.
//! \return true in case of success, false -- otherwise.
bool visu_prs_manager::RemoveUpdateCallback(unsigned long theEventID,
                                            unsigned long theTag)
{
  if ( m_updateCallbackIds.IsEmpty() )
    return false;

  this->RemoveObserver(theTag);

  if ( !this->HasObserver(theEventID) )
    m_updateCallbackIds.Remove(theEventID);

  return true;
}
