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

#ifndef asiVisu_PrsManager_h
#define asiVisu_PrsManager_h

// asiVisu includes
#include <asiVisu_InteractorStylePick.h>
#include <asiVisu_InteractorStylePick2d.h>
#include <asiVisu_Prs.h>
#include <asiVisu_Utils.h>

// asiData includes
#include <asiData_PartNode.h>

// Active Data (API) includes
#include <ActAPI_IModel.h>
#include <ActAPI_INode.h>
#include <ActAPI_IPlotter.h>
#include <ActAPI_IProgressNotifier.h>

// VTK includes
#pragma warning(push, 0)
#include <vtkAxesActor.h>
#include <vtkCellPicker.h>
#include <vtkImageData.h>
#include <vtkObject.h>
#include <vtkPointPicker.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkWorldPointPicker.h>
#pragma warning(pop)

// QVTK includes
#pragma warning(push, 0)
#include <QVTKOpenGLNativeWidget.h>
#pragma warning(pop)

// OCCT includes
#include <NCollection_DataMap.hxx>

//-----------------------------------------------------------------------------

//! This class is designed to handle 3D representations. Besides that,
//! presentation manager controls QVTK widget along with all standard
//! visualization suite like Renderer, Render Window, etc.
//!
//! Presentation Manager is a facade under visualization. It connects
//! Data Model with viewer. It also manages interactive detection
//! and picking.
//!
//! This class inherits vtkObject in order to take advantage of VTK standard
//! observer mechanism.
class asiVisu_PrsManager : public vtkObject
{
public: // VTK macros and methods to override

  vtkTypeMacro(asiVisu_PrsManager, vtkObject);

  asiVisu_EXPORT static asiVisu_PrsManager*
    New();

  asiVisu_EXPORT void
    PrintSelf(ostream& os, vtkIndent indent);

public:

  //! Interaction dimensionality.
  enum InteractionMode
  {
    InteractionMode_3D,
    InteractionMode_2D
  };

public:

  asiVisu_EXPORT
    asiVisu_PrsManager();

  asiVisu_EXPORT
    asiVisu_PrsManager(const Handle(ActAPI_IModel)& model);

public:

  asiVisu_EXPORT void
    SetModel(const Handle(ActAPI_IModel)& model);

  asiVisu_EXPORT void
    SetDiagnosticTools(ActAPI_ProgressEntry progress,
                       ActAPI_PlotterEntry  plotter);

// Presentation management:
public:

  asiVisu_EXPORT virtual void
    Actualize(const Handle(ActAPI_INode)& node,
              const bool                  withChildren  = false,
              const bool                  doFitContents = false,
              const bool                  withRepaint   = true,
              const bool                  doAdjustTrih  = true);

//-----------------------------------------------------------------------------

  //! Re-initializes and updates the Node's Presentation.
  //! \param[in] nodeMap       map of Data Nodes to actualize Presentation for.
  //! \param[in] withChildren  indicates whether child Nodes should also be
  //!                          actualized.
  //! \param[in] doFitContents indicates whether to fit the viewport contents
  //!                          after actualization of Presentations.
  //! \param[in] withRepaint   if true, repaints view window.
  //! \param[in] doAdjustTrih  indicates whether to adjust trihedron.
  //! \param[in] autoPrs       indicates whether to automatically create
  //!                          Presentations for Nodes which are not yet
  //!                          presented.
  template <typename HColType>
  void ActualizeCol(const Handle(HColType)& nodeCol,
                    const bool              withChildren  = false,
                    const bool              doFitContents = false,
                    const bool              withRepaint   = true,
                    const bool              doAdjustTrih  = true,
                    const bool              autoPrs       = true)
  {
    if ( nodeCol.IsNull() )
      return;

    // Actualize each Node from the list individually
    for ( typename HColType::Iterator nit(*nodeCol); nit.More(); nit.Next() )
    {
      const Handle(ActAPI_INode)& node = nit.Value();
      //
      if ( node.IsNull() || !node->IsValidData() )
        return;

      // Initialize Presentation if not yet
      bool isPrsOk = false;
      if ( this->IsPresented(node) )
        isPrsOk = true;
      else if ( autoPrs )
        isPrsOk = this->SetPresentation(node);

      if ( isPrsOk )
      {
        // Clean up current selection
        m_currentSelection.PopAll(m_renderer, SelectionNature_Persistent);
        m_currentSelection.PopAll(m_renderer, SelectionNature_Detection);

        // Finally, update Presentation
        if ( this->GetPresentation(node)->IsVisible() )
        {
          this->InitPresentation(node);
          this->RenderPresentation(node); // Render before update to adjust trihedron correctly
          this->UpdatePresentation(node, false, false);
        }
        else
        {
          this->DeRenderPresentation(node);
        }
      }

      // Proceed with children if requested
      if ( withChildren )
      {
        for ( Handle(ActAPI_IChildIterator) child_it = node->GetChildIterator(); child_it->More(); child_it->Next() )
          this->Actualize(child_it->Value(), true, false, false, false);
      }
    }

    // Re-initialize all pickers (otherwise picking gives strange results...)
    this->InitializePickers<HColType>(nodeCol);

    if ( doAdjustTrih )
      asiVisu_Utils::AdjustTrihedron( m_renderer, m_trihedron, this->PropsByTrihedron() );

    if ( doFitContents )
      asiVisu_Utils::AdjustCamera( m_renderer, this->PropsByTrihedron() );

    // Update view window
    if ( withRepaint && m_widget )
      m_widget->GetRenderWindow()->Render();
  }

//-----------------------------------------------------------------------------

