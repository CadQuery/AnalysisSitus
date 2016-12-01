//-----------------------------------------------------------------------------
// Created on: 25 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_PrsManager.h>

// Visualization includes
#include <asiVisu_NodeInfo.h>
#include <asiVisu_Prs.h>
#include <asiVisu_Pipeline.h>
#include <asiVisu_Selection.h>
#include <asiVisu_ShapeDataSource.h>
#include <asiVisu_Utils.h>

// Qr includes
#include <QrCore.h>

// VTK includes
#include <vtkCamera.h>
#include <vtkCellData.h>
#include <vtkIdTypeArray.h>
#include <vtkImageData.h>
#include <vtkInformation.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkTexturedButtonRepresentation2D.h>
#include <vtkWidgetRepresentation.h>

// VIS includes
#pragma warning(push, 0)
#include <IVtkTools_ShapeObject.hxx>
#pragma warning(pop)

// OCCT includes
#include <NCollection_Sequence.hxx>
#include <Standard_ProgramError.hxx>

#define COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

//! Returns address as string.
//! \param pActor [in] pointer to extract the address for.
//! \return address as string.
static std::string ActorAddr(const vtkActor* pActor)
{
  std::string addr_str;
  std::ostringstream ost;
  ost << pActor;
  addr_str = ost.str();

  size_t pos = 0;
  while ( addr_str[pos] == '0' )
    pos++;

  if ( pos )
    addr_str = QrCore::substr( addr_str, (int) pos, (int) (addr_str.size() - pos) );

  return addr_str;
}

//-----------------------------------------------------------------------------

void asiVisu_PrsManager::PlaceButton(vtkButtonWidget* pButton,
                                     vtkRenderer*     pRenderer)
{
  // Place the widget. Must be done after a render so that the viewport is
  // defined. Here the widget placement is in normalized display coordinates
  vtkSmartPointer<vtkCoordinate> upperRight = vtkSmartPointer<vtkCoordinate>::New();
  upperRight->SetCoordinateSystemToNormalizedDisplay();
  upperRight->SetValue(1.0, 1.0);

  const double displaySize[2] = { upperRight->GetComputedDisplayValue(pRenderer)[0],
                                  upperRight->GetComputedDisplayValue(pRenderer)[1] };

  double bds[6];
  const double size = 25.0;
  bds[0] = 3;
  bds[1] = bds[0] + size;
  bds[2] = displaySize[1] - size - 3;
  bds[3] = bds[2] + size;
  bds[4] = bds[5] = 0.0;

  // Scale to 1, default is .5
  pButton->GetRepresentation()->SetPlaceFactor(1);
  pButton->GetRepresentation()->PlaceWidget(bds);
}

//-----------------------------------------------------------------------------

void asiVisu_PrsManager::CreateImage(vtkSmartPointer<vtkImageData> image,
                                     unsigned char*                color1,
                                     unsigned char*                color2)
{
  // Specify the size of the image data
  image->SetDimensions(10, 10, 1);
  image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);
  int* dims = image->GetDimensions();

  // Fill the image
  for ( int y = 0; y < dims[1]; ++y )
  {
    for ( int x = 0; x < dims[0]; ++x )
    {
      unsigned char*
        pixel = static_cast<unsigned char*>( image->GetScalarPointer(x, y, 0) );
      //
      if ( x < 5 )
      {
        pixel[0] = color1[0];
        pixel[1] = color1[1];
        pixel[2] = color1[2];
      }
      else
      {
        pixel[0] = color2[0];
        pixel[1] = color2[1];
        pixel[2] = color2[2];
      }
    }
  }
}

//-----------------------------------------------------------------------------

vtkStandardNewMacro(asiVisu_PrsManager);

//-----------------------------------------------------------------------------

void asiVisu_PrsManager::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//-----------------------------------------------------------------------------

