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
#include <asiVisu_PrsManager.h>

// asiVisu includes
#include <asiVisu_PartNodeInfo.h>
#include <asiVisu_Prs.h>
#include <asiVisu_Pipeline.h>
#include <asiVisu_Selection.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// VTK includes
#include <vtkCamera.h>
#include <vtkCellData.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkIdTypeArray.h>
#include <vtkInformation.h>
#include <vtkObjectFactory.h>
#include <vtkPNGWriter.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkWidgetRepresentation.h>
#include <vtkWindowToImageFilter.h>

// OCCT includes
#include <gp_Lin.hxx>
#include <NCollection_Sequence.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

#if defined COUT_DEBUG
  //! Returns address as string.
  //! \param[in] pActor pointer to extract the address for.
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
      addr_str = asiAlgo_Utils::Str::SubStr( addr_str, (int) pos, (int) (addr_str.size() - pos) );

    return addr_str;
  }
#endif

//-----------------------------------------------------------------------------

vtkStandardNewMacro(asiVisu_PrsManager)

//-----------------------------------------------------------------------------

void asiVisu_PrsManager::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//-----------------------------------------------------------------------------

//! Constructs presentation manager.
asiVisu_PrsManager::asiVisu_PrsManager()
: vtkObject (),
  m_widget  (nullptr)
{}

//-----------------------------------------------------------------------------

//! Constructs presentation manager.
//! \param[in] model Data Model instance.
asiVisu_PrsManager::asiVisu_PrsManager(const Handle(ActAPI_IModel)& model)
: vtkObject (),
  m_model   (model),
  m_widget  (nullptr)
{}

//-----------------------------------------------------------------------------

//! Sets Data Model instance.
//! \param[in] model Data Model instance to set.
void asiVisu_PrsManager::SetModel(const Handle(ActAPI_IModel)& model)
{
  m_model = model;
}

//-----------------------------------------------------------------------------

//! Sets diagnostic tools to all existing Presentations.
//! \param[in] progress progress notifier.
//! \param[in] plotter  imperative plotter.
void asiVisu_PrsManager::SetDiagnosticTools(ActAPI_ProgressEntry progress,
                                            ActAPI_PlotterEntry  plotter)
{
  m_progress = progress;
  m_plotter  = plotter;

  for ( TNodePrsMap::Iterator it(m_nodePresentations); it.More(); it.Next() )
    it.Value()->SetDiagnosticTools(progress, plotter);
}

//-----------------------------------------------------------------------------
// Presentation management
//-----------------------------------------------------------------------------

//! Re-initializes and updates the Node's Presentation.
//! \param[in] node          Data Node to actualize Presentation for.
//! \param[in] withChildren  indicates whether child Nodes should also be
//!                          actualized.
//! \param[in] doFitContents indicates whether to fit the viewport contents
//!                          after actualization of Presentations.
//! \param[in] withRepaint   if true, repaints view window.
//! \param[in] doAdjustTrih  indicates whether to adjust trihedron.
void asiVisu_PrsManager::Actualize(const Handle(ActAPI_INode)& node,
                                   const bool                  withChildren,
                                   const bool                  doFitContents,
                                   const bool                  withRepaint,
                                   const bool                  doAdjustTrih)
{
  if ( node.IsNull() )
    return;

  Handle(ActAPI_HNodeList) oneNodeList = new ActAPI_HNodeList;
  oneNodeList->Append(node);

  // Call common method
  this->ActualizeCol(oneNodeList, withChildren, doFitContents, withRepaint, doAdjustTrih);
}

//-----------------------------------------------------------------------------

//! Rebuilds the entire scene so that it fully corresponds to the given
//! collection of Nodes exclusively.
//! \param[in] nodeList      Data Nodes to visualize.
//! \param[in] doFitContents indicates whether to fit the viewport contents
//!                          after actualization of Presentations.
void asiVisu_PrsManager::ActualizeExclusively(const Handle(ActAPI_HNodeList)& nodeList,
                                              const bool                      doFitContents)
{
  if ( nodeList.IsNull() )
    return;

  // Delete all existing Presentations except the passed ones
  ActAPI_DataObjectIdList listToDel;
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
      listToDel.Append(currentID);
  }
  for ( ActAPI_DataObjectIdList::Iterator it(listToDel); it.More(); it.Next() )
    this->DeletePresentation( it.Value() );

  // Actualize: it will build all necessary Presentations
  this->ActualizeCol(nodeList, false, doFitContents, true);
}

//-----------------------------------------------------------------------------

//! Checks whether the passed Node has been already registered in the
//! Presentation Manager.
//! \param[in] node Node to check.
//! \return true/false.
bool asiVisu_PrsManager::IsPresented(const Handle(ActAPI_INode)& node)
{
  return this->IsPresented( node->GetId() );
}

//-----------------------------------------------------------------------------

//! Checks whether the passed Node has been already registered in the
//! presentation manager by the passed ID.
//! \param[in] nodeId ID of the Node to check.
//! \return true/false.
bool asiVisu_PrsManager::IsPresented(const ActAPI_DataObjectId& nodeId)
{
  return m_nodePresentations.IsBound(nodeId);
}

//-----------------------------------------------------------------------------

//! Checks whether the passed Node type is presentable or not.
//! \param[in] nodeType Node type to check.
//! \return true/false.
bool asiVisu_PrsManager::IsPresentable(const Handle(Standard_Type)& nodeType)
{
  const asiVisu_Utils::TPrsAllocMap& allocMap = asiVisu_Utils::GetAllocMap();
  const char* nodeTypeName = nodeType->Name();

  if ( !allocMap.IsBound(nodeTypeName) )
    return false; // No Presentation exists for Node

  return true;
}

//-----------------------------------------------------------------------------

