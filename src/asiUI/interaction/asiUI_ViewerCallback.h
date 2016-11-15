//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiUI_ViewerCallback_h
#define asiUI_ViewerCallback_h

// asiUI includes
#include <asiUI.h>

// VTK includes
#include <vtkCommand.h>
#include <vtkSmartPointer.h>

class asiUI_Viewer;

//! Base class for all callbacks associated with 3D viewer.
class asiUI_ViewerCallback : public vtkCommand
{
public:

  vtkTypeMacro(asiUI_ViewerCallback, vtkCommand);

public:

  asiUI_EXPORT void
    SetViewer(asiUI_Viewer* pViewer);

  asiUI_EXPORT asiUI_Viewer*
    GetViewer();

protected:

  asiUI_EXPORT
    asiUI_ViewerCallback(asiUI_Viewer* pViewer);

  asiUI_EXPORT
    ~asiUI_ViewerCallback();

private:

  //! Host viewer. Notice that lifetime of a Callback instance can be longer
  //! that the lifetime of the viewer because user may close that window. In
  //! that case make sure that SetWindow(NULL) is called in order to keep the
  //! callbacks in a consistent (not initialized however) states.
  asiUI_Viewer* m_pViewer;

};

#endif