//! Constructs presentation manager setting up the underlying QVTK widget
//! as a child for the passed arbitrary Qt widget. It allows creation of
//! presentation manager providing basic visualization suite in any window.
//!
//! \param theParent   [in] parent Qt widget for underlying QVTK widget.
//! \param isOffscreen [in] indicates whether presentation manager should
//!                         operate in off-screen rendering mode. In that
//!                         case QVTK widget is not created.
asiVisu_PrsManager::asiVisu_PrsManager() : vtkObject(), m_widget(NULL)
{
  // Initialize renderer
  m_renderer = vtkSmartPointer<vtkRenderer>::New();
  m_renderer->GetActiveCamera()->ParallelProjectionOn();
  m_renderer->LightFollowCameraOn();
  m_renderer->TwoSidedLightingOn();

  // Set background color
  m_renderer->SetBackground(0.15, 0.15, 0.15);

  // Initialize employed pickers
  this->InitializePickers();

  // Set default selection mode
  m_iSelectionModes = SelectionMode_None;

  // Initialize Render Window
  m_renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  m_renderWindow->AddRenderer(m_renderer);
  m_renderWindow->SetMultiSamples(8);

  // Initialize Interactor Style instance for normal operation mode
  m_interactorStyleTrackball = vtkSmartPointer<asiVisu_InteractorStylePick>::New();

  // Initialize Interactor Style instance for 2D scenes
  m_interactorStyleImage = vtkSmartPointer<asiVisu_InteractorStylePick2d>::New();

  // Initialize Render Window Interactor
  m_renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  m_renderWindowInteractor->SetRenderWindow(m_renderWindow);
  m_renderWindowInteractor->SetInteractorStyle(m_interactorStyleImage);

  /* =======================
   *  Button to toggle axes
   * ======================= */

  // Initialize trihedron
  m_trihedron = vtkSmartPointer<vtkAxesActor>::New();
  m_trihedron->SetAxisLabels(0);
  m_trihedron->SetConeRadius(0);
  m_renderer->AddActor(m_trihedron);

  // Button to switch between visualization modes
  m_axesButton = vtkSmartPointer<vtkButtonWidget>::New();
  m_axesCallback = vtkSmartPointer<asiVisu_AxesBtnCallback>::New();
  //
  m_axesCallback->SetAxesActor(m_trihedron);
  m_axesCallback->SetRenderer(m_renderer);
  m_axesButton->AddObserver(vtkCommand::StateChangedEvent, m_axesCallback);

  // Create images for textures
  vtkSmartPointer<vtkImageData> image1 = vtkSmartPointer<vtkImageData>::New();
  vtkSmartPointer<vtkImageData> image2 = vtkSmartPointer<vtkImageData>::New();
  vtkSmartPointer<vtkImageData> image3 = vtkSmartPointer<vtkImageData>::New();
  vtkSmartPointer<vtkImageData> image4 = vtkSmartPointer<vtkImageData>::New();
  //
  unsigned char color1[3] = {255, 0,   0};
  unsigned char color2[3] = {0,   255, 0};
  unsigned char color3[3] = {0,   0,   255};
  unsigned char color4[3] = {0,   255, 255};
  //
  CreateImage(image1, color1, color1);
  CreateImage(image2, color2, color2);
  CreateImage(image3, color3, color3);
  CreateImage(image4, color4, color4);

  // Create the widget and its representation
  vtkSmartPointer<vtkTexturedButtonRepresentation2D>
    buttonRepresentation = vtkSmartPointer<vtkTexturedButtonRepresentation2D>::New();
  //
  buttonRepresentation->SetNumberOfStates(4);
  buttonRepresentation->SetButtonTexture(0, image1);
  buttonRepresentation->SetButtonTexture(1, image2);
  buttonRepresentation->SetButtonTexture(2, image3);
  buttonRepresentation->SetButtonTexture(3, image4);

  m_axesButton->SetInteractor( m_renderer->GetRenderWindow()->GetInteractor() );
  m_axesButton->SetRepresentation(buttonRepresentation);
  //
  m_axesButton->On();
  //
  PlaceButton(m_axesButton, m_renderer);
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
void asiVisu_PrsManager::Actualize(const Handle(ActAPI_INode)& theNode,
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
    asiVisu_Utils::AdjustCamera( m_renderer, this->PropsByTrihedron() );

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
void asiVisu_PrsManager::Actualize(const Handle(ActAPI_HNodeList)& theNodeList,
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
    asiVisu_Utils::AdjustCamera( m_renderer, this->PropsByTrihedron() );

  // Update view window
  if ( withRepaint && m_widget )
    m_widget->repaint();
}

//! Rebuilds the entire scene so that it fully corresponds to the given
//! collection of Nodes exclusively.
//! \param nodeList      [in] Data Nodes to visualize.
//! \param doFitContents [in] indicates whether to fit the viewport contents
//!                           after actualization of Presentations.
void asiVisu_PrsManager::ActualizeExclusively(const Handle(ActAPI_HNodeList)& nodeList,
                                              const bool                      doFitContents)
{
  if ( nodeList.IsNull() )
    return;

  // Delete all existing Presentations except the passed ones
  ActAPI_DataObjectIdList aListToDel;
  for ( TNodePrsMap::Iterator it(m_nodePresentations); it.More(); it.Next() )
  {
    ActAPI_DataObjectId currentID = it.Key();
    bool                isInList  = false;

    for ( ActAPI_NodeList::Iterator nit( *nodeList.operator->() ); nit.More(); nit.Next() )
    {
      ActAPI_DataObjectId passedID = nit.Value()->GetId();
      if ( IsEqual(passedID, currentID) )
      {
        isInList = true;
        break;
      }
    }

    if ( !isInList )
      aListToDel.Append(currentID);
  }
  for ( ActAPI_DataObjectIdList::Iterator it(aListToDel); it.More(); it.Next() )
    this->DeletePresentation( it.Value() );

  // Actualize: it will build all necessary Presentations
  this->Actualize(nodeList, false, doFitContents, true);
}

//! Checks whether the passed Node has been already registered in the
//! Presentation Manager.
//! \param theNode [in] Node to check.
//! \return true/false.
bool asiVisu_PrsManager::IsPresented(const Handle(ActAPI_INode)& theNode)
{
  return this->IsPresented( theNode->GetId() );
}

//! Checks whether the passed Node has been already registered in the
//! presentation manager by the passed ID.
//! \param theNodeID [in] ID of the Node to check.
//! \return true/false.
bool asiVisu_PrsManager::IsPresented(const ActAPI_DataObjectId& theNodeId)
{
  return m_nodePresentations.IsBound(theNodeId);
}

//! Sets up a presentation for the passed Node. If no presentation exists,
//! this method returns false.
//! \param theNode [in] Node to register a Presentation for.
//! \return true if presentation exists and can be registered so, false --
//!         otherwise.
bool asiVisu_PrsManager::SetPresentation(const Handle(ActAPI_INode)& theNode)
{
  TCollection_AsciiString aNodeID = theNode->GetId();

  if ( m_nodePresentations.IsBound(aNodeID) )
    m_nodePresentations.UnBind(aNodeID); // If you do not want us to do this,
                                         // use HasPresentation() method

  const asiVisu_Utils::TPrsAllocMap& anAllocMap = asiVisu_Utils::GetAllocMap();
  Standard_CString aNodeType = theNode->DynamicType()->Name();

  if ( !anAllocMap.IsBound(aNodeType) )
    return false; // No Presentation exists for Node

  const asiVisu_PrsAllocator& aPrsAlloc = anAllocMap.Find(aNodeType);
  m_nodePresentations.Bind( aNodeID, (*aPrsAlloc)(theNode) );

  return true;
}

//! Returns Presentation registered for the passed Node. If no Presentation
//! is registered, returns NULL.
//! \param theNode [in] Node to access Presentation for.
//! \return requested Presentation or NULL.
Handle(asiVisu_Prs)
  asiVisu_PrsManager::GetPresentation(const Handle(ActAPI_INode)& theNode)
{
  return this->GetPresentation( theNode->GetId() );
}

//! Returns Presentation registered for the passed Node. If no Presentation
//! is registered, returns NULL.
//! \param theNodeId [in] ID of the Node to access Presentation for.
//! \return requested Presentation or NULL.
Handle(asiVisu_Prs)
  asiVisu_PrsManager::GetPresentation(const ActAPI_DataObjectId& theNodeId)
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
void asiVisu_PrsManager::InitPresentation(const Handle(ActAPI_INode)& theNode)
{
  this->InitPresentation( theNode->GetId() );
}

//! Performs initialization of the Node's Presentation by rebuilding the
//! input data sets for entire set of the underlying visualization pipelines.
//! Normally, this method should be invoked once at the very first
//! visualization request for a Node, and again each time when Nodal data
//! is changed (e.g. due to Tree Function execution).
//! \param theNodeID [in] ID of the Node to (re-)initialize pipelines for.
void asiVisu_PrsManager::InitPresentation(const ActAPI_DataObjectId& theNodeId)
{
  if ( !m_nodePresentations.IsBound(theNodeId) )
    Standard_ProgramError::Raise("Presentation does not exist");

  const Handle(asiVisu_Prs)& aPrs = m_nodePresentations.Find(theNodeId);
  aPrs->InitPipelines();
}

//! Pushes the Node's Presentation to the renderer. Notice that Presentation
//! should exist and be registered by SetPresentation() method, otherwise
//! an exception is thrown.
//! \param theNode [in] Node to render the Presentation for.
void asiVisu_PrsManager::RenderPresentation(const Handle(ActAPI_INode)& theNode)
{
  this->RenderPresentation( theNode->GetId() );
}

//! Pushes the Node's Presentation to the renderer. Notice that Presentation
//! should exist and be registered by SetPresentation() method, otherwise
//! an exception is thrown.
//! \param theNodeId [in] ID of the Node to render the Presentation for.
void asiVisu_PrsManager::RenderPresentation(const ActAPI_DataObjectId& theNodeId)
{
  if ( !m_nodePresentations.IsBound(theNodeId) )
    Standard_ProgramError::Raise("Presentation does not exist");

  if ( m_renderer.GetPointer() == NULL )
    Standard_ProgramError::Raise("Renderer is NULL");

  Handle(asiVisu_Prs) aPrs = m_nodePresentations.Find(theNodeId);
  aPrs->RenderPipelines(m_renderer);
}

//! Removes the Node's Presentation from the renderer. Notice that
//! Presentation should exist and be registered by SetPresentation()
//! method, otherwise an exception is thrown.
//! \param theNode [in] Node to de-render the Presentation for.
void asiVisu_PrsManager::DeRenderPresentation(const Handle(ActAPI_INode)& theNode)
{
  this->DeRenderPresentation( theNode->GetId() );
}

//! Attempts to remove the Node's Presentation from the renderer.
//! \param theNodeId [in] ID of the Node to de-render the Presentation for.
void asiVisu_PrsManager::DeRenderPresentation(const ActAPI_DataObjectId& theNodeId)
{
  if ( !m_nodePresentations.IsBound(theNodeId) )
    return; // Presentation does not exist --> be safe, do not fail here

  if ( m_renderer.GetPointer() == NULL )
    Standard_ProgramError::Raise("Renderer is NULL");

  Handle(asiVisu_Prs) aPrs = m_nodePresentations.Find(theNodeId);
  aPrs->DeRenderPipelines(m_renderer);
}

//! Removes all existing Presentations from renderer.
void asiVisu_PrsManager::DeRenderAllPresentations()
{
  for ( TNodePrsMap::Iterator it(m_nodePresentations); it.More(); it.Next() )
   this->DeRenderPresentation( it.Key() );

  // Update view window to have it cleared for user
  if ( m_widget )
    m_widget->repaint();
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
void asiVisu_PrsManager::UpdatePresentation(const Handle(ActAPI_INode)& theNode,
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
void asiVisu_PrsManager::UpdatePresentation(const ActAPI_DataObjectId& theNodeId,
                                            const bool                 doFitContents)
{
  if ( !m_nodePresentations.IsBound(theNodeId) )
    Standard_ProgramError::Raise("Presentation does not exist");

  // Update presentation
  const Handle(asiVisu_Prs)& aPrs = m_nodePresentations.Find(theNodeId);
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
    asiVisu_Utils::AdjustCamera( m_renderer, this->PropsByTrihedron() );
}

//! Unbinds the Presentation instance for the given Node. If a Presentation
//! does not exist, returns false, otherwise -- true.
//! \param theNode [in] Node to remove the Presentation for.
//! \return true/false.
bool asiVisu_PrsManager::DeletePresentation(const Handle(ActAPI_INode)& theNode)
{
  return this->DeletePresentation( theNode->GetId() );
}

//! Unbinds the Presentation instance for the given Node. If a Presentation
//! does not exist, returns false, otherwise -- true.
//! \param theNodeId [in] ID of the Node to remove the Presentation for.
//! \return true/false.
bool asiVisu_PrsManager::DeletePresentation(const ActAPI_DataObjectId& theNodeId)
{
  if ( !this->IsPresented(theNodeId) )
    return false;

  this->DeRenderPresentation(theNodeId);
  m_nodePresentations.UnBind(theNodeId);
  return true;
}

//! Unbinds all registered Presentations.
void asiVisu_PrsManager::DeleteAllPresentations()
{
  ActAPI_DataObjectIdList aListToDel;
  for ( TNodePrsMap::Iterator it(m_nodePresentations); it.More(); it.Next() )
  {
    ActAPI_DataObjectId aCurrentID = it.Key();
    aListToDel.Append(aCurrentID);
  }
  for ( ActAPI_DataObjectIdList::Iterator it(aListToDel); it.More(); it.Next() )
    this->DeletePresentation( it.Value() );
  //
  m_nodePresentations.Clear();
}

//-----------------------------------------------------------------------------
// Selection management
//-----------------------------------------------------------------------------

//! Sets selection modes.
//! \param theMode [in] selection modes to set.
void asiVisu_PrsManager::SetSelectionMode(const int theMode)
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
int asiVisu_PrsManager::GetSelectionMode() const
{
  return m_iSelectionModes;
}

//! Performs picking or detection by the passed display coordinates.
//! \param thePickInput [in] picking input data.
//! \param theSelNature [in] selection nature (picking or detection).
//! \param thePickType  [in] type of picker to use.
//! \return list of affected Data Node IDs.
ActAPI_DataObjectIdList
  asiVisu_PrsManager::Pick(asiVisu_PickInput*            thePickInput,
                           const asiVisu_SelectionNature theSelNature,
                           const asiVisu_PickType        thePickType)
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
  asiVisu_PickResult& aPickRes     = m_currentSelection.ChangePickResult(theSelNature);
  aPickRes.SetSelectionModes(m_iSelectionModes);

  if ( m_iSelectionModes & SelectionMode_Workpiece ) // Non-partial selection
  {
    if ( thePickType == PickType_Cell )
      m_cellPicker->Pick(XStart, YStart, 0, m_renderer);
    else if ( thePickType == PickType_Point )
      m_pointPicker->Pick(XStart, YStart, 0, m_renderer);
    else if ( thePickType == PickType_World )
      m_worldPicker->Pick(XStart, YStart, 0, m_renderer);

    // Extract cell ID
    if ( thePickType == PickType_Cell )
    {
      vtkIdType cell_id = m_cellPicker->GetCellId();
      vtkIdType gid = -1, pid = -1;
      //
      if ( cell_id != -1 )
      {
        std::cout << "Picked Cell ID = " << cell_id << std::endl;

        // Global IDs
        vtkSmartPointer<vtkIdTypeArray>
          gids = vtkIdTypeArray::SafeDownCast( m_cellPicker->GetDataSet()->GetCellData()->GetGlobalIds() );
        //
        if ( gids )
        {
          gid = gids->GetValue(cell_id);
          std::cout << "Picked GID (Global ID) = " << gid << std::endl;
        }

        // Pedigree IDs
        vtkSmartPointer<vtkIdTypeArray>
          pids = vtkIdTypeArray::SafeDownCast( m_cellPicker->GetDataSet()->GetCellData()->GetPedigreeIds() );
        //
        if ( pids )
        {
          pid = pids->GetValue(cell_id);
          std::cout << "Picked PID (Pedigree ID) = " << pid << std::endl;
        }
      }

      // Get picked actor
      aPickedActor = m_cellPicker->GetActor();
      //
      if ( !aPickedActor )
      {
        m_widget->repaint();
        return aResult; // Nothing has been picked
      }

      // Push ID to result
      if ( pid != -1 )
        aPickRes << aPickedActor << pid;
      else if ( gid != -1 )
        aPickRes << aPickedActor << gid;
      else
        aPickRes << aPickedActor << cell_id;
    }
    else if ( thePickType == PickType_Point )
    {
      vtkIdType point_id = m_pointPicker->GetPointId();
      //
      if ( point_id != -1 )
      {
        std::cout << "Picked Point ID = " << point_id << std::endl;
      }

      // Get picked actor
      aPickedActor = m_pointPicker->GetActor();
      //
      if ( !aPickedActor )
      {
        m_widget->repaint();
        return aResult; // Nothing has been picked
      }

      // Push ID to result
      aPickRes << aPickedActor << point_id;
    }
    else // World picker
    {
      // Picked position returned by this kind of "world" picker is very
      // inaccurate for the needs of computational geometry. This is because
      // the world picker is based on depth buffer analysis. The latter
      // makes it very efficient, but not very precise. As a result, if
      // we simply use the picked position for the intersection testing (we
      // do that in order to know which face corresponds to the picked
      // position), the test will fail in many cases as it wouldn't
      // find even a bounding box containing such an imprecise point.
      // In order to fix the deal, we take that inaccurate picked position
      // and reconstruct a ray in the direction of projection (this is
      // the camera's property). Then we have to intersect our geometry
      // with the ray, thus obtaining the precise position. If we intersect
      // the accurate B-Rep primitive, then the solution is ideal in terms
      // of achievable accuracy. Another option is to intersect the ray
      // with visualization facets which is faster but less accurate.

      double coord[3];
      m_worldPicker->GetPickPosition(coord);

      vtkCamera* camera = m_renderer->GetActiveCamera();
      double* dirProj = camera->GetDirectionOfProjection();

      gp_Pnt cPos(coord[0], coord[1], coord[2]);
      gp_Pnt cOrigin = cPos.XYZ() - gp_XYZ(dirProj[0], dirProj[1], dirProj[2])*300;

      gp_Lin pickRay( cOrigin, gp_Dir(dirProj[0], dirProj[1], dirProj[2]) );

      std::cout << "Picked world position: ("
                << coord[0] << ", " << coord[1] << ", " << coord[2]
                << ")" << std::endl;

      this->InvokeEvent(EVENT_PICK_WORLD_POINT, &pickRay);
    }

  }
  else // Partial selection: for topological shapes only
  {
    this->actualizeShapeSelectionMode();

    // PICK (!!!)
    m_shapePicker->Pick(XStart, YStart, 0);

    // Traversing results
    asiVisu_NodeInfo*                   aNodeInfo         = NULL;
    vtkSmartPointer<vtkActorCollection> anActorCollection = m_shapePicker->GetPickedActors();
    //
    if ( anActorCollection && anActorCollection->GetNumberOfItems() > 0 )
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
          std::cout << "--------------------------------------------------------------" << std::endl;
          std::cout << "Actor: "          << ActorAddr(aPickedActor)                    << std::endl;
          std::cout << "Sub-shape ID: "   << sIt.Value()                                << std::endl;
          std::cout << "Sub-shape type: " << aSubShape.TShape()->DynamicType()->Name()  << std::endl;
#endif
          aPickRes << sIt.Value();
        }

        aNodeInfo = asiVisu_NodeInfo::Retrieve(aPickedActor);
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
  asiVisu_NodeInfo* nodeInfo = asiVisu_NodeInfo::Retrieve(aPickedActor);
  if ( !nodeInfo )
    return aResult;
  //
  ActAPI_DataObjectId aNodeId = nodeInfo->GetNodeId();

  if ( m_iSelectionModes & SelectionMode_Workpiece )
    aResult.Append(aNodeId);

  Handle(asiVisu_Prs) aPrs3D = this->GetPresentation(aNodeId);
  //
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
void asiVisu_PrsManager::SetPickList(const Handle(ActAPI_HNodeList)& theNodeList)
{
  m_bAllowedNodes = theNodeList;
  m_cellPicker->InitializePickList();

  if ( m_bAllowedNodes.IsNull() )
    return;

  ActAPI_NodeList::Iterator aListIt( *m_bAllowedNodes.operator->() );
  for ( ; aListIt.More(); aListIt.Next() )
  {
    Handle(asiVisu_Prs) aPrs = GetPresentation(aListIt.Value());
    if ( aPrs.IsNull() )
      continue;

    Handle(asiVisu_HPipelineList) aPipelines = aPrs->GetPipelineList();
    if ( aPipelines.IsNull() )
      continue;

    asiVisu_PipelineList::Iterator aPipeIt( *aPipelines.operator->() );
    for ( ; aPipeIt.More(); aPipeIt.Next() )
    {
      Handle(asiVisu_Pipeline) aPipeline = aPipeIt.Value();
      //
      if ( aPipeline.IsNull() )
        continue;

      m_cellPicker->AddPickList( aPipeline->Actor() );
    }
  }
}

//! Returns the list of Nodes allowed for picking when picking
//! from list is enabled.
//! \return list of Nodes allowed for picking.
const Handle(ActAPI_HNodeList)& asiVisu_PrsManager::GetPickList() const
{
  return m_bAllowedNodes;
}

//! Enables or disables picking from list.
//! \param isEnabled [in] true/false.
void asiVisu_PrsManager::SetPickFromList(const bool isEnabled) const
{
  m_cellPicker->SetPickFromList(isEnabled ? 1 : 0);
}

//! Checks whether the picking from list is enabled or not.
//! \return true/false.
bool asiVisu_PrsManager::IsPickFromList() const
{
  return m_cellPicker->GetPickFromList() ? true : false;
}

//! Highlights Presentation for the passed Nodes.
//! \param theNodes [in] Nodes to highlight.
void asiVisu_PrsManager::Highlight(const Handle(ActAPI_HNodeList)& theNodes)
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
      Handle(asiVisu_Prs) aPrs3D = this->GetPresentation(aNode);
      //
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
void asiVisu_PrsManager::Highlight(const Handle(ActAPI_INode)& theNode)
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
void asiVisu_PrsManager::Highlight(const Handle(ActAPI_HNodeList)& theNodes,
                                   const asiVisu_ActorElemMap&      theActorElems,
                                   const int                       theModes)
{
  // Reset current selection (if any)
  m_currentSelection.PopAll(m_renderer, SelectionNature_Pick);

  // Populate Pick resulting structure to be used by Presentation's
  // highlight method
  asiVisu_PickResult& aPickRes = m_currentSelection.ChangePickResult(SelectionNature_Pick);
  aPickRes.Clear();
  aPickRes.SetSelectionModes(theModes);
  for ( asiVisu_ActorElemMap::Iterator it(theActorElems); it.More(); it.Next() )
    aPickRes << it.Key() << it.Value();

  // Ask involved Presentations to highlight themselves
  for ( ActAPI_NodeList::Iterator it( *theNodes.operator->() ); it.More(); it.Next() )
  {
    // Current Node
    Handle(ActAPI_INode) aNextNode = it.Value();

    // Get Presentation
    Handle(asiVisu_Prs) aPrs3D = this->GetPresentation(aNextNode);

    // Push selection to renderer
    m_currentSelection.PushToRender(aPrs3D, m_renderer, SelectionNature_Pick);
  }

  // Update view window
  if ( m_widget )
    m_widget->repaint();
}

//! Cleans up detection.
void asiVisu_PrsManager::CleanDetection()
{
  m_currentSelection.PopAll(m_renderer, SelectionNature_Detection);

  // Update view window
  if ( m_widget )
    m_widget->repaint();
}

//! \return list of the highlighted Nodes.
Handle(ActAPI_HNodeList) asiVisu_PrsManager::GetHighlighted() const
{
  Handle(ActAPI_HNodeList) aList = new ActAPI_HNodeList();

  const asiVisu_ActualSelection::PrsSeq&
    aSeq = m_currentSelection.RenderedPresentations(SelectionNature_Pick);

  asiVisu_ActualSelection::PrsSeq::Iterator anIt(aSeq);
  for ( ; anIt.More(); anIt.Next() )
    aList->Append( anIt.Value()->GetNode() );

  return aList;
}

//! Returns data structure representing the currently selected entities in viewer.
//! \return current selection (picking and detection).
const asiVisu_ActualSelection& asiVisu_PrsManager::GetCurrentSelection() const
{
  return m_currentSelection;
}

//-----------------------------------------------------------------------------
// Viewer-wise methods
//-----------------------------------------------------------------------------

//! Sets up a renderer for the Presentation Manager. Notice that Presentation
//! Manager is always created with default renderer. Use this method only if
//! you really need to populate your presentation manager with some external
//! renderer.
//! \param theRenderer [in] renderer to set.
void asiVisu_PrsManager::SetRenderer(const vtkSmartPointer<vtkRenderer>& theRenderer)
{
  m_renderer = theRenderer;
}

//! Accessor for the renderer.
//! \return renderer.
vtkRenderer* asiVisu_PrsManager::GetRenderer() const
{
  return m_renderer;
}

//! Accessor for the render window.
//! \return render window.
vtkRenderWindow* asiVisu_PrsManager::GetRenderWindow() const
{
  return m_renderWindow;
}

//! Initializes rendering process for the input QVTK widget
//! and VTK render window handled by Presentation Manager.
//! \param theWidget   [in] the widget to set.
//! \param isOffscreen [in] off-screen rendering mode.
void asiVisu_PrsManager::Initialize(QWidget* theWidget, const bool isOffscreen)
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

//! The following logic is normally a part of ctor, but we have it as a
//! distinct method... Shame of us... But the problem is that OCCT picker
//! seems to be not that good to survive without re-initialization from
//! time to time...
void asiVisu_PrsManager::InitializePickers()
{
  // Initialize cell picker
  m_cellPicker = vtkSmartPointer<vtkCellPicker>::New();
  m_cellPicker->SetTolerance(0.015);

  // Initialize point picker
  m_pointPicker = vtkSmartPointer<vtkPointPicker>::New();
  m_pointPicker->SetTolerance(0.005);

  // Initialize world picker
  m_worldPicker = vtkSmartPointer<vtkWorldPointPicker>::New();

  // Create a picker for OCCT shapes
  m_shapePicker = vtkSmartPointer<IVtkTools_ShapePicker>::New();
  m_shapePicker->SetTolerance(0.25);
  m_shapePicker->SetRenderer(m_renderer);
}

//! Returns QVTK widget handled by Presentation Manager.
//! \return QVTK widget.
QVTKWidget* asiVisu_PrsManager::GetQVTKWidget() const
{
  return m_widget;
}

//! Returns default Interactor Style created by Presentation Manager.
//! \return Interactor Style instance.
vtkInteractorStyle* asiVisu_PrsManager::GetDefaultInteractorStyle() const
{
  return m_interactorStyleTrackball;
}

//! Returns image Interactor Style created by Presentation Manager.
//! \return Interactor Style instance.
vtkInteractorStyle* asiVisu_PrsManager::GetImageInteractorStyle() const
{
  return m_interactorStyleImage;
}

//! Returns trihedron actor created by Presentation Manager.
//! \return trihedron actor.
vtkAxesActor* asiVisu_PrsManager::GetTrihedron() const
{
  return m_trihedron;
}

//! Accessor for internal props representing the trihedron actor.
//! \return collection of props comprising the trihedron actor.
vtkSmartPointer<vtkPropCollection> asiVisu_PrsManager::PropsByTrihedron() const
{
  vtkSmartPointer<vtkPropCollection> aRes = vtkSmartPointer<vtkPropCollection>::New();
  m_trihedron->GetActors(aRes);
  return aRes;
}

//! Accessor for cell picker.
//! \return cell picker.
const vtkSmartPointer<vtkCellPicker>& asiVisu_PrsManager::GetCellPicker() const
{
  return m_cellPicker;
}

//! Accessor for shape picker.
//! \return shape picker.
const vtkSmartPointer<IVtkTools_ShapePicker>& asiVisu_PrsManager::GetShapePicker() const
{
  return m_shapePicker;
}

//! Modifies the size of the trihedron so that to make its size comparable to
//! the bounding box of the currently rendered scene.
void asiVisu_PrsManager::adjustTrihedron()
{
  asiVisu_Utils::AdjustTrihedron( m_renderer, m_trihedron, this->PropsByTrihedron() );
}

//! Sets selection mode for Shape Picker.
void asiVisu_PrsManager::actualizeShapeSelectionMode()
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
long int asiVisu_PrsManager::AddUpdateCallback(unsigned long theEventID,
                                               vtkCommand* theCallback)
{
  m_updateCallbackIds.Append(theEventID);
  return this->AddObserver(theEventID, theCallback);
}

//! Removes callback with the given tag.
//! \param theEventID [in] event ID on which events are invoked.
//! \param theTag     [in] tag which is assigned to particular event entity.
//! \return true in case of success, false -- otherwise.
bool asiVisu_PrsManager::RemoveUpdateCallback(unsigned long theEventID,
                                              unsigned long theTag)
{
  if ( m_updateCallbackIds.IsEmpty() )
    return false;

  this->RemoveObserver(theTag);

  if ( !this->HasObserver(theEventID) )
    m_updateCallbackIds.Remove(theEventID);

  return true;
}
