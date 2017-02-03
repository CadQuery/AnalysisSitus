//-----------------------------------------------------------------------------
// Created on: 19 May 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_AxesBtnCallback_h
#define asiVisu_AxesBtnCallback_h

// A-Situs (visualization) includes
#include <asiVisu_Prs.h>
#include <asiVisu_PrsManager.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkButtonWidget.h>
#include <vtkCommand.h>

//! Callback for toggling axes.
class asiVisu_AxesBtnCallback : public vtkCommand
{
public:

  static asiVisu_AxesBtnCallback* New();
  vtkTypeMacro(asiVisu_AxesBtnCallback, vtkCommand);

  asiVisu_AxesBtnCallback() {}

public:

  //! Initializes callback with the axes actor to manage.
  //! \param axes [in] axes actor to set.
  void SetAxesActor(const vtkSmartPointer<vtkAxesActor>& axes)
  {
    m_axes = axes;
  }

  //! Initializes callback with the renderer to manage.
  //! \param renderer [in] renderer to set.
  void SetRenderer(const vtkSmartPointer<vtkRenderer>& renderer)
  {
    m_renderer = renderer;
  }

  //! Initializes callback with the owning presentation manager.
  //! \param manager [in] manager to set.
  void SetManager(const vtkSmartPointer<asiVisu_PrsManager>& manager)
  {
    m_manager = manager;
  }

public:

  virtual void Execute(vtkObject*    caller,
                       unsigned long eventId,
                       void*         callData);

protected:

  void colorizeActors();

private:

  vtkSmartPointer<vtkAxesActor>       m_axes;     //!< Axes actor.
  vtkSmartPointer<vtkRenderer>        m_renderer; //!< Renderer.
  vtkSmartPointer<asiVisu_PrsManager> m_manager;  //!< Presentation manager.

};

#endif
