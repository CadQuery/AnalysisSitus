//-----------------------------------------------------------------------------
// Created on: 27 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef visu_interactor_style_pick_h
#define visu_interactor_style_pick_h

// A-Situs (visualization) includes
#include <visu_selection.h>

// VTK includes
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSmartPointer.h>

// OCCT includes
#include <NCollection_Sequence.hxx>

//! Class representing a specific VTK Interactor Style propagating picking
//! event for a 3D viewer.
class visu_interactor_style_pick : public vtkInteractorStyleTrackballCamera
{
public:

  static visu_interactor_style_pick* New();
  vtkTypeMacro(visu_interactor_style_pick, vtkInteractorStyleTrackballCamera);

// Customization:
public:

  void
    SetRenderer(const vtkSmartPointer<vtkRenderer>& theRenderer);

  vtkSmartPointer<vtkRenderer>
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

private:

  //! Copying prohibited.
  visu_interactor_style_pick(const visu_interactor_style_pick&);

  //! Assignment prohibited.
  void operator=(const visu_interactor_style_pick&);

private:

  visu_interactor_style_pick();
  ~visu_interactor_style_pick();

private:

  //! Instance of active renderer for internal usage.
  vtkSmartPointer<vtkRenderer> m_renderer;

  //! Pick input data.
  visu_pick_input* m_pPickInput;

  //! Indicates whether left mouse button is currently pressed.
  bool m_bIsLeftButtonDown;

  //! Custom callbacks called on camers rotation event.
  NCollection_Sequence<unsigned long> m_rotationCallbackIds;

};

#endif