//! Sets up a presentation for the passed Node. If no presentation exists,
//! this method returns false.
//! \param[in] node Node to register a Presentation for.
//! \return true if presentation exists and can be registered so, false --
//!         otherwise.
bool asiVisu_PrsManager::SetPresentation(const Handle(ActAPI_INode)& node)
{
  TCollection_AsciiString nodeID = node->GetId();

  if ( m_nodePresentations.IsBound(nodeID) )
    m_nodePresentations.UnBind(nodeID); // If you do not want us to do this,
                                        // use HasPresentation() method

  const asiVisu_Utils::TPrsAllocMap& allocMap = asiVisu_Utils::GetAllocMap();
  const char* nodeType = node->DynamicType()->Name();

  if ( !allocMap.IsBound(nodeType) )
    return false; // No Presentation exists for Node

  const asiVisu_PrsAllocator& prsAlloc = allocMap.Find(nodeType);
  Handle(asiVisu_Prs)         prs      = (*prsAlloc)(node);
  //
  prs->SetModel(m_model); // Pass Data Model instance.
  prs->SetDiagnosticTools(m_progress, m_plotter); // Pass diagnostic tools.
  //
  m_nodePresentations.Bind(nodeID, prs);

  return true;
}

//-----------------------------------------------------------------------------

//! Returns Presentation registered for the passed Node. If no Presentation
//! is registered, returns nullptr.
//! \param[in] node Node to access Presentation for.
//! \return requested Presentation or nullptr.
Handle(asiVisu_Prs)
  asiVisu_PrsManager::GetPresentation(const Handle(ActAPI_INode)& node)
{
  return this->GetPresentation( node->GetId() );
}

//-----------------------------------------------------------------------------

//! Returns Presentation registered for the passed Node. If no Presentation
//! is registered, returns nullptr.
//! \param[in] nodeId ID of the Node to access Presentation for.
//! \return requested Presentation or nullptr.
Handle(asiVisu_Prs)
  asiVisu_PrsManager::GetPresentation(const ActAPI_DataObjectId& nodeId)
{
  if ( !m_nodePresentations.IsBound(nodeId) )
    return nullptr;

  return m_nodePresentations.Find(nodeId);
}

//-----------------------------------------------------------------------------

//! Performs initialization of the Node's Presentation by rebuilding the
//! input data sets for entire set of the underlying visualization pipelines.
//! Normally, this method should be invoked once at the very first
//! visualization request for a Node, and again each time when the pipelined
//! data is changed (e.g. due to Tree Function execution).
//!
//! \param[in] node Node to (re-)initialize pipelines for.
void asiVisu_PrsManager::InitPresentation(const Handle(ActAPI_INode)& node)
{
  this->InitPresentation( node->GetId() );
}

//-----------------------------------------------------------------------------

//! Performs initialization of the Node's Presentation by rebuilding the
//! input data sets for entire set of the underlying visualization pipelines.
//! Normally, this method should be invoked once at the very first
//! visualization request for a Node, and again each time when Nodal data
//! is changed (e.g. due to Tree Function execution).
//!
//! \param[in] nodeID ID of the Node to (re-)initialize pipelines for.
void asiVisu_PrsManager::InitPresentation(const ActAPI_DataObjectId& nodeId)
{
  if ( !m_nodePresentations.IsBound(nodeId) )
  {
    vtkErrorMacro( << "Presentation does not exist" );
    return;
  }

  const Handle(asiVisu_Prs)& prs = m_nodePresentations.Find(nodeId);
  prs->InitPipelines();
}

//-----------------------------------------------------------------------------

//! Pushes the Node's Presentation to the renderer. Notice that Presentation
//! should exist and be registered by SetPresentation() method, otherwise
//! an exception is thrown.
//!
//! \param[in] node Node to render the Presentation for.
void asiVisu_PrsManager::RenderPresentation(const Handle(ActAPI_INode)& node)
{
  this->RenderPresentation( node->GetId() );
}

//-----------------------------------------------------------------------------

//! Pushes the Node's Presentation to the renderer. Notice that Presentation
//! should exist and be registered by SetPresentation() method, otherwise
//! an exception is thrown.
//!
//! \param[in] nodeId ID of the Node to render the Presentation for.
void asiVisu_PrsManager::RenderPresentation(const ActAPI_DataObjectId& nodeId)
{
  if ( !m_nodePresentations.IsBound(nodeId) )
  {
    vtkErrorMacro( << "Presentation does not exist" );
    return;
  }

  if ( !m_renderer.GetPointer()  )
  {
    vtkErrorMacro( << "Renderer is null" );
    return;
  }

  const Handle(asiVisu_Prs)& prs = m_nodePresentations.Find(nodeId);
  prs->RenderPipelines(m_renderer);
}

//-----------------------------------------------------------------------------

//! Removes the Node's Presentation from the renderer. Notice that
//! Presentation should exist and be registered by SetPresentation()
//! method, otherwise an exception is thrown.
//!
//! \param[in] node Node to de-render the Presentation for.
void asiVisu_PrsManager::DeRenderPresentation(const Handle(ActAPI_INode)& node)
{
  if ( node.IsNull() )
    return;

  this->DeRenderPresentation( node->GetId() );
}

//-----------------------------------------------------------------------------

//! Attempts to remove the Node's Presentation from the renderer.
//! \param[in] nodeId ID of the Node to de-render the Presentation for.
void asiVisu_PrsManager::DeRenderPresentation(const ActAPI_DataObjectId& nodeId)
{
  if ( !m_nodePresentations.IsBound(nodeId) )
    return; // Presentation does not exist --> be safe, do not fail here

  if ( !m_renderer.GetPointer() )
  {
    vtkErrorMacro( << "Renderer is null" );
    return;
  }

  const Handle(asiVisu_Prs)& prs = m_nodePresentations.Find(nodeId);
  prs->DeRenderPipelines(m_renderer);
}

//-----------------------------------------------------------------------------

//! Removes all existing Presentations from renderer.
void asiVisu_PrsManager::DeRenderAllPresentations()
{
  for ( TNodePrsMap::Iterator it(m_nodePresentations); it.More(); it.Next() )
   this->DeRenderPresentation( it.Key() );

  // Update view window to have it cleared for user
  if ( m_widget )
    m_widget->GetRenderWindow()->Render();
}

//-----------------------------------------------------------------------------