  asiVisu_EXPORT virtual void
    ActualizeExclusively(const Handle(ActAPI_HNodeList)& nodeList,
                         const bool                      doFitContents = true);

//-----------------------------------------------------------------------------

  asiVisu_EXPORT virtual bool
    IsPresented(const Handle(ActAPI_INode)& node);

  asiVisu_EXPORT virtual bool
    IsPresented(const ActAPI_DataObjectId& nodeId);

//-----------------------------------------------------------------------------

  asiVisu_EXPORT virtual bool
    IsPresentable(const Handle(Standard_Type)& nodeType);

//-----------------------------------------------------------------------------

  asiVisu_EXPORT virtual bool
    SetPresentation(const Handle(ActAPI_INode)& node);

//-----------------------------------------------------------------------------

  asiVisu_EXPORT virtual Handle(asiVisu_Prs)
    GetPresentation(const Handle(ActAPI_INode)& node);

  asiVisu_EXPORT virtual Handle(asiVisu_Prs)
    GetPresentation(const ActAPI_DataObjectId& nodeId);

//-----------------------------------------------------------------------------

  asiVisu_EXPORT virtual void
    InitPresentation(const Handle(ActAPI_INode)& node);

  asiVisu_EXPORT virtual void
    InitPresentation(const ActAPI_DataObjectId& nodeId);

//-----------------------------------------------------------------------------

  asiVisu_EXPORT virtual void
    RenderPresentation(const Handle(ActAPI_INode)& node);

  asiVisu_EXPORT virtual void
    RenderPresentation(const ActAPI_DataObjectId& nodeId);

//-----------------------------------------------------------------------------

  asiVisu_EXPORT virtual void
    DeRenderPresentation(const Handle(ActAPI_INode)& node);

  asiVisu_EXPORT virtual void
    DeRenderPresentation(const ActAPI_DataObjectId& nodeId);

  asiVisu_EXPORT virtual void
    DeRenderAllPresentations();

//-----------------------------------------------------------------------------

  asiVisu_EXPORT virtual void
    GarbageCollect();

//-----------------------------------------------------------------------------

  asiVisu_EXPORT virtual void
    UpdatePresentation(const Handle(ActAPI_INode)& node,
                       const bool doFitContents = true,
                       const bool doAdjustTrihedron = true);

  asiVisu_EXPORT virtual void
    UpdatePresentation(const ActAPI_DataObjectId& nodeId,
                       const bool doFitContents = true,
                       const bool doAdjustTrihedron = true);

//-----------------------------------------------------------------------------

  asiVisu_EXPORT virtual bool
    DeletePresentation(const Handle(ActAPI_INode)& node,
                       const bool                  withChildren = false);

  asiVisu_EXPORT virtual bool
    DeletePresentation(const ActAPI_DataObjectId& nodeId,
                       const bool                 withChildren = false);

  asiVisu_EXPORT virtual void
    DeleteAllPresentations();

//-----------------------------------------------------------------------------

  asiVisu_EXPORT virtual void
    InitPicker(const Handle(ActAPI_INode)& node);

