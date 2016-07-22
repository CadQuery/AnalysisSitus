//-----------------------------------------------------------------------------
// Created on: 25 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_prs_manager_h
#define visu_prs_manager_h

// Visualization includes
#include <visu_axes_btn_callback.h>
#include <visu_interactor_style_pick.h>
#include <visu_interactor_style_pick_2d.h>
#include <visu_prs.h>

// Active Data (API) includes
#include <ActAPI_IModel.h>

// VTK includes
#include <vtkAxesActor.h>
#include <vtkButtonWidget.h>
#include <vtkCellPicker.h>
#include <vtkCubeAxesActor.h>
#include <vtkObject.h>
#include <vtkPointPicker.h>
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

  static visu_prs_manager* New();
  void PrintSelf(ostream& os, vtkIndent indent);

public:

  //! Interaction dimensionality.
  enum InteractionMode
  {
    InteractionMode_3D,
    InteractionMode_2D
  };

public:

  static void PlaceButton(vtkButtonWidget* pButton,
                          vtkRenderer*     pRenderer);

  static void CreateImage(vtkSmartPointer<vtkImageData> image,
                          unsigned char*                color1,
                          unsigned char*                color2);

public:

  visu_prs_manager();

// Presentation management:
public:

  virtual void
    Actualize(const Handle(ActAPI_INode)& theNode,
              const bool                  withChildren  = false,
              const bool                  doFitContents = false,
              const bool                  withRepaint   = true);

  virtual void
    Actualize(const Handle(ActAPI_HNodeList)& theNodeList,
              const bool                      withChildren  = false,
              const bool                      doFitContents = false,
              const bool                      withRepaint   = true);

  virtual void
    ActualizeExclusively(const Handle(ActAPI_HNodeList)& theNodeList,
                         const bool                      doFitContents = true);

//-----------------------------------------------------------------------------

  virtual bool
    IsPresented(const Handle(ActAPI_INode)& theNode);

  virtual bool
    IsPresented(const ActAPI_DataObjectId& theNodeId);

//-----------------------------------------------------------------------------

  virtual bool
    SetPresentation(const Handle(ActAPI_INode)& theNode);

//-----------------------------------------------------------------------------

  virtual Handle(visu_prs)
    GetPresentation(const Handle(ActAPI_INode)& theNode);

  virtual Handle(visu_prs)
    GetPresentation(const ActAPI_DataObjectId& theNodeId);

//-----------------------------------------------------------------------------

  virtual void
    InitPresentation(const Handle(ActAPI_INode)& theNode);

  virtual void
    InitPresentation(const ActAPI_DataObjectId& theNodeId);

//-----------------------------------------------------------------------------

  virtual void
    RenderPresentation(const Handle(ActAPI_INode)& theNode);

  virtual void
    RenderPresentation(const ActAPI_DataObjectId& theNodeId);

//-----------------------------------------------------------------------------

  virtual void
    DeRenderPresentation(const Handle(ActAPI_INode)& theNode);

  virtual void
    DeRenderPresentation(const ActAPI_DataObjectId& theNodeId);

  virtual void
    DeRenderAllPresentations();

//-----------------------------------------------------------------------------

  virtual void
    UpdatePresentation(const Handle(ActAPI_INode)& theNode,
                       const bool doFitContents = true);

  virtual void
    UpdatePresentation(const ActAPI_DataObjectId& theNodeId,
                       const bool doFitContents = true);

//-----------------------------------------------------------------------------

  virtual bool
    DeletePresentation(const Handle(ActAPI_INode)& theNode);

  virtual bool
    DeletePresentation(const ActAPI_DataObjectId& theNodeId);

  virtual void
    DeleteAllPresentations();

//-----------------------------------------------------------------------------

// Selection management:
public:

  void
    SetSelectionMode(const int theMode);

  int
    GetSelectionMode() const;

  ActAPI_DataObjectIdList
    Pick(visu_pick_input*            thePickInput,
         const visu_selection_nature theSelNature,
         const bool                  isTopoPicker = true);

  void
    SetPickList(const Handle(ActAPI_HNodeList)& theNodeList);

  Handle(ActAPI_HNodeList)
    GetPickList() const;

  void
    SetPickFromList(const bool isEnabled) const;

  bool
    IsPickFromList() const;

  void
    Highlight(const Handle(ActAPI_HNodeList)& theNodes);

  void
    Highlight(const Handle(ActAPI_INode)& theNode);

  void
    Highlight(const Handle(ActAPI_HNodeList)& theNodes,
              const visu_actor_elem_map&      theActorElems,
              const int                       theModes);

  void
    CleanDetection();

  Handle(ActAPI_HNodeList)
    GetHighlighted() const;

  const visu_actual_selection&
    GetCurrentSelection() const;

// Visualization kernel:
public:

  long int
    AddUpdateCallback(unsigned long theEventID,
                      vtkCommand*   theCallback);

  bool
    RemoveUpdateCallback(unsigned long theEventID,
                         unsigned long theTag);

  void
    SetRenderer(const vtkSmartPointer<vtkRenderer>& theRenderer);

  vtkRenderer*
    GetRenderer() const;

  vtkRenderWindow*
    GetRenderWindow() const;

  void
    Initialize(QWidget*   theWidget,
               const bool theIsOffscreen = false);

  void
    InitializePickers();

  QVTKWidget*
    GetQVTKWidget() const;

  vtkInteractorStyle*
    GetDefaultInteractorStyle() const;

  vtkInteractorStyle*
    GetImageInteractorStyle() const;

  vtkAxesActor*
    GetTrihedron() const;

  vtkSmartPointer<vtkPropCollection>
    PropsByTrihedron() const;

  const vtkSmartPointer<vtkCellPicker>&
    GetCellPicker() const;

  const vtkSmartPointer<IVtkTools_ShapePicker>&
    GetShapePicker() const;

public:

  inline void SetInteractionMode(const InteractionMode mode)
  {
    if ( mode == InteractionMode_3D )
      m_renderWindowInteractor->SetInteractorStyle(m_interactorStyleTrackball);
    if ( mode == InteractionMode_2D )
      m_renderWindowInteractor->SetInteractorStyle(m_interactorStyleImage);
  }

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

  //! Picker to select VTK topology (cells).
  vtkSmartPointer<vtkCellPicker> m_cellPicker;

  //! Picker to select VTK geometry (points).
  vtkSmartPointer<vtkPointPicker> m_pointPicker;

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
  vtkSmartPointer<visu_interactor_style_pick> m_interactorStyleTrackball;

  //! Interactor Style for 2D scenes.
  vtkSmartPointer<visu_interactor_style_pick_2d> m_interactorStyleImage;

  //! Axes actor.
  vtkSmartPointer<vtkAxesActor> m_trihedron;

  //! Button to toggle axes.
  vtkSmartPointer<vtkButtonWidget> m_axesButton;

  //! Callback for axes button.
  vtkSmartPointer<visu_axes_btn_callback> m_axesCallback;

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