void asiVisu_PrsManager::GarbageCollect()
{
  if ( m_model.IsNull() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot collect garbage with uninitialized model.");
    return;
  }

  for ( TNodePrsMap::Iterator it(m_nodePresentations); it.More(); it.Next() )
  {
    const ActAPI_DataObjectId& nodeId = it.Key();

    // Check if the Node is still alive in the model.
    Handle(ActAPI_INode) N = m_model->FindNode(nodeId);
    //
    if ( N.IsNull() || !N->IsWellFormed() )
      this->DeRenderPresentation(nodeId);
  }

  if ( m_widget )
    m_widget->GetRenderWindow()->Render();
}

//-----------------------------------------------------------------------------

//! Builds (if not yet) visualization pipelines for the Node's Presentation
//! and sends VTK Update request for pipeline execution. Normally, you invoke
//! this method once you somehow affect the Node's data. Notice that a
//! Presentation should exist and be registered with SetPresentation()
//! method, otherwise an exception is thrown.
//!
//! \param[in] node              Node to update the Presentation for.
//! \param[in] doFitContents     indicates whether to adjust camera in order for
//!                              the viewer contents to fit the rendering window.
//! \param[in] doAdjustTrihedron indicates whether to automatically adjust the
//!                              global trihedron's size.
void asiVisu_PrsManager::UpdatePresentation(const Handle(ActAPI_INode)& node,
                                            const bool                  doFitContents,
                                            const bool                  doAdjustTrihedron)
{
  this->UpdatePresentation(node->GetId(), doFitContents, doAdjustTrihedron);
}

//-----------------------------------------------------------------------------

//! Builds (if not yet) visualization pipelines for the Node's Presentation
//! and sends VTK Update request for pipeline execution. Normally, you invoke
//! this method once you somehow affect the Node's data. Notice that a
//! Presentation should exist and be registered with SetPresentation()
//! method, otherwise an exception is thrown.
//!
//! \param[in] nodeId            ID of the Node to update the Presentation for.
//! \param[in] doFitContents     indicates whether to adjust camera in order for
//!                              the viewer contents to fit the rendering window.
//! \param[in] doAdjustTrihedron indicates whether to automatically adjust the
//!                              global trihedron's size.
void asiVisu_PrsManager::UpdatePresentation(const ActAPI_DataObjectId& nodeId,
                                            const bool                 doFitContents,
                                            const bool                 doAdjustTrihedron)
{
  if ( !m_nodePresentations.IsBound(nodeId) )
  {
    vtkErrorMacro( << "Presentation does not exist" );
    return;
  }

  // Update presentation
  const Handle(asiVisu_Prs)& prs = m_nodePresentations.Find(nodeId);
  prs->UpdatePipelines();

  // Update callbacks
  for ( NCollection_Sequence<unsigned long>::Iterator it(m_updateCallbackIds);
        it.More(); it.Next() )
  {
    this->InvokeEvent( it.Value() );
  }

  // Adjust trihedron
  if ( doAdjustTrihedron )
    asiVisu_Utils::AdjustTrihedron( m_renderer,
                                    m_trihedron,
                                    this->PropsByTrihedron() );

  // Adjust camera if requested
  if ( doFitContents )
    asiVisu_Utils::AdjustCamera( m_renderer, this->PropsByTrihedron() );
}

//-----------------------------------------------------------------------------

//! Unbinds the Presentation instance for the given Node. If a Presentation
//! does not exist, returns false, otherwise -- true.
//!
//! \param[in] node         Node to remove the Presentation for.
//! \param[in] withChildren whether to kill presentations for child Nodes.
//! \return true/false.
bool asiVisu_PrsManager::DeletePresentation(const Handle(ActAPI_INode)& node,
                                            const bool                  withChildren)
{
  return this->DeletePresentation(node->GetId(), withChildren);
}

//-----------------------------------------------------------------------------

//! Unbinds the Presentation instance for the given Node. If a Presentation
//! does not exist, returns false, otherwise -- true.
//!
//! \param[in] nodeId       ID of the Node to remove the Presentation for.
//! \param[in] withChildren whether to kill presentations for child Nodes.
//! \return true/false.
bool asiVisu_PrsManager::DeletePresentation(const ActAPI_DataObjectId& nodeId,
                                            const bool                 withChildren)
{
  if ( m_model.IsNull() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Data Model is null.");
    return false;
  }

  this->DeRenderPresentation(nodeId);
  m_nodePresentations.UnBind(nodeId);

  if ( withChildren )
  {
    Handle(ActAPI_INode) node = m_model->FindNode(nodeId);

    if ( !node.IsNull() )
    {
      for ( Handle(ActAPI_IChildIterator) child_it = node->GetChildIterator(); child_it->More(); child_it->Next() )
      {
        Handle(ActAPI_INode) childNode = child_it->Value();

        if ( !this->IsPresented(childNode) )
          continue;

        this->DeRenderPresentation(childNode);
        m_nodePresentations.UnBind( childNode->GetId() );
      }
    }
  }

  return true;
}

//-----------------------------------------------------------------------------

//! Unbinds all registered Presentations.
void asiVisu_PrsManager::DeleteAllPresentations()
{
  ActAPI_DataObjectIdList listToDel;
  for ( TNodePrsMap::Iterator it(m_nodePresentations); it.More(); it.Next() )
  {
    const ActAPI_DataObjectId& currentID = it.Key();
    listToDel.Append(currentID);
  }
  //
  for ( ActAPI_DataObjectIdList::Iterator it(listToDel); it.More(); it.Next() )
    this->DeletePresentation( it.Value() );
  //
  m_nodePresentations.Clear();
}

//-----------------------------------------------------------------------------

void asiVisu_PrsManager::InitPicker(const Handle(ActAPI_INode)& node)
{
  this->InitPicker( node->GetId() );
}

//-----------------------------------------------------------------------------

void asiVisu_PrsManager::InitPicker(const ActAPI_DataObjectId& nodeId)
{
  if ( !m_nodePresentations.IsBound(nodeId) )
  {
    vtkErrorMacro( << "Presentation does not exist" );
    return;
  }

  // Ask presentation to initialize picker
  const Handle(asiVisu_Prs)& prs = m_nodePresentations.Find(nodeId);
  //
  prs->InitializePicker(m_cellPicker);
}

