//-----------------------------------------------------------------------------
// Created on: 22 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_pick_facet_callback_h
#define visu_pick_facet_callback_h

// A-Situs (visualization) includes
#include <visu_viewer_callback.h>

// VIS includes
#include <IVtk_Types.hxx>

// VTK includes
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

//! Callback for picking a facet in 3D viewer.
class visu_pick_facet_callback : public visu_viewer_callback
{
public:

  static visu_pick_facet_callback* New();
  static visu_pick_facet_callback* New(gui_viewer* theViewer);
  vtkTypeMacro(visu_pick_facet_callback, visu_viewer_callback);

public:

  virtual void Execute(vtkObject*    theCaller,
                       unsigned long theEventId,
                       void*         theCallData);

private:

  visu_pick_facet_callback  (gui_viewer* theViewer);
  ~visu_pick_facet_callback ();

};

#endif
