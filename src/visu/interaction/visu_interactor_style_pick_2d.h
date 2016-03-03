//-----------------------------------------------------------------------------
// Created on: 17 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_interactor_style_pick_2d_h
#define visu_interactor_style_pick_2d_h

// A-Situs (visualization) includes
#include <visu_selection.h>

// VTK includes
#include <vtkInteractorStyleImage.h>
#include <vtkSmartPointer.h>

// OCCT includes
#include <NCollection_Sequence.hxx>

//! Class representing a specific VTK Interactor Style propagating picking
//! event for a 2D viewer.
class visu_interactor_style_pick_2d : public vtkInteractorStyleImage
{
public:

  static visu_interactor_style_pick_2d* New();
  vtkTypeMacro(visu_interactor_style_pick_2d, vtkInteractorStyleImage);

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

private:

  //! Copying prohibited.
  visu_interactor_style_pick_2d(const visu_interactor_style_pick_2d&);

  //! Assignment prohibited.
  void operator=(const visu_interactor_style_pick_2d&);

private:

  visu_interactor_style_pick_2d();
  ~visu_interactor_style_pick_2d();

private:

  //! Instance of active renderer for internal usage.
  vtkSmartPointer<vtkRenderer> m_renderer;

  //! Pick input data.
  visu_pick_input* m_pPickInput;

};

#endif