//-----------------------------------------------------------------------------
// Selection management
//-----------------------------------------------------------------------------

//! Sets selection modes.
//! \param[in] mode selection modes to set.
void asiVisu_PrsManager::SetSelectionMode(const int mode)
{
  m_currentSelection.SetSelectionModes(mode);
  m_currentSelection.PopAll(m_renderer, SelectionNature_Persistent);
  m_currentSelection.PopAll(m_renderer, SelectionNature_Detection);

  if ( m_widget )
    m_widget->GetRenderWindow()->Render();
}

//-----------------------------------------------------------------------------

//! Returns currently active selection modes.
//! \return selection modes.
int asiVisu_PrsManager::GetSelectionMode() const
{
  return m_currentSelection.GetSelectionModes();
}

//-----------------------------------------------------------------------------

//! Performs picking or detection by the passed display coordinates (pick input).
//!
//! \param[in] pickInput picking input data.
//! \param[in] selNature selection nature (picking or detection).
//! \param[in] pickType  type of picker to use.
//!
//! \return true if selection result is not empty, false -- otherwise.
bool
  asiVisu_PrsManager::Pick(asiVisu_PickInput*            pickInput,
                           const asiVisu_SelectionNature selNature,
                           const int                     pickType)
{
  // Selection is disabled.
  if ( m_currentSelection.GetSelectionModes() & SelectionMode_None )
    return false;

  /* ===================
   *  Some preparations
   * =================== */

  const int  xStart     = pickInput->Start.x();
  const int  yStart     = pickInput->Start.y();
  const bool isMultiple = pickInput->IsMultiple;

  // Reset current selection (if any).
  if ( !isMultiple )
    m_currentSelection.PopAll(m_renderer, selNature);

  /* =====================
   *  Perform actual pick
   * ===================== */

  // Use try-catch as sometimes VTK 7.1 crashes if cell locators are used.
  try
  {
    if ( pickType & PickerType_Cell ) // Cell (VTK topology) picker.
    {
      m_cellPicker->Pick(xStart, yStart, 0, m_renderer);
    }
    if ( pickType & PickerType_Point ) // Point (VTK geometry) picker.
    {
      m_pointPicker->Pick(xStart, yStart, 0, m_renderer);
    }
    if ( pickType & PickerType_World ) // World (any visible point) picker.
    {
      m_worldPicker->Pick(xStart, yStart, 0, m_renderer);
    }
  }
  catch ( ... )
  {
    std::cerr << "VTK picker raised an exception" << std::endl;
    return false;
  }

  //-------------//
  // Cell picker
  //-------------//

  bool isPicked = false;
  Handle(asiVisu_PickerResult) lastPickerRes;

  if ( pickType & PickerType_Cell )
  {
    isPicked = this->cellPickerResult( selNature,
                                       m_currentSelection.GetCellPickerResult(selNature) );
    //
    if ( isPicked )
      lastPickerRes = m_currentSelection.GetCellPickerResult(selNature);
  }

  //--------------//
  // Point picker
  //--------------//

  if ( !isPicked && (pickType & PickerType_Point) )
  {
    isPicked = this->pointPickerResult( selNature,
                                        m_currentSelection.GetPointPickerResult(selNature) );
    //
    if ( isPicked )
      lastPickerRes = m_currentSelection.GetPointPickerResult(selNature);
  }

  //--------------//
  // World picker
  //--------------//

  if ( !isPicked && (pickType & PickerType_World) )
  {
    isPicked = this->worldPickerResult( selNature,
                                        m_currentSelection.GetWorldPickerResult(selNature) );
    //
    if ( isPicked )
      lastPickerRes = m_currentSelection.GetWorldPickerResult(selNature);
  }

  /* ======================================
   *  Ask Presentation to highlight itself
   * ====================================== */

  // When picking erase detection at first in order to prevent blinking.
  if ( selNature == SelectionNature_Persistent )
    m_currentSelection.PopAll(m_renderer, SelectionNature_Detection);

  Handle(asiVisu_Prs) prs3D = this->preparePickedPrs(selNature, lastPickerRes);

  // Push selection to renderer.
  m_currentSelection.PushToRender(prs3D, m_renderer, selNature);

  // Update view window.
  if ( m_widget )
    m_widget->GetRenderWindow()->Render();

  return true;
}

//-----------------------------------------------------------------------------

//! Set list of Nodes allowed for picking. This can be used to limit
//! the picking "perimeter".
//! \param[in] nodeList list of Nodes allowed for picking.
void asiVisu_PrsManager::SetPickList(const Handle(ActAPI_HNodeList)& nodeList)
{
  m_bAllowedNodes = nodeList;
  m_cellPicker->InitializePickList();

  if ( m_bAllowedNodes.IsNull() )
    return;

  ActAPI_NodeList::Iterator listIt( *m_bAllowedNodes.operator->() );
  for ( ; listIt.More(); listIt.Next() )
  {
    const Handle(ActAPI_INode)& node = listIt.Value();
    //
    if ( !this->IsPresented(node) && !this->SetPresentation(node) )
      continue;

    Handle(asiVisu_Prs) prs = this->GetPresentation(node);
    //
    if ( prs.IsNull() )
      continue;

    Handle(asiVisu_HPipelineList) pipelines = prs->GetPipelineList();
    //
    if ( pipelines.IsNull() )
      continue;

    asiVisu_PipelineList::Iterator pit( *pipelines.operator->() );
    for ( ; pit.More(); pit.Next() )
    {
      const Handle(asiVisu_Pipeline)& pipeline = pit.Value();
      //
      if ( pipeline.IsNull() )
        continue;

      m_cellPicker->AddPickList( pipeline->Actor() );
    }
  }
}

//-----------------------------------------------------------------------------

//! Returns the list of Nodes allowed for picking when picking
//! from list is enabled.
//! \return list of Nodes allowed for picking.
const Handle(ActAPI_HNodeList)& asiVisu_PrsManager::GetPickList() const
{
  return m_bAllowedNodes;
}

//-----------------------------------------------------------------------------

