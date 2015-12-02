//-----------------------------------------------------------------------------
// Created on: 25 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef visu_prs_manager_h
#define visu_prs_manager_h

// Visualization includes
#include <visu_prs.h>

// Active Data (API) includes
#include <ActAPI_IModel.h>
#include <ActAPI_INode.h>

// VTK includes
#include <vtkAxesActor.h>
#include <vtkCellPicker.h>
#include <vtkCubeAxesActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObject.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

// QVTK includes
#pragma warning(push, 0)
#include <QVTKWidget.h>
#pragma warning(pop)

// VIS includes
#pragma warning(push, 0)
#include <IVtkTools_ShapePicker.hxx>
#pragma warning(pop)

// OCCT includes
#include <NCollection_DataMap.hxx>

//! This class is designed to handle 3D representations. Besides that,
//! Presentation Manager controls QVTK widget along with all standard
//! visualization suite like Renderer, Render Window, etc.
//!
//! Presentation Manager is a facade under visualization. It connects
//! Data Model with viewer. Moreover, it manages interactive detection
//! and picking.
//!
//! This class inherits vtkObject in order to take advantage of VTK standard
//! observer mechanism.
class visu_prs_manager : public vtkObject
{
public: // VTK macros and methods to override

  vtkTypeMacro(visu_prs_manager, vtkObject);

  ASitus_EXPORT static visu_prs_manager* New();
  ASitus_EXPORT void PrintSelf(ostream& os, vtkIndent indent);

public:

  ASitus_EXPORT visu_prs_manager();

// Presentation management:
public:

  ASitus_EXPORT virtual void
    Actualize(const Handle(ActAPI_INode)& theNode,
              const bool                  withChildren  = false,
              const bool                  doFitContents = false,
              const bool                  withRepaint   = true);

  ASitus_EXPORT virtual void
    Actualize(const Handle(ActAPI_HNodeList)& theNodeList,
              const bool                      withChildren  = false,
              const bool                      doFitContents = false,
              const bool                      withRepaint   = true);

  ASitus_EXPORT virtual void
    ActualizeExclusively(const Handle(ActAPI_HNodeList)& theNodeList,
                         const bool                      doFitContents = true);

//-----------------------------------------------------------------------------

  ASitus_EXPORT virtual bool
    IsPresented(const Handle(ActAPI_INode)& theNode);

  ASitus_EXPORT virtual bool
    IsPresented(const ActAPI_DataObjectId& theNodeId);

//-----------------------------------------------------------------------------

  ASitus_EXPORT virtual bool
    SetPresentation(const Handle(ActAPI_INode)& theNode);

//-----------------------------------------------------------------------------

  ASitus_EXPORT virtual Handle(visu_prs)
    GetPresentation(const Handle(ActAPI_INode)& theNode);

  ASitus_EXPORT virtual Handle(visu_prs)
    GetPresentation(const ActAPI_DataObjectId& theNodeId);

//-----------------------------------------------------------------------------

  ASitus_EXPORT virtual void
    InitPresentation(const Handle(ActAPI_INode)& theNode);

  ASitus_EXPORT virtual void
    InitPresentation(const ActAPI_DataObjectId& theNodeId);

//-----------------------------------------------------------------------------

  ASitus_EXPORT virtual void
    RenderPresentation(const Handle(ActAPI_INode)& theNode);

  ASitus_EXPORT virtual void
    RenderPresentation(const ActAPI_DataObjectId& theNodeId);

//-----------------------------------------------------------------------------

  ASitus_EXPORT virtual void
    DeRenderPresentation(const Handle(ActAPI_INode)& theNode);

  ASitus_EXPORT virtual void
    DeRenderPresentation(const ActAPI_DataObjectId& theNodeId);

  ASitus_EXPORT virtual void
    DeRenderAllPresentations();

//-----------------------------------------------------------------------------

  ASitus_EXPORT virtual void
    UpdatePresentation(const Handle(ActAPI_INode)& theNode,
                       const bool doFitContents = true);

  ASitus_EXPORT virtual void
    UpdatePresentation(const ActAPI_DataObjectId& theNodeId,
                       const bool doFitContents = true);

//-----------------------------------------------------------------------------

  ASitus_EXPORT virtual bool
    DeletePresentation(const Handle(ActAPI_INode)& theNode);

  ASitus_EXPORT virtual bool
    DeletePresentation(const ActAPI_DataObjectId& theNodeId);

