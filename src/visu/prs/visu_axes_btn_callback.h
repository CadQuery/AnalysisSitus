//-----------------------------------------------------------------------------
// Created on: 19 May 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_axes_btn_callback_h
#define visu_axes_btn_callback_h

// A-Situs (visualization) includes
#include <visu_prs.h>
#include <visu_utils.h>

// VTK includes
#include <vtkButtonWidget.h>
#include <vtkCommand.h>

//! Callback for toggling axes.
class visu_axes_btn_callback : public vtkCommand
{
public:

  static visu_axes_btn_callback* New();
  vtkTypeMacro(visu_axes_btn_callback, vtkCommand);

  visu_axes_btn_callback() {}

public:

  //! Initializes callback with the axes actor to manage.
  //! \param axes [in] axes actor to set.
  inline void SetAxesActor(const vtkSmartPointer<vtkAxesActor>& axes)
  {
    m_axes = axes;
  }

  //! Initializes callback with the renderer to manage.
  //! \param renderer [in] renderer to set.
  inline void SetRenderer(const vtkSmartPointer<vtkRenderer>& renderer)
  {
    m_renderer = renderer;
  }

public:

  virtual void Execute(vtkObject*    caller,
                       unsigned long eventId,
                       void*         callData);

private:

  vtkSmartPointer<vtkAxesActor> m_axes;     //!< Axes actor.
  vtkSmartPointer<vtkRenderer>  m_renderer; //!< Renderer.

};

#endif