//! Enables or disables picking from list.
//! \param[in] isEnabled true/false.
void asiVisu_PrsManager::SetPickFromList(const bool isEnabled) const
{
  m_cellPicker->SetPickFromList(isEnabled ? 1 : 0);
}

//-----------------------------------------------------------------------------

//! Checks whether the picking from list is enabled or not.
//! \return true/false.
bool asiVisu_PrsManager::IsPickFromList() const
{
  return m_cellPicker->GetPickFromList() ? true : false;
}

//-----------------------------------------------------------------------------

//! Highlights Presentation for the passed Nodes.
//! \param[in] nodeList Nodes to highlight.
void asiVisu_PrsManager::Highlight(const Handle(ActAPI_HNodeList)& nodeList)
{
  // Reset current selection (if any).
  m_currentSelection.PopAll(m_renderer, SelectionNature_Persistent);
  //
  m_currentSelection.GetCellPickerResult(SelectionNature_Persistent)->Clear();
  m_currentSelection.GetCellPickerResult(SelectionNature_Persistent)->SetSelectionModes(SelectionMode_Workpiece);
  //
  m_currentSelection.GetPointPickerResult(SelectionNature_Persistent)->Clear();
  m_currentSelection.GetPointPickerResult(SelectionNature_Persistent)->SetSelectionModes(SelectionMode_Workpiece);
  //
  m_currentSelection.GetWorldPickerResult(SelectionNature_Persistent)->Clear();
  m_currentSelection.GetWorldPickerResult(SelectionNature_Persistent)->SetSelectionModes(SelectionMode_Workpiece);

  if ( !(m_currentSelection.GetSelectionModes() & SelectionMode_None) )
  {
    // Iterate over the passed Data Nodes accessing & highlighting
    // their Presentations
    for ( ActAPI_NodeList::Iterator nit( *nodeList.operator->() ); nit.More(); nit.Next() )
    {
      const Handle(ActAPI_INode)& node = nit.Value();

      // Access Presentation
      Handle(asiVisu_Prs) prs = this->GetPresentation(node);
      //
      if ( prs.IsNull() || !prs->IsVisible() )
        continue; // Node does not have Presentation to highlight

      // Push selection to renderer
      m_currentSelection.PushToRender(prs, m_renderer, SelectionNature_Persistent);
    }
  }

  // Update view window
  if ( m_widget )
    m_widget->GetRenderWindow()->Render();
}

//-----------------------------------------------------------------------------

//! Highlights Presentation for the passed Node.
//! \param[in] node Node to highlight.
void asiVisu_PrsManager::Highlight(const Handle(ActAPI_INode)& node)
{
  Handle(ActAPI_HNodeList) dummyList = new ActAPI_HNodeList;
  dummyList->Append(node);
  this->Highlight(dummyList);
}

//-----------------------------------------------------------------------------

//! This method stands for programmatic API of selection.
//! \param[in] node       Data Node to highlight.
//! \param[in] actor      actor to pick.
//! \param[in] elementIds element IDs to pick.
//! \param[in] modes      active selection modes.
void asiVisu_PrsManager::Highlight(const Handle(ActAPI_INode)&       node,
                                   const vtkSmartPointer<vtkActor>&  actor,
                                   const TColStd_PackedMapOfInteger& elementIds,
                                   const int                         modes)
{
  // Get Presentation for the target Node. The idea is to ask Presentation
  // to highlight itself as it "knows better" how to do that ;)
  Handle(asiVisu_Prs) prs3D = this->GetPresentation(node);
  //
  if ( prs3D.IsNull() )
  {
    vtkErrorMacro( << "No presentation to highlight" );
    return;
  }

  // Reset current selection (if any)
  m_currentSelection.PopAll(m_renderer, SelectionNature_Persistent);

  // Populate Pick resulting structure to be used by Presentation's
  // highlight method.
  {
    const Handle(asiVisu_CellPickerResult)&
      pickRes = m_currentSelection.GetCellPickerResult(SelectionNature_Persistent);
    //
    pickRes->Clear();
    pickRes->SetSelectionModes(modes);
    pickRes->SetPickedActor(actor);
    pickRes->SetPickedElementIds(elementIds);
  }
  //
  {
    const Handle(asiVisu_PointPickerResult)&
      pickRes = m_currentSelection.GetPointPickerResult(SelectionNature_Persistent);
    //
    pickRes->Clear();
    pickRes->SetSelectionModes(modes);
    pickRes->SetPickedActor(actor);
  }
  //
  {
    const Handle(asiVisu_WorldPickerResult)&
      pickRes = m_currentSelection.GetWorldPickerResult(SelectionNature_Persistent);
    //
    pickRes->Clear();
    pickRes->SetSelectionModes(modes);
    pickRes->SetPickedActor(actor);
  }

  // Push selection to renderer
  m_currentSelection.PushToRender(prs3D, m_renderer, SelectionNature_Persistent);

  // Update view window
  if ( m_widget )
    m_widget->GetRenderWindow()->Render();
}

//-----------------------------------------------------------------------------

//! Cleans up detection.
void asiVisu_PrsManager::CleanDetection()
{
  m_currentSelection.PopAll(m_renderer, SelectionNature_Detection);

  // Update view window
  if ( m_widget )
    m_widget->GetRenderWindow()->Render();
}

//-----------------------------------------------------------------------------

//! \return list of the highlighted Nodes.
Handle(ActAPI_HNodeList) asiVisu_PrsManager::GetHighlighted() const
{
  Handle(ActAPI_HNodeList) list = new ActAPI_HNodeList();

  const std::vector<Handle(asiVisu_Prs)>&
    seq = m_currentSelection.GetRenderedPresentations(SelectionNature_Persistent);

  for ( size_t k = 0; k < seq.size(); ++k )
    list->Append( seq[k]->GetNode() );

  return list;
}

//-----------------------------------------------------------------------------

//! Returns data structure representing the currently selected entities in viewer.
//! \return current selection (picking and detection).
const asiVisu_ActualSelection& asiVisu_PrsManager::GetCurrentSelection() const
{
  return m_currentSelection;
}

//-----------------------------------------------------------------------------