  ASitus_EXPORT virtual void
    DeleteAllPresentations();

//-----------------------------------------------------------------------------

// Selection management:
public:

  ASitus_EXPORT void
    SetSelectionMode(const int theMode);

  ASitus_EXPORT int
    GetSelectionMode() const;

  ASitus_EXPORT ActAPI_DataObjectIdList
    Pick(visu_pick_input*            thePickInput,
         const visu_selection_nature theSelNature);

  ASitus_EXPORT void
    SetPickList(const Handle(ActAPI_HNodeList)& theNodeList);

  ASitus_EXPORT Handle(ActAPI_HNodeList)
    GetPickList() const;

  ASitus_EXPORT void
    SetPickFromList(const bool isEnabled) const;

  ASitus_EXPORT bool
    IsPickFromList() const;

  ASitus_EXPORT void
    Highlight(const Handle(ActAPI_HNodeList)& theNodes);

  ASitus_EXPORT void
    Highlight(const Handle(ActAPI_INode)& theNode);

  ASitus_EXPORT void
    Highlight(const Handle(ActAPI_HNodeList)& theNodes,
              const visu_actor_elem_map&      theActorElems,
              const int                       theModes);

  ASitus_EXPORT void
    CleanDetection();

  ASitus_EXPORT Handle(ActAPI_HNodeList)
    GetHighlighted() const;

  ASitus_EXPORT const visu_actual_selection&
    GetCurrentSelection() const;

// Visualization kernel:
public:

  ASitus_EXPORT long int
    AddUpdateCallback(unsigned long theEventID,
                      vtkCommand*   theCallback);

  ASitus_EXPORT bool
    RemoveUpdateCallback(unsigned long theEventID,
                         unsigned long theTag);

  ASitus_EXPORT void
    SetRenderer(const vtkSmartPointer<vtkRenderer>& theRenderer);

  ASitus_EXPORT vtkRenderer*
    GetRenderer() const;

  ASitus_EXPORT vtkRenderWindow*
    GetRenderWindow() const;

  ASitus_EXPORT void
    Initialize(QWidget*   theWidget,
               const bool theIsOffscreen = false);

  ASitus_EXPORT QVTKWidget*
    GetQVTKWidget() const;

  ASitus_EXPORT vtkInteractorStyle*
    GetDefaultInteractorStyle() const;

  ASitus_EXPORT vtkAxesActor*
    GetTrihedron() const;

  ASitus_EXPORT vtkSmartPointer<vtkPropCollection>
    PropsByTrihedron() const;

// Auxiliary methods:
private:

  void adjustTrihedron();
  void actualizeShapeSelectionMode();

// Presentation management:
private:

  //! Shortcut for mapping type between Nodes and Presentations. We use
  //! Node ID as key type in order not to deal with data objects when
  //! storing/retrieving Node Information to/from pipelines.
  typedef NCollection_DataMap<ActAPI_DataObjectId, Handle(visu_prs)> TNodePrsMap;

  //! Mapping between Nodes and their Presentations.
  TNodePrsMap m_nodePresentations;

// Selection management:
private:

  //! Picker to select VTK Props.
  vtkSmartPointer<vtkCellPicker> m_cellPicker;

  //! Picker to select sub-shapes.
  vtkSmartPointer<IVtkTools_ShapePicker> m_shapePicker;

  //! Currently selected Presentations.
  visu_actual_selection m_currentSelection;

// Visualization kernel:
private:

  //! QVTK widget.
  QVTKWidget* m_widget;
  
  //! Active renderer.
  vtkSmartPointer<vtkRenderer> m_renderer;

  //! Render Window.
  vtkSmartPointer<vtkRenderWindow> m_renderWindow;

  //! Render Window Interactor.
  vtkSmartPointer<vtkRenderWindowInteractor> m_renderWindowInteractor;

  //! Default Interactor Style.
  vtkSmartPointer<vtkInteractorStyleTrackballCamera> m_interactorStyleTrackball;

  //! Trihedron actor.
  vtkSmartPointer<vtkAxesActor> m_trihedron;

  //! List of nodes allowed for picking
  Handle(ActAPI_HNodeList) m_bAllowedNodes;

  //! Selection modes.
  int m_iSelectionModes;

  //! Custom Update Presentation callbacks.
  //! They are called in UpdatePresentation() method and
  //! provide possibility to perform custom actions when
  //! presentation manager is updating some of its presentations.
  NCollection_Sequence<unsigned long> m_updateCallbackIds;

};

#endif
