//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef visu_viewer_callback_h
#define visu_viewer_callback_h

// A-Situs (visualization) includes
#include <visu_common.h>

// VTK includes
#include <vtkCommand.h>
#include <vtkSmartPointer.h>

class gui_viewer;

//! Base class for all callbacks associated with 3D viewer.
class visu_viewer_callback : public vtkCommand
{
public:

  vtkTypeMacro(visu_viewer_callback, vtkCommand);

public:

  void        SetViewer (gui_viewer* theViewer);
  gui_viewer* Viewer    ();

protected:

  visu_viewer_callback(gui_viewer* theViewer);
  ~visu_viewer_callback();

private:

  //! Host window. Notice that lifetime of a Callback instance can be longer
  //! that the lifetime of the viewer because user may close that window. In
  //! that case make sure that SetWindow(NULL) is called in order to keep the
  //! callbacks in a consistent (not initialized however) states.
  gui_viewer* m_pViewer;

};

#endif