//! Returns non-const reference to the data structure representing the
//! currently selected entities in viewer.
//! \return current selection (picking and detection).
asiVisu_ActualSelection& asiVisu_PrsManager::ChangeCurrentSelection()
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
//!
//! \param[in] renderer renderer to set.
void asiVisu_PrsManager::SetRenderer(const vtkSmartPointer<vtkRenderer>& renderer)
{
  m_renderer = renderer;
}

//-----------------------------------------------------------------------------

//! Accessor for the renderer.
//! \return renderer.
vtkRenderer* asiVisu_PrsManager::GetRenderer() const
{
  return m_renderer;
}

//-----------------------------------------------------------------------------

//! Accessor for the render window.
//! \return render window.
vtkRenderWindow* asiVisu_PrsManager::GetRenderWindow() const
{
  return m_renderWindow;
}

//-----------------------------------------------------------------------------

#include <QApplication>
#include <QHBoxLayout>
#include <QDockWidget>
#include <vtkSphereSource.h>

//! Initializes rendering process for the input QVTK widget
//! and VTK render window handled by Presentation Manager.
//!
//! \param[in] pWidget     widget to set.
//! \param[in] isOffscreen off-screen rendering mode.
void asiVisu_PrsManager::Initialize(QWidget* pWidget, const bool isOffscreen)
{
  if ( !isOffscreen )
  {
    // Initialize widget.
    m_widget = new QVTKOpenGLNativeWidget(pWidget);

    // Initialize render window.
    m_renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    //
    m_renderWindow->SetMultiSamples(16);
    m_renderWindow->SetLineSmoothing(true);
    m_renderWindow->SetPolygonSmoothing(false);
    //
    m_widget->SetRenderWindow(m_renderWindow);

    // Initialize renderer.
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    m_renderer->GetActiveCamera()->ParallelProjectionOn();
    m_renderer->LightFollowCameraOn();
    m_renderer->TwoSidedLightingOff();
    m_renderer->SetBackground(0.15, 0.15, 0.15);
    //
    m_widget->GetRenderWindow()->AddRenderer(m_renderer);

    // Initialize Interactor Style instance for normal operation mode.
    m_interactorStyleTrackball = vtkSmartPointer<asiVisu_InteractorStylePick>::New();

    // Initialize Interactor Style instance for 2D scenes.
    m_interactorStyleImage = vtkSmartPointer<asiVisu_InteractorStylePick2d>::New();

    // Initialize Render Window Interactor.
    m_renderWindowInteractor = m_renderWindow->GetInteractor();
    m_renderWindowInteractor->SetInteractorStyle(m_interactorStyleImage);

   /* =========
    *  Pickers.
    * ========= */

    // Initialize employed pickers.
    this->InitializePickers( Handle(ActAPI_INode)() );

    // Set default selection mode.
    m_currentSelection.SetSelectionModes(SelectionMode_None);

   /* ===========
    *  Trihedron.
    * =========== */

    // Initialize trihedron.
    m_trihedron = vtkSmartPointer<vtkAxesActor>::New();
    m_trihedron->SetAxisLabels(0);
    m_trihedron->SetConeRadius(0);
    m_renderer->AddActor(m_trihedron);
  }
  else // Offscreen rendering.
  {
    m_widget = nullptr; // No widgets.

    // A renderer and render window.
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    //
    m_renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    //
    m_renderWindow->SetOffScreenRendering(1);
    m_renderWindow->AddRenderer(m_renderer);
  }
}

//-----------------------------------------------------------------------------

//! Initializes pickers and lets Data Model participate in such initialization.
//! \param[in] nodeList list of Data Nodes which may want to perform some
//!                     additional initializations (e.g. to construct and
//!                     settle down accelerating structures).
void asiVisu_PrsManager::InitializePickers(const Handle(ActAPI_INode)& node)
{
  Handle(ActAPI_HNodeList) oneNodeList = new ActAPI_HNodeList;
  oneNodeList->Append(node);
  //
  this->InitializePickers(oneNodeList);
}

//-----------------------------------------------------------------------------

//! Returns QVTK widget handled by Presentation Manager.
//! \return QVTK widget.
QVTKOpenGLNativeWidget* asiVisu_PrsManager::GetQVTKWidget() const
{
  return m_widget;
}

//-----------------------------------------------------------------------------

//! Returns default Interactor Style created by Presentation Manager.
//! \return Interactor Style instance.
const vtkSmartPointer<asiVisu_InteractorStylePick>&
  asiVisu_PrsManager::GetDefaultInteractorStyle() const
{
  return m_interactorStyleTrackball;
}

//-----------------------------------------------------------------------------

//! Returns image Interactor Style created by Presentation Manager.
//! \return Interactor Style instance.
const vtkSmartPointer<asiVisu_InteractorStylePick2d>&
  asiVisu_PrsManager::GetImageInteractorStyle() const
{
  return m_interactorStyleImage;
}

//-----------------------------------------------------------------------------

//! Returns trihedron actor created by Presentation Manager.
//! \return trihedron actor.
vtkAxesActor* asiVisu_PrsManager::GetTrihedron() const
{
  return m_trihedron;
}

//-----------------------------------------------------------------------------

//! Accessor for internal props representing the trihedron actor.
//! \return collection of props comprising the trihedron actor.
vtkSmartPointer<vtkPropCollection> asiVisu_PrsManager::PropsByTrihedron() const
{
  vtkSmartPointer<vtkPropCollection> res = vtkSmartPointer<vtkPropCollection>::New();

  if ( m_trihedron.Get() )
    m_trihedron->GetActors(res);

  return res;
}

//-----------------------------------------------------------------------------

//! Accessor for cell picker.
//! \return cell picker.
const vtkSmartPointer<vtkCellPicker>& asiVisu_PrsManager::GetCellPicker() const
{
  return m_cellPicker;
}

//-----------------------------------------------------------------------------

//! Dumps the raster image to a PNG file.
//! \param[in] filename target filename.
void asiVisu_PrsManager::DumpPNG(const char* filename) const
{
  asiVisu_Utils::WritePNG(this->DumpPNG(), filename);
}

