//-----------------------------------------------------------------------------
// Created on: 27 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiUI_InteractorStylePick_h
#define asiUI_InteractorStylePick_h

// A-Situs (visualization) includes
#include <asiVisu_Selection.h>

// VTK includes
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSmartPointer.h>

// OCCT includes
#include <NCollection_Sequence.hxx>

//! Class representing a specific VTK Interactor Style propagating picking
//! event for a 3D viewer.
class asiUI_InteractorStylePick : public vtkInteractorStyleTrackballCamera
{
public:

  static asiUI_InteractorStylePick* New();
  vtkTypeMacro(asiUI_InteractorStylePick, vtkInteractorStyleTrackballCamera);

// Customization:
public:

  void
    SetRenderer(const vtkSmartPointer<vtkRenderer>& theRenderer);

  const vtkSmartPointer<vtkRenderer>&
    GetRenderer() const;

  unsigned long
    AddRotationCallback(unsigned long theEventID, vtkCommand* theCallback);

  bool
    RemoveRotationCallback(unsigned long theEventID, unsigned long theTag);

// Overriding:
public:

  virtual void
    OnMouseMove();

  virtual void
    OnLeftButtonDown();

  virtual void
    OnLeftButtonUp();

  virtual void
    EndRotate();

  virtual void
    OnKeyPress();

private:

  //! Copying prohibited.
  asiUI_InteractorStylePick(const asiUI_InteractorStylePick&);

  //! Assignment prohibited.
  void operator=(const asiUI_InteractorStylePick&);

private:

  asiUI_InteractorStylePick();
  ~asiUI_InteractorStylePick();

private:

  //! Instance of active renderer for internal usage.
  vtkSmartPointer<vtkRenderer> m_renderer;

  //! Pick input data.
  asiVisu_PickInput* m_pPickInput;

  //! Indicates whether left mouse button is currently pressed.
  bool m_bIsLeftButtonDown;

  //! Custom callbacks called on camera rotation event.
  NCollection_Sequence<unsigned long> m_rotationCallbackIds;

  //! Currently picked display position.
  int m_PickedPos[2];

};

#endif
