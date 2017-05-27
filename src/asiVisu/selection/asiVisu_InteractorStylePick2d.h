//-----------------------------------------------------------------------------
// Created on: 17 December 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_InteractorStylePick2d_h
#define asiVisu_InteractorStylePick2d_h

// asiVisu includes
#include <asiVisu_Selection.h>

// VTK includes
#include <vtkInteractorStyleImage.h>
#include <vtkSmartPointer.h>

// OCCT includes
#include <NCollection_Sequence.hxx>

//! Class representing a specific VTK Interactor Style propagating picking
//! event for a 2D viewer.
class asiVisu_InteractorStylePick2d : public vtkInteractorStyleImage
{
public:

  static asiVisu_InteractorStylePick2d* New();
  vtkTypeMacro(asiVisu_InteractorStylePick2d, vtkInteractorStyleImage);

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

public:

  void GetPickedPos(double& x, double& y) const
  {
    x = m_PickedPos[0];
    y = m_PickedPos[1];
  }

private:

  //! Copying prohibited.
  asiVisu_InteractorStylePick2d(const asiVisu_InteractorStylePick2d&);

  //! Assignment prohibited.
  void operator=(const asiVisu_InteractorStylePick2d&);

private:

  asiVisu_InteractorStylePick2d();
  ~asiVisu_InteractorStylePick2d();

private:

  //! Instance of active renderer for internal usage.
  vtkSmartPointer<vtkRenderer> m_renderer;

  //! Pick input data.
  asiVisu_PickInput* m_pPickInput;

  //! Currently picked display position.
  int m_PickedPos[2];

};

#endif
