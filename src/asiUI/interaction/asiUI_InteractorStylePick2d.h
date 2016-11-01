//-----------------------------------------------------------------------------
// Created on: 17 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiUI_InteractorStylePick2d_h
#define asiUI_InteractorStylePick2d_h

// A-Situs (visualization) includes
#include <asiVisu_Selection.h>

// VTK includes
#include <vtkInteractorStyleImage.h>
#include <vtkSmartPointer.h>

// OCCT includes
#include <NCollection_Sequence.hxx>

//! Class representing a specific VTK Interactor Style propagating picking
//! event for a 2D viewer.
class asiUI_InteractorStylePick2d : public vtkInteractorStyleImage
{
public:

  static asiUI_InteractorStylePick2d* New();
  vtkTypeMacro(asiUI_InteractorStylePick2d, vtkInteractorStyleImage);

// Customization:
public:

  void
    SetRenderer(const vtkSmartPointer<vtkRenderer>& theRenderer);

  vtkSmartPointer<vtkRenderer>
    GetRenderer() const;

// Overriding:
public:

  virtual void
    OnMouseMove();

  virtual void
    OnLeftButtonDown();

  virtual void
    OnKeyPress();

private:

  //! Copying prohibited.
  asiUI_InteractorStylePick2d(const asiUI_InteractorStylePick2d&);

  //! Assignment prohibited.
  void operator=(const asiUI_InteractorStylePick2d&);

private:

  asiUI_InteractorStylePick2d();
  ~asiUI_InteractorStylePick2d();

private:

  //! Instance of active renderer for internal usage.
  vtkSmartPointer<vtkRenderer> m_renderer;

  //! Pick input data.
  asiVisu_PickInput* m_pPickInput;

};

#endif
