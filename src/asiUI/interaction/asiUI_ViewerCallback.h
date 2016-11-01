//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_ViewerCallback_h
#define asiVisu_ViewerCallback_h

// A-Situs (visualization) includes
#include <asiVisu.h>

// VTK includes
#include <vtkCommand.h>
#include <vtkSmartPointer.h>

class asiUI_Viewer;

//! Base class for all callbacks associated with 3D viewer.
class asiVisu_ViewerCallback : public vtkCommand
{
public:

  vtkTypeMacro(asiVisu_ViewerCallback, vtkCommand);

public:

  void        SetViewer (asiUI_Viewer* theViewer);
  asiUI_Viewer* Viewer    ();

protected:

  asiVisu_ViewerCallback  (asiUI_Viewer* theViewer);
  ~asiVisu_ViewerCallback ();

private:

  //! Host viewer. Notice that lifetime of a Callback instance can be longer
  //! that the lifetime of the viewer because user may close that window. In
  //! that case make sure that SetWindow(NULL) is called in order to keep the
  //! callbacks in a consistent (not initialized however) states.
  asiUI_Viewer* m_pViewer;

};

#endif