  asiVisu_EXPORT virtual void
    InitPicker(const ActAPI_DataObjectId& nodeId);

//-----------------------------------------------------------------------------

// Selection management:
public:

  asiVisu_EXPORT void
    SetSelectionMode(const int mode);

  asiVisu_EXPORT int
    GetSelectionMode() const;

  asiVisu_EXPORT bool
    Pick(asiVisu_PickInput*            pickInput,
         const asiVisu_SelectionNature selNature,
         const int                     pickType);

  asiVisu_EXPORT void
    SetPickList(const Handle(ActAPI_HNodeList)& nodeList);

  asiVisu_EXPORT const Handle(ActAPI_HNodeList)&
    GetPickList() const;

  asiVisu_EXPORT void
    SetPickFromList(const bool isEnabled) const;

  asiVisu_EXPORT bool
    IsPickFromList() const;

  asiVisu_EXPORT void
    Highlight(const Handle(ActAPI_HNodeList)& nodeList);

  asiVisu_EXPORT void
    Highlight(const Handle(ActAPI_INode)& node);

  asiVisu_EXPORT void
    Highlight(const Handle(ActAPI_INode)&       node,
              const vtkSmartPointer<vtkActor>&  actor,
              const TColStd_PackedMapOfInteger& elementIds,
              const int                         modes);

  asiVisu_EXPORT void
    CleanDetection();

  asiVisu_EXPORT Handle(ActAPI_HNodeList)
    GetHighlighted() const;

  asiVisu_EXPORT const asiVisu_ActualSelection&
    GetCurrentSelection() const;

  asiVisu_EXPORT asiVisu_ActualSelection&
    ChangeCurrentSelection();

// Visualization kernel:
public:

  asiVisu_EXPORT long int
    AddUpdateCallback(unsigned long eventID,
                      vtkCommand*   pCallback);

  asiVisu_EXPORT bool
    RemoveUpdateCallback(unsigned long eventID,
                         unsigned long tag);

  asiVisu_EXPORT void
    SetRenderer(const vtkSmartPointer<vtkRenderer>& renderer);

  asiVisu_EXPORT vtkRenderer*
    GetRenderer() const;

  asiVisu_EXPORT vtkRenderWindow*
    GetRenderWindow() const;

  asiVisu_EXPORT void
    Initialize(QWidget*   pWidget,
               const bool isOffscreen = false);

  asiVisu_EXPORT void
    InitializePickers(const Handle(ActAPI_INode)& node);

//-----------------------------------------------------------------------------

  //! Initializes pickers and lets Data Model participate in such initialization.
  //! \param[in] nodeCol collection of Data Nodes which may want to perform some
  //!                    additional initializations (e.g. to construct and
  //!                    settle down accelerating structures).
  template <typename HColType>
  void InitializePickers(const Handle(HColType)& nodeCol)
  {
    // Initialize cell picker
    m_cellPicker = vtkSmartPointer<vtkCellPicker>::New();
    m_cellPicker->SetTolerance(0.005);

    // Initialize point picker
    m_pointPicker = vtkSmartPointer<vtkPointPicker>::New();
    m_pointPicker->SetTolerance(0.1);

    // Initialize world picker
    m_worldPicker = vtkSmartPointer<vtkWorldPointPicker>::New();

    if ( nodeCol.IsNull() || nodeCol->IsEmpty() )
      return;

    // Actualize each Node from the list individually
    for ( typename HColType::Iterator nit(*nodeCol); nit.More(); nit.Next() )
    {
      const Handle(ActAPI_INode)& node = nit.Value();
      //
      if ( node.IsNull() || !node->IsValidData() )
        return;

      // Finally, update Presentation
      if ( this->GetPresentation(node) )
        this->InitPicker(node);
    }
  }

//-----------------------------------------------------------------------------

  asiVisu_EXPORT QVTKOpenGLNativeWidget*
    GetQVTKWidget() const;

  asiVisu_EXPORT const vtkSmartPointer<asiVisu_InteractorStylePick>&
    GetDefaultInteractorStyle() const;

  asiVisu_EXPORT const vtkSmartPointer<asiVisu_InteractorStylePick2d>&
    GetImageInteractorStyle() const;

  asiVisu_EXPORT vtkAxesActor*
    GetTrihedron() const;

  asiVisu_EXPORT vtkSmartPointer<vtkPropCollection>
    PropsByTrihedron() const;