//-----------------------------------------------------------------------------

//! Dumps the raster image to a PNG format in memory.
//! \return dumped image data.
vtkSmartPointer<vtkImageData> asiVisu_PrsManager::DumpPNG() const
{
  return asiVisu_Utils::GetImage(m_renderWindow);
}

//-----------------------------------------------------------------------------

//! Adds callback to be activated in UpdatePresentation() method.
//!
//! \param[in] eventID   ID of callback action
//! \param[in] pCallback callback to add.
//! \return tag of the event.
long int asiVisu_PrsManager::AddUpdateCallback(unsigned long eventID,
                                               vtkCommand*   pCallback)
{
  m_updateCallbackIds.Append(eventID);
  return this->AddObserver(eventID, pCallback);
}

//-----------------------------------------------------------------------------

//! Removes callback with the given tag.
//! \param[in] eventID event ID on which events are invoked.
//! \param[in] tag     tag which is assigned to particular event entity.
//! \return true in case of success, false -- otherwise.
bool asiVisu_PrsManager::RemoveUpdateCallback(unsigned long eventID,
                                              unsigned long tag)
{
  if ( m_updateCallbackIds.IsEmpty() )
    return false;

  this->RemoveObserver(tag);

  if ( !this->HasObserver(eventID) )
    m_updateCallbackIds.Remove(eventID);

  return true;
}

//-----------------------------------------------------------------------------

bool
  asiVisu_PrsManager::cellPickerResult(const asiVisu_SelectionNature           selNature,
                                       const Handle(asiVisu_CellPickerResult)& pickRes)
{
  vtkIdType cellId = m_cellPicker->GetCellId();
  vtkIdType gid    = -1;
  vtkIdType pid    = -1;
  //
  if ( cellId != -1 )
  {
#if defined COUT_DEBUG
    std::cout << "Picked Cell ID = " << cellId << std::endl;
#endif

    // Global IDs.
    vtkSmartPointer<vtkIdTypeArray>
      gids = vtkIdTypeArray::SafeDownCast( m_cellPicker->GetDataSet()->GetCellData()->GetGlobalIds() );
    //
    if ( gids )
    {
      gid = gids->GetValue(cellId);
#if defined COUT_DEBUG
      std::cout << "Picked GID (Global ID) = " << gid << std::endl;
#endif
    }

    // Pedigree IDs.
    vtkSmartPointer<vtkIdTypeArray>
      pids = vtkIdTypeArray::SafeDownCast( m_cellPicker->GetDataSet()->GetCellData()->GetPedigreeIds() );
    //
    if ( pids )
    {
      pid = pids->GetValue(cellId);
#if defined COUT_DEBUG
      std::cout << "Picked PID (Pedigree ID) = " << pid << std::endl;
#endif
    }
  }

  // Get picked position and store it in the result.
  double pickedPos[3];
  m_cellPicker->GetPickPosition(pickedPos);
  pickRes->SetPickedPos(pickedPos[0], pickedPos[1], pickedPos[2]);

  // Get picked actor.
  vtkActor* pickedActor = m_cellPicker->GetActor();
  //
  if ( !pickedActor )
  {
#if defined COUT_DEBUG
    std::cout << "No picked actor" << std::endl;
#endif
    m_widget->GetRenderWindow()->Render();
    return false; // Nothing has been picked.
  }

  // Check consistency of the picked actor with the already
  // recorded one (if any).
  if ( pickRes->GetPickedActor().GetPointer() &&
       pickRes->GetPickedActor() != pickedActor )
  {
    vtkErrorMacro( << "Selection logic error: attempt to accumulate cell IDs from different actors to a single picking result" );
    return false;
  }
  if ( !pickRes->GetPickedActor().GetPointer() )
    pickRes->SetPickedActor(pickedActor);

  // Push ID to result: ID can be either a pedigree ID or a global ID
  // depending on the context.
  if ( pid != -1 )
  {
    // Let the user unpick the already selected elements.
    if ( (selNature == SelectionNature_Persistent) && pickRes->GetPickedElementIds().Contains(pid) )
      pickRes->RemovePickedElementId(pid);
    else
      pickRes->AddPickedElementId(pid);
  }
  else if ( gid != -1 )
  {
    // Let the user unpick the already selected elements.
    if ( (selNature == SelectionNature_Persistent) && pickRes->GetPickedElementIds().Contains(gid) )
      pickRes->RemovePickedElementId(gid);
    else
      pickRes->AddPickedElementId(gid);
  }

  // Let the user unpick the already selected elements.
  if ( (selNature == SelectionNature_Persistent) && pickRes->GetPickedCellIds().Contains(cellId) )
    pickRes->RemovePickedCellId(cellId);
  else
    pickRes->AddPickedCellId(cellId);

  if ( selNature == SelectionNature_Detection )
    this->InvokeEvent(EVENT_DETECT_CELL, &*pickRes);
  else
    this->InvokeEvent(EVENT_SELECT_CELL, &*pickRes);

  return true;
}

//-----------------------------------------------------------------------------

bool
  asiVisu_PrsManager::pointPickerResult(const asiVisu_SelectionNature            selNature,
                                        const Handle(asiVisu_PointPickerResult)& pickRes)
{
  vtkIdType pointId = m_pointPicker->GetPointId();

  // Get picked position and store it in the result.
  double pickedPos[3];
  m_pointPicker->GetPickPosition(pickedPos);
  pickRes->SetPickedPos(pickedPos[0], pickedPos[1], pickedPos[2]);

  // Get picked actor.
  vtkActor* pickedActor = m_pointPicker->GetActor();
  //
  if ( !pickedActor )
  {
    m_widget->GetRenderWindow()->Render();
    return false; // Nothing has been picked.
  }

  // Check consistency of the picked actor with the already recorded one (if any).
  if ( pickRes->GetPickedActor().GetPointer() &&
       pickRes->GetPickedActor() != pickedActor )
  {
    vtkErrorMacro( << "Selection logic error: attempt to accumulate cell IDs from different actors to a single picking result" );
    return false;
  }

  // Push ID to result.
  if ( !pickRes->GetPickedActor().GetPointer() )
    pickRes->SetPickedActor(pickedActor);
  //
  pickRes->AddPickedPointId(pointId);

  if ( pointId != -1 )
  {
#if defined COUT_DEBUG
    std::cout << "Picked Point ID = " << pointId << std::endl;
#endif

    double coord[3] = {0.0, 0.0, 0.0};
    m_pointPicker->GetDataSet()->GetPoint(pointId, coord);

#if defined COUT_DEBUG
    std::cout << "Picked point position: ("
              << coord[0] << ", " << coord[1] << ", " << coord[2]
              << ")" << std::endl;
#endif

    if ( selNature == SelectionNature_Detection )
      this->InvokeEvent(EVENT_DETECT_POINT, &*pickRes);
    else
      this->InvokeEvent(EVENT_SELECT_POINT, &*pickRes);
  }

  return true;
}

