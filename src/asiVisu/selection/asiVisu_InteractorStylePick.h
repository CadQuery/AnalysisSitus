//-----------------------------------------------------------------------------
// Created on: 27 November 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_InteractorStylePick_h
#define asiVisu_InteractorStylePick_h

// A-Situs (UI) includes
#include <asiVisu_Selection.h>

// VTK includes
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSmartPointer.h>

// OCCT includes
#include <NCollection_Sequence.hxx>

//! Class representing a specific VTK Interactor Style propagating picking
//! event for a 3D viewer.
class asiVisu_InteractorStylePick : public vtkInteractorStyleTrackballCamera
{
public:

  vtkTypeMacro(asiVisu_InteractorStylePick, vtkInteractorStyleTrackballCamera);

  asiVisu_EXPORT static asiVisu_InteractorStylePick*
    New();

// Customization:
public:

  asiVisu_EXPORT void
    SetRenderer(const vtkSmartPointer<vtkRenderer>& theRenderer);

  asiVisu_EXPORT const vtkSmartPointer<vtkRenderer>&
    GetRenderer() const;

  asiVisu_EXPORT unsigned long
    AddRotationCallback(unsigned long theEventID, vtkCommand* theCallback);

  asiVisu_EXPORT bool
    RemoveRotationCallback(unsigned long theEventID, unsigned long theTag);

// Overriding:
public:

  asiVisu_EXPORT virtual void
    OnMouseMove();

  asiVisu_EXPORT virtual void
    OnLeftButtonDown();

  asiVisu_EXPORT virtual void
    OnLeftButtonUp();

  asiVisu_EXPORT virtual void
    EndRotate();

  asiVisu_EXPORT virtual void
    OnKeyPress();

public:

  void GetPickedPos(double& x, double& y) const
  {
    x = m_PickedPos[0];
    y = m_PickedPos[1];
  }

private:

  //! Copying prohibited.
  asiVisu_EXPORT asiVisu_InteractorStylePick(const asiVisu_InteractorStylePick&);

  //! Assignment prohibited.
  asiVisu_EXPORT void operator=(const asiVisu_InteractorStylePick&);

private:

  asiVisu_EXPORT asiVisu_InteractorStylePick();
  asiVisu_EXPORT ~asiVisu_InteractorStylePick();

private:

  //! Instance of active renderer for internal usage.
  vtkSmartPointer<vtkRenderer> m_renderer;

  //! Pick input data.
  asiVisu_PickInput* m_pPickInput;

  //! Indicates whether left mouse button is currently pressed.
  bool m_bIsLeftButtonDown;

  //! Indicates whether rotation is in progress.
  bool m_bIsRotation;

  //! Custom callbacks called on camera rotation event.
  NCollection_Sequence<unsigned long> m_rotationCallbackIds;

  //! Currently picked display position.
  int m_PickedPos[2];

};

#endif