  asiVisu_EXPORT const vtkSmartPointer<vtkCellPicker>&
    GetCellPicker() const;

  asiVisu_EXPORT void
    DumpPNG(const char* filename) const;

  asiVisu_EXPORT vtkSmartPointer<vtkImageData>
    DumpPNG() const;

public:

  //! Sets interaction mode to either 2D or 3D.
  //! \param[in] mode interaction mode to set.
  void SetInteractionMode(const InteractionMode mode)
  {
    if ( !m_renderWindowInteractor.Get() )
      return;

    if ( mode == InteractionMode_3D )
      m_renderWindowInteractor->SetInteractorStyle(m_interactorStyleTrackball);
    if ( mode == InteractionMode_2D )
      m_renderWindowInteractor->SetInteractorStyle(m_interactorStyleImage);
  }

  //! \return 2D or 3D interaction mode depending on the current interaction style.
  InteractionMode GetInteractionMode() const
  {
    vtkInteractorObserver* pStyle = m_renderWindowInteractor->GetInteractorStyle();
    //
    if ( pStyle == m_interactorStyleImage.GetPointer() )
      return InteractionMode_2D;

    return InteractionMode_3D;
  }

// Auxiliary methods:
protected:

  asiVisu_EXPORT bool
    cellPickerResult(const asiVisu_SelectionNature           selNature,
                     const Handle(asiVisu_CellPickerResult)& pickRes);

  asiVisu_EXPORT bool
    pointPickerResult(const asiVisu_SelectionNature            selNature,
                      const Handle(asiVisu_PointPickerResult)& pickRes);

  asiVisu_EXPORT bool
    worldPickerResult(const asiVisu_SelectionNature            selNature,
                      const Handle(asiVisu_WorldPickerResult)& pickRes);

  asiVisu_EXPORT Handle(asiVisu_Prs)
    preparePickedPrs(const asiVisu_SelectionNature       selNature,
                     const Handle(asiVisu_PickerResult)& pickRes);

// Data Model:
private:

  Handle(ActAPI_IModel) m_model; //!< Data Model instance.

// Presentation management:
private:

  //! Shortcut for mapping type between Nodes and Presentations. We use
  //! Node ID as key type in order not to deal with data objects when
  //! storing/retrieving Node Information to/from pipelines.
  typedef NCollection_DataMap<ActAPI_DataObjectId, Handle(asiVisu_Prs)> TNodePrsMap;

  //! Mapping between Nodes and their Presentations.
  TNodePrsMap m_nodePresentations;

// Selection management:
private:

  //! Picker to select VTK topology (cells).
  vtkSmartPointer<vtkCellPicker> m_cellPicker;

  //! Picker to select VTK geometry (points).
  vtkSmartPointer<vtkPointPicker> m_pointPicker;

  //! Picker to select any point (not necessarily from point data).
  vtkSmartPointer<vtkWorldPointPicker> m_worldPicker;

  //! Currently selected Presentations.
  asiVisu_ActualSelection m_currentSelection;

// Visualization kernel:
private:

  //! QVTK widget.
  QVTKOpenGLNativeWidget* m_widget;

  //! Active renderer.
  vtkSmartPointer<vtkRenderer> m_renderer;

  //! Render Window.
  vtkSmartPointer<vtkRenderWindow> m_renderWindow;

  //! Render Window Interactor.
  vtkSmartPointer<vtkRenderWindowInteractor> m_renderWindowInteractor;

  //! Default Interactor Style.
  vtkSmartPointer<asiVisu_InteractorStylePick> m_interactorStyleTrackball;

  //! Interactor Style for 2D scenes.
  vtkSmartPointer<asiVisu_InteractorStylePick2d> m_interactorStyleImage;

  //! Axes actor.
  vtkSmartPointer<vtkAxesActor> m_trihedron;

  //! List of nodes allowed for picking
  Handle(ActAPI_HNodeList) m_bAllowedNodes;

  //! Custom Update Presentation callbacks.
  //! They are called in UpdatePresentation() method and
  //! provide possibility to perform custom actions when
  //! presentation manager is updating some of its presentations.
  NCollection_Sequence<unsigned long> m_updateCallbackIds;

  //! Progress notifier.
  ActAPI_ProgressEntry m_progress;

  //! Imperative plotter.
  ActAPI_PlotterEntry m_plotter;

};

#endif