//-----------------------------------------------------------------------------

bool
  asiVisu_PrsManager::worldPickerResult(const asiVisu_SelectionNature            selNature,
                                        const Handle(asiVisu_WorldPickerResult)& pickRes)
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

  // Store picked position in the result.
  pickRes->SetPickedPos(coord[0], coord[1], coord[2]);

  vtkCamera* camera  = m_renderer->GetActiveCamera();
  double*    dirProj = camera->GetDirectionOfProjection();

  gp_Pnt cPos(coord[0], coord[1], coord[2]);
  gp_Pnt cOrigin = cPos.XYZ() - gp_XYZ(dirProj[0], dirProj[1], dirProj[2])*300;
  gp_Lin pickRay( cOrigin, gp_Dir(dirProj[0], dirProj[1], dirProj[2]) );

#if defined COUT_DEBUG
  std::cout << "Picked world position: ("
            << coord[0] << ", " << coord[1] << ", " << coord[2]
            << ")" << std::endl;
#endif

  if ( selNature == SelectionNature_Detection )
    this->InvokeEvent(EVENT_DETECT_WORLD_POINT, &pickRay);
  else
    this->InvokeEvent(EVENT_SELECT_WORLD_POINT, &pickRay);

  return true;
}

//-----------------------------------------------------------------------------

Handle(asiVisu_Prs)
  asiVisu_PrsManager::preparePickedPrs(const asiVisu_SelectionNature       selNature,
                                       const Handle(asiVisu_PickerResult)& pickRes)
{
  if ( pickRes.IsNull() )
    return nullptr;

  // Retrieve the corresponding Presentation by the data object ID.
  asiVisu_NodeInfo* nodeInfo = asiVisu_NodeInfo::Retrieve( pickRes->GetPickedActor() );
  //
  if ( !nodeInfo )
  {
#if defined COUT_DEBUG
    std::cout << "NULL Node information key: highlighting is not possible." << std::endl;
#endif
    return NULL;
  }
  //
  const ActAPI_DataObjectId& nodeId = nodeInfo->GetNodeId();
  //
  m_currentSelection.AddNodeId(selNature, nodeId);

  // Eliminate elements of improper type for the picking result. This functionality
  // is available for master Nodes only.
  if ( !m_model.IsNull() &&
        dynamic_cast<asiVisu_PartNodeInfo*>(nodeInfo) &&
        pickRes->IsKind( STANDARD_TYPE(asiVisu_CellPickerResult) ) )
  {
    Handle(asiVisu_CellPickerResult)
      cellPickRes = Handle(asiVisu_CellPickerResult)::DownCast(pickRes);

    // Get Part Node.
    asiVisu_PartNodeInfo* partInfo = dynamic_cast<asiVisu_PartNodeInfo*>(nodeInfo);
    //
    Handle(asiData_PartNode)
      partNode = Handle(asiData_PartNode)::DownCast( m_model->FindNode( partInfo->GetNodeId() ) );

    // Get AAG.
    Handle(asiAlgo_AAG) aag = partNode->GetAAG();
    //
    if ( !aag.IsNull() )
    {
      // Filter sub-shape IDs.
      TColStd_PackedMapOfInteger subShapes2Highlight;
      const TColStd_PackedMapOfInteger& pickedSubshapes = cellPickRes->GetPickedElementIds();
      //
      for ( TColStd_MapIteratorOfPackedMapOfInteger mit(pickedSubshapes); mit.More(); mit.Next() )
      {
        if ( pickRes->IsSelectionFace() )
        {
          if ( aag->RequestMapOfSubShapes().Extent() >= mit.Key() ) // To avoid crashes in some circumstances.
          {
            TopoDS_Shape shapeFromAAG = aag->RequestMapOfSubShapes().FindKey( mit.Key() );
            //
            if ( !shapeFromAAG.IsNull() && shapeFromAAG.ShapeType() == TopAbs_FACE )
              subShapes2Highlight.Add( mit.Key() );
          }
        }
        else if ( pickRes->IsSelectionEdge() )
        {
          if ( aag->RequestMapOfSubShapes().Extent() >= mit.Key() ) // To avoid crashes in some circumstances.
          {
            TopoDS_Shape shapeFromAAG = aag->RequestMapOfSubShapes().FindKey( mit.Key() );
            //
            if ( !shapeFromAAG.IsNull() && shapeFromAAG.ShapeType() == TopAbs_EDGE )
              subShapes2Highlight.Add( mit.Key() );
          }
        }
        else if ( pickRes->IsSelectionVertex() )
        {
          if ( aag->RequestMapOfSubShapes().Extent() >= mit.Key() ) // To avoid crashes in some circumstances.
          {
            TopoDS_Shape shapeFromAAG = aag->RequestMapOfSubShapes().FindKey( mit.Key() );
            //
            if ( !shapeFromAAG.IsNull() && shapeFromAAG.ShapeType() == TopAbs_VERTEX )
              subShapes2Highlight.Add( mit.Key() );
          }
        }
      }

      // Re-initialize IDs of the picked elements.
      cellPickRes->SetPickedElementIds(subShapes2Highlight);
    }
  }

  // Get presentation.
  Handle(asiVisu_Prs) prs3D = this->GetPresentation(nodeId);
  //
  return prs3D;
}
